```

まず、リポジトリの構造と内容を確認させてください。

ツールを使用する

|

読み取り

[https://github.com/k5sano/CLOUDSVST](https://github.com/k5sano/CLOUDSVST)

エージェントを表示

GitHub のページ自体はうまく取れないようです。Raw ファイルを直接取得してみましょう。

ツールを使用する

|

検索

github k5sano CLOUDSVST repository files structure

エージェントを表示

ツールを使用する

|

読み取り

[https://github.com/k5sano/CLOUDSVST/tree/main](https://github.com/k5sano/CLOUDSVST/tree/main)

エージェントを表示

GitHub のHTML取得がうまくいかないので、raw ファイルURLを直接試してみます。

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/README.md](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/README.md)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/CMakeLists.txt](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/CMakeLists.txt)

エージェントを表示

CMakeLists.txt は取得できました。次にソースファイルを取得していきましょう。

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginProcessor.h](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginProcessor.h)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginProcessor.cpp](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginProcessor.cpp)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginEditor.h](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginEditor.h)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginEditor.cpp](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginEditor.cpp)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/Parameters.h](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/Parameters.h)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/DSP/MT2GainStage.h](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/DSP/MT2GainStage.h)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/DSP/MT2GainStage.cpp](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/DSP/MT2GainStage.cpp)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/DSP/MT2ToneStack.h](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/DSP/MT2ToneStack.h)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/DSP/MT2ToneStack.cpp](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/DSP/MT2ToneStack.cpp)

エージェントを表示

Parameters.h が取得できました。これは実は Clouds（Mutable Instruments のグラニュラーシンセ）のVSTクローンですね。リバーブパラメータが含まれています。他のDSPファイルも見てみましょう。

ツールを使用する

|

検索

site:github.com/k5sano/CLOUDSVST Source DSP

エージェントを表示

ツールを使用する

|

検索

"k5sano" "CLOUDSVST" github

エージェントを表示

Let me try to access the API to get the full repo tree.

ツールを使用する

|

読み取り

[https://api.github.com/repos/k5sano/CLOUDSVST/git/trees/main?recursive=1](https://api.github.com/repos/k5sano/CLOUDSVST/git/trees/main?recursive=1)

エージェントを表示

ツリー構造が把握できました。今度は主要なソースファイルを取得します。

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/CloudsEngine.h](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/CloudsEngine.h)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/CloudsEngine.cpp](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/CloudsEngine.cpp)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/SampleRateAdapter.h](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/SampleRateAdapter.h)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/SampleRateAdapter.cpp](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/SampleRateAdapter.cpp)

エージェントを表示

Now let me get the remaining key files:

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginProcessor.h](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginProcessor.h)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginProcessor.cpp](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginProcessor.cpp)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginEditor.h](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginEditor.h)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginEditor.cpp](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/Source/PluginEditor.cpp)

エージェントを表示

PluginProcessor.h/.cpp が MetalCosmos（MT2）のものが表示されていますが、Parameters.h は CloudsVST のものでした。CMakeLists.txt にも MetalCosmos と書かれています。これはどうやらリポジトリにClouds関連の実際のSourceと、scaffold（MetalCosmos用テンプレ）が混在しているようです。CLOUDSVSTの実際のProcessor/Editorはsource直下にあるはずですが、GitHub上のキャッシュが2つの異なるプロジェクトを返しているようです。

CLOUDSVSTの真のPluginProcessorを確認するために、blobのSHA値を使ってAPIから取得してみましょう。

ツールを使用する

|

読み取り

[https://api.github.com/repos/k5sano/CLOUDSVST/git/blobs/c025d067b0cadf16f9fb7d74f95c36ce72de0178](https://api.github.com/repos/k5sano/CLOUDSVST/git/blobs/c025d067b0cadf16f9fb7d74f95c36ce72de0178)

エージェントを表示

ツールを使用する

|

読み取り

[https://api.github.com/repos/k5sano/CLOUDSVST/git/blobs/e787ed5ee91e54441487755f129340067f2e6875](https://api.github.com/repos/k5sano/CLOUDSVST/git/blobs/e787ed5ee91e54441487755f129340067f2e6875)

エージェントを表示

GitHubのblob APIはBase64エンコードされています。PluginProcessor.h/cppは確かにMetalCosmosのものですが、CMakeLists.txtにはMetalCosmosが書かれていて、Parameters.hはCloudsVSTのものでした。

これはどうやら、リポジトリ名はCLOUDSVSTですが、scaffoldディレクトリにMetalCosmos用テンプレートがあり、Sourceディレクトリ直下にはCloudsVST固有のファイル（CloudsEngine, SampleRateAdapter, Parameters.h）と、MT2系のファイル（PluginProcessor, PluginEditor + DSP/）が混在しているようです。

CLAUDE.mdとhandoffファイルを確認して、プロジェクトの全体像を把握しましょう。

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/CLAUDE.md](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/CLAUDE.md)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/handoff/spec.md](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/handoff/spec.md)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/handoff/design.md](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/handoff/design.md)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/handoff/tasks.md](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/handoff/tasks.md)

エージェントを表示

状況が完全に把握できました。このリポジトリは実際には2つのプロジェクトが混在しています：

1.  **MetalCosmos** (MT2) — PluginProcessor/Editor, DSP/\* がscaffoldから展開されたもの
2.  **CloudsVST** — CloudsEngine, SampleRateAdapter, Parameters.h（Cloudsのパラメータ）がClouds（Mutable Instruments グラニュラーシンセ）のVSTクローン

CloudsVSTのパラメータにreverbがあり、CloudsEngineの内部でMutable Instrumentsのeurorackライブラリ（`clouds::GranularProcessor`）のリバーブが使われています。

Mutable Instrumentsのcloudsリバーブの仕組みを確認しましょう。

ツールを使用する

|

検索

mutable instruments clouds reverb DSP algorithm

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/pichenettes/eurorack/master/clouds/dsp/fx/reverb.h](https://raw.githubusercontent.com/pichenettes/eurorack/master/clouds/dsp/fx/reverb.h)

エージェントを表示

Cloudsのリバーブの実装が取得できました。Dattorro plate reverbアルゴリズムです。FxEngineも確認しましょう。

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/pichenettes/eurorack/master/clouds/dsp/fx/fx\_engine.h](https://raw.githubusercontent.com/pichenettes/eurorack/master/clouds/dsp/fx/fx_engine.h)

エージェントを表示

Now I have a complete understanding of the Clouds reverb. It uses the Dattorro plate reverb topology with FxEngine, which depends on stmlib. Rather than depending on the entire eurorack/stmlib codebase, I'll implement the same reverb algorithm as a standalone C++ implementation in float, following the same JUCE/APVTS pattern as CLOUDSVST.

Let me also check the `.claude/settings.json` and `.claude/commands/build-check.md` for context:

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/settings.json](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/settings.json)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/commands/build-check.md](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/commands/build-check.md)

エージェントを表示

ツールを使用する

|

読み取り

[https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/commands/build-fix.md](https://raw.githubusercontent.com/k5sano/CLOUDSVST/main/.claude/commands/build-fix.md)

エージェントを表示

これで全体像が完全に把握できました。それでは、CLOUDSVSTリポジトリの構造とルール（CLAUDE.md のルールセット）に準拠して、Cloudsのリバーブのみを抽出した独立VST3プラグインを作成します。

Dattorro plate reverb（Cloudsの`Reverb`クラス）を、外部依存（eurorack/stmlib）なしで自己完結するC++コードとして再実装します。

* * *

以下が全ファイル出力です。

【START:.claude/handoff/spec.md】

# CloudsReverb VST3 プラグイン — 要件仕様書

## 1\. 製品概要

Mutable Instruments Clouds の内蔵リバーブ (Dattorro plate reverb) を抽出し、 単体の VST3 エフェクトプラグインとして動作させる。 JUCE フレームワークで開発。

## 2\. 元アルゴリズム

Clouds の `clouds/dsp/fx/reverb.h` に実装された Griesinger/Dattorro トポロジー。

-   入力段: 4 段の Allpass Diffuser
-   ループ: 2×(2 Allpass + 1 Delay) の循環構造
-   LFO モジュレーション（コーラス的揺らぎ）
-   ローパス減衰

内部サンプルレート: 32000Hz (Clouds オリジナル準拠) ホストサンプルレートとの変換は SampleRateAdapter で行う。

## 3\. シグナルチェーン

```
Input (Stereo)
  │
  ├─ [Input Gain] (dB)
  ├─ [SampleRateAdapter: Downsample to 32kHz]
  ├─ [Dattorro Plate Reverb]
  │    ├─ 4x Allpass Diffuser
  │    ├─ 2x (2AP + Delay) Loop
  │    ├─ LFO Modulation
  │    └─ LP Damping
  ├─ [SampleRateAdapter: Upsample to host rate]
  ├─ [Dry/Wet Mix]
  └─ Output (Stereo)
```

## 4\. パラメータ一覧 (6パラメータ)

| パラメータ名 | ID | 範囲 | デフォルト | 説明 |
| --- | --- | --- | --- | --- |
| Decay | `decay` | 0.0〜1.0 | 0.5 | リバーブ・ディケイ (reverb\_time) |
| Damping | `damping` | 0.0〜1.0 | 0.7 | 高域減衰 LP 係数 |
| Diffusion | `diffusion` | 0.0〜1.0 | 0.625 | Allpass 拡散量 |
| Amount | `amount` | 0.0〜1.0 | 0.5 | Dry/Wet (0.0=Dry, 1.0=Wet) |
| Input Gain | `input_gain` | \-18.0〜+6.0 dB | 0.0 | 入力ゲイン |
| Mod Speed | `mod_speed` | 0.0〜1.0 | 0.5 | LFO 変調速度 |

## 5\. DSP 要件

| 項目 | 仕様 |
| --- | --- |
| 内部サンプルレート | 32000Hz (Clouds 準拠) |
| サンプルレート変換 | Hermite 補間によるリサンプリング |
| ディレイライン合計 | 16384 サンプル (float32) |
| 処理ブロック | 32 サンプル単位 |
| 処理精度 | float (32bit) — Clouds オリジナル準拠 |

## 6\. 技術要件

| 項目 | 仕様 |
| --- | --- |
| フレームワーク | JUCE 7 or 8 |
| プラグイン形式 | VST3, Standalone |
| パラメータ管理 | `AudioProcessorValueTreeState` |
| スレッドセーフ | `getRawParameterValue()` のアトミック読み出し |
| GUI | ジェネリックエディタ (400×300) |
| C++規格 | C++17 以上 |

## 7\. 受け入れ基準

-   AC-01: `cmake --build build/` がエラーなしで成功する
-   AC-02: VST3 プラグインバイナリが生成される
-   AC-03: 全6パラメータが APVTS に登録され仕様通りに動作する
-   AC-04: processBlock 内で動的メモリ確保がない
-   AC-05: 入力信号なし + Decay=0 で出力が無音に収束する
-   AC-06: 32kHz 以外のホストサンプルレートでもノイズなく動作する