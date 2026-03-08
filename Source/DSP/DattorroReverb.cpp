#include "DattorroReverb.h"
#include <cmath>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================
// CosineOsc
// ============================================================

float DattorroReverb::CosineOsc::value() const
{
    return std::cos(2.0f * static_cast<float>(M_PI) * phase);
}

float DattorroReverb::CosineOsc::next()
{
    phase += freq;
    if (phase >= 1.0f)
        phase -= 1.0f;
    return value();
}

void DattorroReverb::CosineOsc::setFrequency(float f)
{
    freq = f;
}

// ============================================================
// Helpers
// ============================================================

int DattorroReverb::nextPow2(int n)
{
    int v = 1;
    while (v < n) v <<= 1;
    return v;
}

int DattorroReverb::scaleTap(int refTap) const
{
    return std::max(1, static_cast<int>(
        std::round(refTap * ratioScale_)));
}

// ============================================================
// Lifecycle
// ============================================================

DattorroReverb::DattorroReverb()
{
    prepare(48000.0);
}

void DattorroReverb::prepare(double sampleRate)
{
    sampleRate_ = sampleRate;
    ratioScale_ = static_cast<float>(sampleRate / 32000.0);

    // Scale all taps
    tapAp1_   = scaleTap(kRefAp1);
    tapAp2_   = scaleTap(kRefAp2);
    tapAp3_   = scaleTap(kRefAp3);
    tapAp4_   = scaleTap(kRefAp4);
    tapDap1a_ = scaleTap(kRefDap1a);
    tapDap1b_ = scaleTap(kRefDap1b);
    tapDel1_  = scaleTap(kRefDel1);
    tapDap2a_ = scaleTap(kRefDap2a);
    tapDap2b_ = scaleTap(kRefDap2b);
    tapDel2_  = scaleTap(kRefDel2);

    // Compute delay line bases (each line gets length+1 slots)
    baseAp1_   = 0;
    baseAp2_   = baseAp1_   + tapAp1_   + 1;
    baseAp3_   = baseAp2_   + tapAp2_   + 1;
    baseAp4_   = baseAp3_   + tapAp3_   + 1;
    baseDap1a_ = baseAp4_   + tapAp4_   + 1;
    baseDap1b_ = baseDap1a_ + tapDap1a_ + 1;
    baseDel1_  = baseDap1b_ + tapDap1b_ + 1;
    baseDap2a_ = baseDel1_  + tapDel1_  + 1;
    baseDap2b_ = baseDap2a_ + tapDap2a_ + 1;
    baseDel2_  = baseDap2b_ + tapDap2b_ + 1;

    int totalNeeded = baseDel2_ + tapDel2_ + 2;
    bufferSize_ = nextPow2(totalNeeded);
    bufferMask_ = bufferSize_ - 1;

    buffer_ = std::make_unique<float[]>(bufferSize_);
    clear();

    // Scale smear / modulation offsets
    smearRead_   = 10.0f  * ratioScale_;
    smearAmp_    = 60.0f  * ratioScale_;
    smearWrite_  = scaleTap(100);
    loopModRead_ = 4680.0f * ratioScale_;
    loopModAmp_  = 100.0f  * ratioScale_;

    // LFO: base frequencies are per-sample at 32kHz -> scale
    float lfoScale = 1.0f / static_cast<float>(sampleRate);
    lfo_[0].setFrequency(0.5f * lfoScale);
    lfo_[1].setFrequency(0.3f * lfoScale);

    lpDecay1_ = 0.0f;
    lpDecay2_ = 0.0f;
}

void DattorroReverb::clear()
{
    if (buffer_)
        std::memset(buffer_.get(), 0, sizeof(float) * bufferSize_);
    writePtr_ = 0;
    lpDecay1_ = 0.0f;
    lpDecay2_ = 0.0f;
}

void DattorroReverb::setModSpeed(float speed)
{
    float scale = 0.2f + speed * 1.6f;
    float lfoScale = 1.0f / static_cast<float>(sampleRate_);
    lfo_[0].setFrequency(0.5f * lfoScale * scale);
    lfo_[1].setFrequency(0.3f * lfoScale * scale);
}

// ============================================================
// Context helpers
// ============================================================

void DattorroReverb::ctxLoad(float value)
{
    accumulator_ = value;
}

void DattorroReverb::ctxRead(float value, float scale)
{
    accumulator_ += value * scale;
}

void DattorroReverb::ctxReadTail(int base, int length, float scale)
{
    float r = buffer_[(writePtr_ + base + length - 1) & bufferMask_];
    previousRead_ = r;
    accumulator_ += r * scale;
}

void DattorroReverb::ctxInterpolate(int base, float offset, float scale)
{
    int intPart = static_cast<int>(offset);
    float frac = offset - static_cast<float>(intPart);
    float a = buffer_[(writePtr_ + base + intPart) & bufferMask_];
    float b = buffer_[(writePtr_ + base + intPart + 1) & bufferMask_];
    float x = a + (b - a) * frac;
    previousRead_ = x;
    accumulator_ += x * scale;
}

void DattorroReverb::ctxInterpolateLfo(
    int base, float offset, int lfoIdx,
    float amplitude, float scale)
{
    float modOffset = offset + amplitude * lfoValue_[lfoIdx];
    ctxInterpolate(base, modOffset, scale);
}

void DattorroReverb::ctxWrite(float& value, float scale)
{
    value = accumulator_;
    accumulator_ *= scale;
}

void DattorroReverb::ctxWriteDelay(int base, int offset, float scale)
{
    buffer_[(writePtr_ + base + offset) & bufferMask_] = accumulator_;
    accumulator_ *= scale;
}

void DattorroReverb::ctxWriteAllPass(int base, int offset, float scale)
{
    buffer_[(writePtr_ + base + offset) & bufferMask_] = accumulator_;
    accumulator_ *= scale;
    accumulator_ += previousRead_;
}

void DattorroReverb::ctxLp(float& state, float coefficient)
{
    state += coefficient * (accumulator_ - state);
    accumulator_ = state;
}

// ============================================================
// Process — Dattorro plate reverb (Clouds topology)
// ============================================================

void DattorroReverb::process(float* inOutL, float* inOutR, int numSamples)
{
    const float kap  = diffusion_;
    const float klp  = lp_;
    const float krt  = reverbTime_;
    const float amt  = amount_;
    const float gain = inputGain_;

    for (int i = 0; i < numSamples; ++i)
    {
        // Advance write pointer
        --writePtr_;
        if (writePtr_ < 0) writePtr_ += bufferSize_;

        // Update LFO every 32 samples (Clouds behaviour)
        if ((writePtr_ & 31) == 0)
        {
            lfoValue_[0] = lfo_[0].next();
            lfoValue_[1] = lfo_[1].next();
        }

        accumulator_ = 0.0f;
        previousRead_ = 0.0f;

        float wet;
        float apout = 0.0f;

        // --- Smear AP1 inside the loop ---
        ctxInterpolateLfo(baseAp1_, smearRead_, 0, smearAmp_, 1.0f);
        ctxWriteDelay(baseAp1_, smearWrite_, 0.0f);

        // --- Read input (mono sum) ---
        ctxLoad(0.0f);
        ctxRead(inOutL[i] + inOutR[i], gain);

        // --- Diffuse through 4 allpasses ---
        ctxReadTail(baseAp1_, tapAp1_, kap);
        ctxWriteAllPass(baseAp1_, 0, -kap);

        ctxReadTail(baseAp2_, tapAp2_, kap);
        ctxWriteAllPass(baseAp2_, 0, -kap);

        ctxReadTail(baseAp3_, tapAp3_, kap);
        ctxWriteAllPass(baseAp3_, 0, -kap);

        ctxReadTail(baseAp4_, tapAp4_, kap);
        ctxWriteAllPass(baseAp4_, 0, -kap);

        ctxWrite(apout, 0.0f);

        // --- Loop side A ---
        ctxLoad(apout);
        ctxInterpolateLfo(baseDel2_, loopModRead_, 1,
                          loopModAmp_, krt);
        ctxLp(lpDecay1_, klp);

        ctxReadTail(baseDap1a_, tapDap1a_, -kap);
        ctxWriteAllPass(baseDap1a_, 0, kap);

        ctxReadTail(baseDap1b_, tapDap1b_, kap);
        ctxWriteAllPass(baseDap1b_, 0, -kap);

        ctxWriteDelay(baseDel1_, 0, 2.0f);
        ctxWrite(wet, 0.0f);

        inOutL[i] += (wet - inOutL[i]) * amt;

        // --- Loop side B ---
        ctxLoad(apout);
        ctxReadTail(baseDel1_, tapDel1_, krt);
        ctxLp(lpDecay2_, klp);

        ctxReadTail(baseDap2a_, tapDap2a_, kap);
        ctxWriteAllPass(baseDap2a_, 0, -kap);

        ctxReadTail(baseDap2b_, tapDap2b_, -kap);
        ctxWriteAllPass(baseDap2b_, 0, kap);

        ctxWriteDelay(baseDel2_, 0, 2.0f);
        ctxWrite(wet, 0.0f);

        inOutR[i] += (wet - inOutR[i]) * amt;
    }
}
