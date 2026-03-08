#pragma once

#include <cstring>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

/// Dattorro plate reverb — standalone reimplementation of
/// Mutable Instruments Clouds reverb (clouds/dsp/fx/reverb.h).
/// No external dependencies (eurorack / stmlib).
///
/// Runs at any host sample rate. Delay tap lengths are scaled
/// from the 32kHz reference values by (sampleRate / 32000).
class DattorroReverb {
public:
    DattorroReverb();
    ~DattorroReverb() = default;

    /// Call once before processing. Allocates delay buffer scaled
    /// for the given sample rate.
    void prepare(double sampleRate);

    /// Reset all internal state to silence (keeps buffer allocation).
    void clear();

    /// Process L/R buffers in-place.
    void process(float* inOutL, float* inOutR, int numSamples);

    // --- Parameter setters (call before process or per-block) ---
    void setAmount(float amount)       { amount_ = amount; }
    void setInputGain(float gain)      { inputGain_ = gain; }
    void setDecay(float time)          { reverbTime_ = time; }
    void setDiffusion(float diffusion) { diffusion_ = diffusion; }
    void setLp(float lp)              { lp_ = lp; }
    void setModSpeed(float speed);

private:
    // --- 32kHz reference tap lengths (Clouds original) ---
    static constexpr int kRefAp1   = 113;
    static constexpr int kRefAp2   = 162;
    static constexpr int kRefAp3   = 241;
    static constexpr int kRefAp4   = 399;
    static constexpr int kRefDap1a = 1653;
    static constexpr int kRefDap1b = 2038;
    static constexpr int kRefDel1  = 3411;
    static constexpr int kRefDap2a = 1913;
    static constexpr int kRefDap2b = 1663;
    static constexpr int kRefDel2  = 4782;

    // --- Scaled tap lengths (set in prepare()) ---
    int tapAp1_   = kRefAp1;
    int tapAp2_   = kRefAp2;
    int tapAp3_   = kRefAp3;
    int tapAp4_   = kRefAp4;
    int tapDap1a_ = kRefDap1a;
    int tapDap1b_ = kRefDap1b;
    int tapDel1_  = kRefDel1;
    int tapDap2a_ = kRefDap2a;
    int tapDap2b_ = kRefDap2b;
    int tapDel2_  = kRefDel2;

    // --- Delay line bases (computed in prepare()) ---
    int baseAp1_   = 0;
    int baseAp2_   = 0;
    int baseAp3_   = 0;
    int baseAp4_   = 0;
    int baseDap1a_ = 0;
    int baseDap1b_ = 0;
    int baseDel1_  = 0;
    int baseDap2a_ = 0;
    int baseDap2b_ = 0;
    int baseDel2_  = 0;

    // --- Delay buffer ---
    std::unique_ptr<float[]> buffer_;
    int bufferSize_ = 0;
    int bufferMask_ = 0;
    int writePtr_   = 0;

    // --- Parameters ---
    float amount_     = 0.5f;
    float inputGain_  = 1.0f;
    float reverbTime_ = 0.5f;
    float diffusion_  = 0.625f;
    float lp_         = 0.7f;

    // --- LP decay state ---
    float lpDecay1_ = 0.0f;
    float lpDecay2_ = 0.0f;

    // --- Cosine LFO ---
    struct CosineOsc {
        float phase = 0.0f;
        float freq  = 0.0f;
        float value() const;
        float next();
        void setFrequency(float f);
    };
    CosineOsc lfo_[2];
    float lfoValue_[2] = {};

    double sampleRate_ = 48000.0;
    float ratioScale_  = 1.0f;

    // --- Smear tap offsets (scaled) ---
    float smearRead_   = 10.0f;
    float smearAmp_    = 60.0f;
    int   smearWrite_  = 100;
    float loopModRead_ = 4680.0f;
    float loopModAmp_  = 100.0f;

    // --- Context-style processing helpers ---
    float accumulator_  = 0.0f;
    float previousRead_ = 0.0f;

    void ctxLoad(float value);
    void ctxRead(float value, float scale);
    void ctxReadTail(int base, int length, float scale);
    void ctxInterpolate(int base, float offset, float scale);
    void ctxInterpolateLfo(int base, float offset, int lfoIdx,
                           float amplitude, float scale);
    void ctxWrite(float& value, float scale);
    void ctxWriteDelay(int base, int offset, float scale);
    void ctxWriteAllPass(int base, int offset, float scale);
    void ctxLp(float& state, float coefficient);

    static int nextPow2(int n);
    int scaleTap(int refTap) const;
};
