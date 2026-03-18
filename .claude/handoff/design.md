# EMVerb 設計書

## アーキテクチャ概要

```
AudioInput (L+R)
     │
     ▼
[Input Gain]
     │
     ▼
[Pre-diffusion: 4段 AllPass Filter]
  AP1(113) → AP2(162) → AP3(241) → AP4(399)  ← LFO smear付き
     │
     ▼
┌────────────────────────────────────────────┐
│  Tank Loop A                               │◄──────────────┐
│  Del2フィードバック×krt                    │               │
│  → SVF LPF (damping)                       │               │
│  → DAP1a(1653) → DAP1b(2038)               │               │
│  → Del1(3411)  ← LFO mod A                 │               │
└─────────────────┬──────────────────────────┘               │
                  │ Del1 tap → Left Out                       │
                  ▼                                           │
┌────────────────────────────────────────────┐               │
│  Tank Loop B                               │───────────────┘
│  Del1フィードバック×krt                    │
│  → SVF LPF (damping)                       │
│  → DAP2a(1913) → DAP2b(1663)               │
│  → Del2(4782)  ← LFO mod B                 │
└─────────────────┬──────────────────────────┘
                  │ Del2 tap → Right Out
                  ▼
[Wet/Dry Mix (amount)]
     │
     ▼
AudioOutput (L+R)
```

## DSPクラス設計

### `DattorroReverb` クラス（改善後）

```cpp
Copyclass DattorroReverb {
public:
    void prepare(double sampleRate);
    void clear();
    void process(float* inOutL, float* inOutR, int numSamples);

    // パラメータセッター（スレッドセーフ: atomic経由で呼ぶこと）
    void setAmount(float amount);
    void setInputGain(float gain);
    void setDecay(float time);
    void setDiffusion(float diffusion);
    void setLp(float lp);
    void setModSpeed(float speed);

private:
    // --- SVF (State Variable Filter) ---
    // タンク内LPFをSVFに置き換える
    struct SVF {
        float low = 0.0f, band = 0.0f;
        float process(float in, float cutoff_norm, float q = 0.5f);
        void reset() { low = band = 0.0f; }
    };
    SVF svfA_, svfB_;  // ループA・B それぞれ独立

    // --- LFO ---
    // 毎サンプル更新に変更（32サンプルごと更新を廃止）
    struct CosineOsc {
        float phase = 0.0f, freq = 0.0f;
        float next();           // 毎サンプル呼ぶ
        void setFrequency(float f);
    };
    CosineOsc lfo_[2];  // lfo_[0]: LoopA用, lfo_[1]: LoopB用

    // --- タンク内サチュレーション ---
    // tanhベースのソフトサチュレーション、ループ内APF通過後に適用
    inline float saturate(float x, float drive = 0.8f) {
        return std::tanh(x * drive) / drive;
    }
};
Copy
```

## 改善タスク詳細

### Task 1: LFO毎サンプル更新化

**変更ファイル:** `DattorroReverb.cpp`

`process()`内の以下のブロックを削除し、サンプルループの先頭で毎サンプル更新する。

```cpp
Copy// 削除対象
if ((writePtr_ & 31) == 0) {
    lfoValue_[0] = lfo_[0].next();
    lfoValue_[1] = lfo_[1].next();
}

// 置き換え（ループ先頭に配置）
lfoValue_[0] = lfo_[0].next();
lfoValue_[1] = lfo_[1].next();
```

### Task 2: タンク内LPFをSVFに置き換え

**変更ファイル:** `DattorroReverb.h`, `DattorroReverb.cpp`

既存の `lpDecay1_`, `lpDecay2_` による1次IIRを削除し、`SVF svfA_`, `SVF svfB_` に置き換える。

SVFのカットオフは `damping` パラメータ（0〜1）から以下の式でマッピングする。

fcutoff​\=200×(20020000​)damping

これにより `damping=0` で200Hz、`damping=1` で20kHzとなる対数スケールになる。

```cpp
Copy// SVF process実装例
float SVF::process(float in, float cutoff_norm, float q) {
    // cutoff_norm: 0〜1 → 0〜π にマップ
    float f = 2.0f * std::sin(juce::MathConstants<float>::pi * cutoff_norm);
    f = std::min(f, 0.99f); // 安定性のためクランプ
    band += f * (in - low - q * band);
    low  += f * band;
    return low;
}
```

### Task 3: Loop AへのLFOモジュレーション追加

**変更ファイル:** `DattorroReverb.h`, `DattorroReverb.cpp`

現状Loop Aでは `interpRead` によるモジュレーションがなく固定読み取りになっている。 `lfo_[0]` をLoop AのDel1読み取りに適用する。

```cpp
Copy// Loop A内 Del1書き込み前に追加
float loopAModOfs = loopModBase_ * 0.7f + loopModAmp_ * 0.7f * lfoValue_[0];
// Del1への書き込み前にinterpReadでモジュレートされた値を使う
```

### Task 4: タンクループ内サチュレーション追加

**変更ファイル:** `DattorroReverb.cpp`

各ループのAPF通過後、`bufWrite` の前に `saturate()` を適用する。 `drive` パラメータは固定値（0.7f）から始め、将来的に外部パラメータ化する。

```cpp
Copy// Loop A: DAP1b通過後
acc_ = saturate(acc_, 0.7f);
bufWrite(writePtr_ + baseDel1_, acc_);

// Loop B: DAP2b通過後
acc_ = saturate(acc_, 0.7f);
bufWrite(writePtr_ + baseDel2_, acc_);
```

### Task 5: Soft clampの移動

**変更ファイル:** `DattorroReverb.cpp`

wet/dry合成後のclampを削除し、代わりにTask 4のサチュレーションで制御する。 最終出力には `-1.0f〜1.0f` のソフトクリップのみ残す。

```cpp
Copy// 削除
inOutL[i] = std::max(-4.0f, std::min(4.0f, inOutL[i]));
inOutR[i] = std::max(-4.0f, std::min(4.0f, inOutR[i]));

// 置き換え（tanh系ソフトクリップ）
inOutL[i] = std::tanh(inOutL[i]);
inOutR[i] = std::tanh(inOutR[i]);
```

## テスト方針

-   `Tests/` 以下にCatch2またはJUCEのUnitTestBaseを使ったDSPユニットテストを追加する
-   無音入力で `process()` を呼んだ時に出力が0に収束することを確認（安定性テスト）
-   1kHz正弦波入力でDecay=0.99, Damping=0.5のとき出力がクリップしないことを確認