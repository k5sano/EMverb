#pragma once

#include <cstring>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <memory>

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

    void prepare(double sampleRate);
    void clear();
    void process(float* inOutL, float* inOutR, int numSamples);

    void setAmount(float amount)       { amount_ = amount; }
    void setInputGain(float gain)      { inputGain_ = gain; }
    void setDecay(float time)          { reverbTime_ = time; }
    void setDiffusion(float diffusion) { diffusion_ = diffusion; }
    void setLp(float lp)              { lp_ = lp; }
    void setModSpeed(float speed);
    void setTanhEnabled(bool enabled)   { tanhEnabled_ = enabled; }
    void setTanhThreshold(float thresh) { tanhThreshold_ = thresh; }

private:
    // 32kHz reference tap lengths (Clouds original)
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

    int tapAp1_ = 0, tapAp2_ = 0, tapAp3_ = 0, tapAp4_ = 0;
    int tapDap1a_ = 0, tapDap1b_ = 0, tapDel1_ = 0;
    int tapDap2a_ = 0, tapDap2b_ = 0, tapDel2_ = 0;

    int baseAp1_ = 0, baseAp2_ = 0, baseAp3_ = 0, baseAp4_ = 0;
    int baseDap1a_ = 0, baseDap1b_ = 0, baseDel1_ = 0;
    int baseDap2a_ = 0, baseDap2b_ = 0, baseDel2_ = 0;

    std::unique_ptr<float[]> buffer_;
    int bufferSize_ = 0;
    int bufferMask_ = 0;
    int writePtr_   = 0;

    float amount_     = 0.5f;
    float inputGain_  = 1.0f;
    float reverbTime_ = 0.5f;
    float diffusion_  = 0.625f;
    float lp_         = 0.7f;
    bool  tanhEnabled_   = true;
    float tanhThreshold_ = 1.0f;

    float lpDecay1_ = 0.0f;
    float lpDecay2_ = 0.0f;

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
    float ratioScale_  = 1.5f;

    float smearReadBase_ = 10.0f;
    float smearAmp_      = 60.0f;
    int   smearWriteOfs_ = 100;
    float loopModBase_   = 4680.0f;
    float loopModAmp_    = 100.0f;

    float acc_      = 0.0f;
    float prevRead_ = 0.0f;

    inline float bufRead(int index) const
    {
        return buffer_[index & bufferMask_];
    }
    inline void bufWrite(int index, float value)
    {
        buffer_[index & bufferMask_] = value;
    }

    float interpRead(int base, int maxLen, float offset) const;

    inline float saturate(float x, float drive = 0.7f) {
        return std::tanh(x * drive) / drive;
    }

    static int nextPow2(int n);
    int scaleTap(int refTap) const;
};
