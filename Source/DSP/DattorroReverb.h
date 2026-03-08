#pragma once

#include #include #include #include

/// Dattorro plate reverb — standalone reimplementation of /// Mutable Instruments Clouds reverb (clouds/dsp/fx/reverb.h). /// No external dependencies (eurorack / stmlib). /// Internal sample rate: 32 kHz. class DattorroReverb { public: DattorroReverb(); ~DattorroReverb() = default;

```
void init();
void clear();

/// Process interleaved L/R frames in-place.
/// @param inOutL  left channel buffer  (numSamples floats)
/// @param inOutR  right channel buffer (numSamples floats)
/// @param numSamples  number of sample frames
void process(float* inOutL, float* inOutR, int numSamples);

// --- Parameter setters ---
void setAmount(float amount)       { amount_ = amount; }
void setInputGain(float gain)      { inputGain_ = gain; }
void setDecay(float time)          { reverbTime_ = time; }
void setDiffusion(float diffusion) { diffusion_ = diffusion; }
void setLp(float lp)              { lp_ = lp; }
void setModSpeed(float speed);
```

private: // --- Delay buffer (Clouds uses 16384 with 12-bit compression, // we use float32 for quality) --- static constexpr int kBufferSize = 16384; static constexpr int kBufferMask = kBufferSize - 1; float buffer\_\[kBufferSize\] = {}; int writePtr\_ = 0;

```
// --- Parameters ---
float amount_     = 0.5f;
float inputGain_  = 1.0f;
float reverbTime_ = 0.5f;
float diffusion_  = 0.625f;
float lp_         = 0.7f;

// --- State ---
float lpDecay1_ = 0.0f;
float lpDecay2_ = 0.0f;

// --- LFO (cosine oscillator, simplified) ---
struct CosineOsc {
    float phase = 0.0f;
    float freq  = 0.0f;
    float value() const;
    float next();
    void setFrequency(float f);
};
CosineOsc lfo_[2];
float lfoValue_[2] = {};

// --- Delay line helpers ---
// Delay line layout (same tap lengths as Clouds)
static constexpr int kAp1Base = 0;
static constexpr int kAp1Len  = 113;
static constexpr int kAp2Base = kAp1Base + kAp1Len + 1;
static constexpr int kAp2Len  = 162;
static constexpr int kAp3Base = kAp2Base + kAp2Len + 1;
static constexpr int kAp3Len  = 241;
static constexpr int kAp4Base = kAp3Base + kAp3Len + 1;
static constexpr int kAp4Len  = 399;
static constexpr int kDap1aBase = kAp4Base + kAp4Len + 1;
static constexpr int kDap1aLen  = 1653;
static constexpr int kDap1bBase = kDap1aBase + kDap1aLen + 1;
static constexpr int kDap1bLen  = 2038;
static constexpr int kDel1Base  = kDap1bBase + kDap1bLen + 1;
static constexpr int kDel1Len   = 3411;
static constexpr int kDap2aBase = kDel1Base + kDel1Len + 1;
static constexpr int kDap2aLen  = 1913;
static constexpr int kDap2bBase = kDap2aBase + kDap2aLen + 1;
static constexpr int kDap2bLen  = 1663;
static constexpr int kDel2Base  = kDap2bBase + kDap2bLen + 1;
static constexpr int kDel2Len   = 4782;

// Read from delay line at given base+offset
float readDelay(int base, int offset) const;
// Read with linear interpolation
float readDelayInterp(int base, float offset) const;
// Write to delay line at given base+offset
void writeDelay(int base, int offset, float value);
// Read from delay line tail (last sample)
float readTail(int base, int length) const;

// Accumulator-based processing helpers (mirroring FxEngine::Context)
float accumulator_   = 0.0f;
float previousRead_  = 0.0f;

void ctxLoad(float value);
void ctxRead(float value, float scale);
void ctxReadDelay(int base, int offset, float scale);
void ctxReadTail(int base, int length, float scale);
void ctxInterpolate(int base, float offset, float scale);
void ctxInterpolateLfo(int base, float offset, int lfoIdx,
                       float amplitude, float scale);
void ctxWrite(float& value, float scale);
void ctxWriteDelay(int base, int offset, float scale);
void ctxWriteAllPass(int base, int offset, float scale);
void ctxLp(float& state, float coefficient);
void ctxAdvanceWritePtr();
```

};