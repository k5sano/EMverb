#include "DattorroReverb.h"
#include <cmath>
#include <cstring>
#include <algorithm>

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
    if (phase >= 1.0f) phase -= 1.0f;
    if (phase < 0.0f)  phase += 1.0f;
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

float DattorroReverb::interpRead(int base, int maxLen,
                                  float offset) const
{
    offset = std::max(0.0f,
        std::min(offset, static_cast<float>(maxLen - 2)));
    int intPart = static_cast<int>(offset);
    float frac = offset - static_cast<float>(intPart);
    float a = bufRead(writePtr_ + base + intPart);
    float b = bufRead(writePtr_ + base + intPart + 1);
    return a + (b - a) * frac;
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
    sampleRate_ = std::max(sampleRate, 8000.0);
    ratioScale_ = static_cast<float>(sampleRate_ / 32000.0);

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

    smearReadBase_ = 10.0f  * ratioScale_;
    smearAmp_      = 60.0f  * ratioScale_;
    smearWriteOfs_ = std::min(scaleTap(100), tapAp1_ - 1);
    loopModBase_   = 4680.0f * ratioScale_;
    loopModAmp_    = 100.0f  * ratioScale_;

    // Clamp so LFO max doesn't exceed del2 or del1 range
    int minDelTap = std::min(tapDel1_, tapDel2_);
    float maxMod = loopModBase_ + loopModAmp_;
    if (maxMod >= static_cast<float>(minDelTap - 2))
    {
        float available = static_cast<float>(minDelTap - 2);
        loopModBase_ = available - loopModAmp_;
        if (loopModBase_ < 0.0f)
        {
            loopModBase_ = available * 0.8f;
            loopModAmp_  = available * 0.15f;
        }
    }

    float invSr = 1.0f / static_cast<float>(sampleRate_);
    lfo_[0].setFrequency(0.5f * invSr);
    lfo_[1].setFrequency(0.3f * invSr);

    clear();
}

void DattorroReverb::clear()
{
    if (buffer_)
        std::memset(buffer_.get(), 0,
            sizeof(float) * static_cast<size_t>(bufferSize_));
    writePtr_ = 0;
    svfA_.reset();
    svfB_.reset();
    hpfA_.reset();
    hpfB_.reset();
    lfoValue_[0] = 0.0f;
    lfoValue_[1] = 0.0f;
}

void DattorroReverb::setModSpeed(float speed)
{
    float scale = 0.2f + speed * 1.6f;
    float invSr = 1.0f / static_cast<float>(sampleRate_);
    lfo_[0].setFrequency(0.5f * invSr * scale);
    lfo_[1].setFrequency(0.3f * invSr * scale);
}

// ============================================================
// Process — Dattorro plate reverb (Clouds topology)
// ============================================================

void DattorroReverb::process(float* inOutL, float* inOutR,
                              int numSamples)
{
    const float kap  = diffusion_;
    const float krt  = reverbTime_;
    const float amt  = amount_;
    const float gain = inputGain_;

    // Map damping (0..1) to LPF cutoff: 200Hz at 0, 20kHz at 1 (log scale)
    float lpCutoffHz = 200.0f * std::pow(100.0f, lp_);
    float lpCutoffNorm = lpCutoffHz / static_cast<float>(sampleRate_);
    lpCutoffNorm = std::min(lpCutoffNorm, 0.49f);

    // Map lo_cut (0..1) to HPF cutoff: 20Hz at 0, 2kHz at 1 (log scale)
    float hpCutoffHz = 20.0f * std::pow(100.0f, hp_);
    float hpCutoffNorm = hpCutoffHz / static_cast<float>(sampleRate_);
    hpCutoffNorm = std::min(hpCutoffNorm, 0.49f);

    for (int i = 0; i < numSamples; ++i)
    {
        --writePtr_;
        if (writePtr_ < 0) writePtr_ += bufferSize_;

        lfoValue_[0] = lfo_[0].next();
        lfoValue_[1] = lfo_[1].next();

        acc_ = 0.0f;
        prevRead_ = 0.0f;

        float wet;
        float apout = 0.0f;

        // Smear: read AP1 with LFO, write back
        float smearOfs = smearReadBase_
                       + smearAmp_ * lfoValue_[0];
        acc_ = interpRead(baseAp1_, tapAp1_, smearOfs);
        bufWrite(writePtr_ + baseAp1_ + smearWriteOfs_, acc_);
        acc_ = 0.0f;

        // Input (mono sum)
        acc_ = (inOutL[i] + inOutR[i]) * gain;

        // 4x Allpass diffuser
        prevRead_ = bufRead(writePtr_ + baseAp1_ + tapAp1_ - 1);
        acc_ += prevRead_ * kap;
        bufWrite(writePtr_ + baseAp1_, acc_);
        acc_ *= -kap;
        acc_ += prevRead_;

        prevRead_ = bufRead(writePtr_ + baseAp2_ + tapAp2_ - 1);
        acc_ += prevRead_ * kap;
        bufWrite(writePtr_ + baseAp2_, acc_);
        acc_ *= -kap;
        acc_ += prevRead_;

        prevRead_ = bufRead(writePtr_ + baseAp3_ + tapAp3_ - 1);
        acc_ += prevRead_ * kap;
        bufWrite(writePtr_ + baseAp3_, acc_);
        acc_ *= -kap;
        acc_ += prevRead_;

        prevRead_ = bufRead(writePtr_ + baseAp4_ + tapAp4_ - 1);
        acc_ += prevRead_ * kap;
        bufWrite(writePtr_ + baseAp4_, acc_);
        acc_ *= -kap;
        acc_ += prevRead_;

        apout = acc_;

        // Loop side A
        acc_ = apout;
        float modOfs = loopModBase_
                     + loopModAmp_ * lfoValue_[1];
        acc_ += interpRead(baseDel2_, tapDel2_, modOfs) * krt;

        acc_ = svfA_.process(acc_, lpCutoffNorm);
        acc_ -= hpfA_.process(acc_, hpCutoffNorm);

        prevRead_ = bufRead(writePtr_ + baseDap1a_ + tapDap1a_ - 1);
        acc_ += prevRead_ * (-kap);
        bufWrite(writePtr_ + baseDap1a_, acc_);
        acc_ *= kap;
        acc_ += prevRead_;

        prevRead_ = bufRead(writePtr_ + baseDap1b_ + tapDap1b_ - 1);
        acc_ += prevRead_ * kap;
        bufWrite(writePtr_ + baseDap1b_, acc_);
        acc_ *= -kap;
        acc_ += prevRead_;

        acc_ = saturate(acc_, 0.7f);
        bufWrite(writePtr_ + baseDel1_, acc_);
        wet = acc_;
        inOutL[i] += (wet - inOutL[i]) * amt;

        // Loop side B
        acc_ = apout;
        float modOfsA = loopModBase_ * 0.7f
                      + loopModAmp_ * 0.7f * lfoValue_[0];
        acc_ += interpRead(baseDel1_, tapDel1_, modOfsA) * krt;

        acc_ = svfB_.process(acc_, lpCutoffNorm);
        acc_ -= hpfB_.process(acc_, hpCutoffNorm);

        prevRead_ = bufRead(writePtr_ + baseDap2a_ + tapDap2a_ - 1);
        acc_ += prevRead_ * kap;
        bufWrite(writePtr_ + baseDap2a_, acc_);
        acc_ *= -kap;
        acc_ += prevRead_;

        prevRead_ = bufRead(writePtr_ + baseDap2b_ + tapDap2b_ - 1);
        acc_ += prevRead_ * (-kap);
        bufWrite(writePtr_ + baseDap2b_, acc_);
        acc_ *= kap;
        acc_ += prevRead_;

        acc_ = saturate(acc_, 0.7f);
        bufWrite(writePtr_ + baseDel2_, acc_);
        wet = acc_;
        inOutR[i] += (wet - inOutR[i]) * amt;

        // Soft clip (tanh with threshold)
        if (tanhEnabled_)
        {
            float t = tanhThreshold_;
            inOutL[i] = std::tanh(inOutL[i] / t) * t;
            inOutR[i] = std::tanh(inOutR[i] / t) * t;
        }
    }
}
