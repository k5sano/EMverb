# Claude Code 実装タスク

## 優先順位

1.  CMakeLists.txt を作成（juce\_add\_plugin 使用）
2.  Parameters.h にパラメータ定義（全6パラメータ）
3.  DSP クラスの実装 a. DattorroReverb (Dattorro plate reverb)
4.  SampleRateAdapter の実装 (32kHz 変換)
5.  PluginProcessor の実装
6.  PluginEditor の実装（ジェネリックエディタ、400×300）
7.  ビルド確認
8.  テストコード作成 (Tests/ 配下)

## 注意事項

-   processBlock 内で new/delete 禁止
-   APVTS は PluginProcessor のコンストラクタで初期化
-   エディタサイズは 400×300 で固定（ジェネリックエディタ）
-   内部リバーブ処理は float (Clouds 準拠)
-   1関数 50行以内、1ファイル 300行以内を目安

## 完了条件

-   `cmake --build build/` が成功する
-   VST3 プラグインが生成される
-   全6パラメータが APVTS に登録され仕様通りに動作する