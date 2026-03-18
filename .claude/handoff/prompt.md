# EMVerb — Claude Code ハンドオフプロンプト

## あなたの役割

あなたはJUCE C++オーディオプラグイン開発の専門家です。 このリポジトリ `EMVerb` はEarthQuaker Devices的な質感を目指したDattorroプレートリバーブのVST3/AUプラグインです。 以下の仕様書・設計書・現状コードを読み込み、指示された改善タスクを実装してください。

## 最重要制約

-   `processBlock` はリアルタイムスレッドで呼ばれます。**malloc/new/例外/ロック禁止**
-   `std::atomic<float>*` パラメータは `.load()` で読み取り、サンプルループ外でローカル変数にコピーして使うこと
-   すべてのDSPコードは `Source/DSP/` 以下に配置すること
-   JUCEのモジュールは `juce_audio_processors`, `juce_dsp`, `juce_audio_basics` のみ使用可

## 現在のファイル構成

```
Source/
  DSP/
    DattorroReverb.h
    DattorroReverb.cpp
  Parameters.h
  PluginProcessor.h
  PluginProcessor.cpp
  PluginEditor.h
  PluginEditor.cpp
  PresetManager.h
  PresetManager.cpp
```

## 現状コードの既知の問題点（優先度順）

1.  LFOが32サンプルごとにしか更新されず、モジュレーションが粗い
2.  タンク内LPFが1次IIR（Leaky Integrator）のみで音が硬い
3.  Loop AにLFOモジュレーションがかかっていない（Loop Bのみ）
4.  タンクループ内にサチュレーションがなくEQD的な粒感が出ない
5.  Soft clampがwet/dry合成後にかかっており意味をなしていない

## 参照ファイル

`spec.md` および `design.md` を参照してください。