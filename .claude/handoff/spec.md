# EMVerb プラグイン仕様書

## 概要

EarthQuaker Devices的な空間系サウンドを目指したDattorroプレートリバーブVST3/AUプラグイン。 Mutable Instruments CloudsのDSPトポロジーをベースに、EQD的な質感（粒感・揺らぎ・フィルターの色付け）を付加する。

## ターゲット環境

-   フォーマット: VST3 / AU
-   チャンネル: Stereo in / Stereo out
-   サンプルレート: 44100 / 48000 / 88200 / 96000 Hz 対応
-   ビット深度: 32-bit float
-   バッファサイズ: 32〜2048 samples

## パラメータ仕様

| ID | 表示名 | 範囲 | デフォルト | 単位 | 説明 |
| --- | --- | --- | --- | --- | --- |
| `decay` | Decay | 0.0〜1.0 | 0.5 | \- | リバーブの減衰時間。タンクループのフィードバック量に対応 |
| `damping` | Damping | 0.0〜1.0 | 0.7 | \- | タンク内LPFのカットオフ。高域の減衰速度を制御 |
| `diffusion` | Diffusion | 0.0〜1.0 | 0.625 | \- | 入力拡散段とタンク内APFの係数。音の密度感を制御 |
| `amount` | Mix | 0.0〜1.0 | 0.5 | \- | Wet/Dryミックス比 |
| `input_gain` | Input Gain | \-18.0〜+6.0 | 0.0 | dB | 入力ゲイン |
| `mod_speed` | Mod Speed | 0.0〜1.0 | 0.5 | \- | LFO速度。タンク内ディレイ変調の速度を制御 |

## サウンドデザイン目標

-   EarthQuaker Devices（Afterneath / Spectre）的な「粒感」と「揺らぎ」
-   デジタルアーティファクトを音楽的に聴かせるサチュレーション
-   アナログフィルター的な色付けのある高域減衰
-   左右非対称なモジュレーションによる立体感

## 将来追加予定パラメータ（未実装）

-   `shimmer` : ピッチシフトフィードバック（+1oct）
-   `filter_freq` : タンク後段のSVFカットオフ
-   `filter_res` : SVFレゾナンス
-   `env_mod` : エンベロープフォロワー連動のフィルターモジュレーション