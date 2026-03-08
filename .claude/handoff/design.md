# CloudsReverb 設計書

## クラス構成

```
CloudsReverbPlugin : juce::AudioProcessor
├── DattorroReverb (リバーブ DSP コア)
│   ├── AllpassDiffuser ×4 (入力拡散)
│   ├── DelayLine ×6 (ループ用)
│   ├── CosineOscillator ×2 (LFO)
│   └── LP Damping (1次 IIR)
└── SampleRateAdapter (32kHz ↔ ホストレート変換)
```

## オーディオ処理フロー

1.  APVTS から全パラメータを atomic 読み出し
2.  入力ゲイン適用
3.  SampleRateAdapter でホストレート → 32kHz にダウンサンプル
4.  DattorroReverb で処理 (32サンプルブロック単位)
5.  SampleRateAdapter で 32kHz → ホストレートにアップサンプル
6.  Dry/Wet ミックス適用

## ファイル構成

```
Source/
  PluginProcessor.h
  PluginProcessor.cpp
  PluginEditor.h
  PluginEditor.cpp
  Parameters.h
  DSP/
    DattorroReverb.h
    DattorroReverb.cpp
  SampleRateAdapter.h
  SampleRateAdapter.cpp
CMakeLists.txt
Tests/
  ReverbTests.cpp
```

## モジュール責務

### DattorroReverb

Clouds reverb.h のアルゴリズムを外部依存なしで C++ 再実装。 FxEngine のディレイライン・Allpass・LFO 機構を単一クラスに統合。

### SampleRateAdapter

ホストサンプルレート ↔ 32kHz の変換。 CLOUDSVST の SampleRateAdapter.h/.cpp をベースに、 CloudsEngine ではなく DattorroReverb を呼び出す形に改変。

### CloudsReverbPlugin (PluginProcessor)

APVTS による全パラメータ管理、SampleRateAdapter + DattorroReverb のオーケストレーション。