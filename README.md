# EMVerb

Dattorro plate reverb plugin with EarthQuaker Devices-inspired texture. Built with JUCE.

DSP based on [Mutable Instruments Clouds](https://github.com/pichenettes/eurorack) by Emilie Gillet.

## Features

- Dattorro plate reverb (Clouds topology)
- SVF high-cut / low-cut filters (HicutFilt / LocutFilt)
- Per-sample LFO modulation with L/R asymmetry
- Tank saturation (tanh-based)
- Tanh soft clip with on/off toggle and threshold
- Preset manager (save / load / delete)
- Custom background image
- VST3 / AU / Standalone

## Parameters

| Name | Range | Description |
|------|-------|-------------|
| Decay | 0–1 | Reverb tail length |
| HicutFilt | 0–1 | High-frequency damping (SVF LPF, 200Hz–20kHz) |
| LocutFilt | 0–1 | Low-frequency cut (SVF HPF, 20Hz–2kHz) |
| Diffusion | 0–1 | Allpass diffusion density |
| Mix | 0–1 | Wet/dry ratio |
| Input Gain | -18–+6 dB | Input level |
| Mod Speed | 0–1 | LFO modulation rate |
| Tanh | on/off | Output soft clip toggle |
| Threshold | 0.1–2.0 | Soft clip ceiling |

## Install (macOS)

```bash
unzip EMVerb-macOS.zip
cd EMVerb
./install.sh
```

If macOS blocks the plugin, go to **System Settings > Privacy & Security > Allow anyway**.

## Build from source

Requires [JUCE](https://juce.com/) and CMake 3.22+.

```bash
cmake -B build
cmake --build build/
ctest --test-dir build/   # requires Catch2
```

## License

This project is licensed under the **GNU General Public License v3.0** — see [LICENSE](LICENSE) for details.

### Third-party

DSP algorithms are based on **Mutable Instruments Clouds** by Emilie Gillet, licensed under the **MIT License**.

Copyright (c) 2014 Emilie Gillet (emilie.o.gillet@gmail.com)
