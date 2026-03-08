#include "DattorroReverb.h"
#include <cmath>

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
// DattorroReverb
// ============================================================

DattorroReverb::DattorroReverb()
{
    init();
}

void DattorroReverb::init()
{
    clear();
    lfo_[0].setFrequency(0.5f / 32000.0f);
    lfo_[1].setFrequency(0.3f / 32000.0f);
    lp_ = 0.7f;
    diffusion_ = 0.625f;
    lpDecay1_ = 0.0f;
    lpDecay2_ = 0.0f;
}

void DattorroReverb::clear()
{
    std::memset(buffer_, 0, sizeof(buffer_));
    writePtr_ = 0;
    lpDecay1_ = 0.0f;
    lpDecay2_ = 0.0f;
}

void DattorroReverb::setModSpeed(float speed)
{
    float scale = 0.2f + speed * 1.6f;
    lfo_[0].setFrequency(0.5f / 32000.0f * scale);
    lfo_[1].setFrequency(0.3f / 32000.0f * scale);
}

// ============================================================
// Delay helpers
// ============================================================

float DattorroReverb::readDelay(int base, int offset) const
{
    return buffer_[(writePtr_ + base + offset) & kBufferMask];
}

float DattorroReverb::readDelayInterp(int base, float offset) const
{
    int intPart = static_cast<int>(offset);
    float fracPart = offset - static_cast<float>(intPart);
    float a = buffer_[(writePtr_ + base + intPart) & kBufferMask];
    float b = buffer_[(writePtr_ + base + intPart + 1) & kBufferMask];
    return a + (b - a) * fracPart;
}

void DattorroReverb::writeDelay(int base, int offset, float value)
{
    buffer_[(writePtr_ + base + offset) & kBufferMask] = value;
}

float DattorroReverb::readTail(int base, int length) const
{
    return buffer_[(writePtr_ + base + length - 1) & kBufferMask];
}

// ============================================================
// Context-style helpers
// ============================================================

void DattorroReverb::ctxLoad(float value)
{
    accumulator_ = value;
}

void DattorroReverb::ctxRead(float value, float scale)
{
    accumulator_ += value * scale;
}

void DattorroReverb::ctxReadDelay(int base, int offset, float scale)
{
    float r = readDelay(base, offset);
    previousRead_ = r;
    accumulator_ += r * scale;
}

void DattorroReverb::ctxReadTail(int base, int length, float scale)
{
    float r = readTail(base, length);
    previousRead_ = r;
    accumulator_ += r * scale;
}

void DattorroReverb::ctxInterpolate(int base, float offset, float scale)
{
    float x = readDelayInterp(base, offset);
    previousRead_ = x;
    accumulator_ += x * scale;
}

void DattorroReverb::ctxInterpolateLfo(int base, float offset, int lfoIdx,
                                        float amplitude, float scale)
{
    float modOffset = offset + amplitude * lfoValue_[lfoIdx];
    float x = readDelayInterp(base, modOffset);
    previousRead_ = x;
    accumulator_ += x * scale;
}

void DattorroReverb::ctxWrite(float& value, float scale)
{
    value = accumulator_;
    accumulator_ *= scale;
}

void DattorroReverb::ctxWriteDelay(int base, int offset, float scale)
{
    buffer_[(writePtr_ + base + offset) & kBufferMask] = accumulator_;
    accumulator_ *= scale;
}

void DattorroReverb::ctxWriteAllPass(int base, int offset, float scale)
{
    buffer_[(writePtr_ + base + offset) & kBufferMask] = accumulator_;
    accumulator_ *= scale;
    accumulator_ += previousRead_;
}

void DattorroReverb::ctxLp(float& state, float coefficient)
{
    state += coefficient * (accumulator_ - state);
    accumulator_ = state;
}

void DattorroReverb::ctxAdvanceWritePtr()
{
    --writePtr_;
    if (writePtr_ < 0)
        writePtr_ += kBufferSize;
}

// ============================================================
// Process — Dattorro plate reverb (Clouds topology)
// ============================================================

void DattorroReverb::process(float* inOutL, float* inOutR, int numSamples)
{
    const float kap = diffusion_;
    const float klp = lp_;
    const float krt = reverbTime_;
    const float amt = amount_;
    const float gain = inputGain_;

    for (int i = 0; i < numSamples; ++i)
    {
        ctxAdvanceWritePtr();

        if ((writePtr_ & 31) == 0)
        {
            lfoValue_[0] = lfo_[0].next();
            lfoValue_[1] = lfo_[1].next();
        }

        accumulator_ = 0.0f;
        previousRead_ = 0.0f;

        float wet;
        float apout = 0.0f;

        // Smear AP1
        ctxInterpolateLfo(kAp1Base, 10.0f, 0, 60.0f, 1.0f);
        ctxWriteDelay(kAp1Base, 100, 0.0f);

        // Input (mono sum)
        ctxLoad(0.0f);
        ctxRead(inOutL[i] + inOutR[i], gain);

        // Diffuse through 4 allpasses
        ctxReadTail(kAp1Base, kAp1Len, kap);
        ctxWriteAllPass(kAp1Base, 0, -kap);

        ctxReadTail(kAp2Base, kAp2Len, kap);
        ctxWriteAllPass(kAp2Base, 0, -kap);

        ctxReadTail(kAp3Base, kAp3Len, kap);
        ctxWriteAllPass(kAp3Base, 0, -kap);

        ctxReadTail(kAp4Base, kAp4Len, kap);
        ctxWriteAllPass(kAp4Base, 0, -kap);

        ctxWrite(apout, 0.0f);

        // Loop side A
        ctxLoad(apout);
        ctxInterpolateLfo(kDel2Base, 4680.0f, 1, 100.0f, krt);
        ctxLp(lpDecay1_, klp);

        ctxReadTail(kDap1aBase, kDap1aLen, -kap);
        ctxWriteAllPass(kDap1aBase, 0, kap);

        ctxReadTail(kDap1bBase, kDap1bLen, kap);
        ctxWriteAllPass(kDap1bBase, 0, -kap);

        ctxWriteDelay(kDel1Base, 0, 2.0f);
        ctxWrite(wet, 0.0f);

        inOutL[i] += (wet - inOutL[i]) * amt;

        // Loop side B
        ctxLoad(apout);
        ctxReadTail(kDel1Base, kDel1Len, krt);
        ctxLp(lpDecay2_, klp);

        ctxReadTail(kDap2aBase, kDap2aLen, kap);
        ctxWriteAllPass(kDap2aBase, 0, -kap);

        ctxReadTail(kDap2bBase, kDap2bLen, -kap);
        ctxWriteAllPass(kDap2bBase, 0, kap);

        ctxWriteDelay(kDel2Base, 0, 2.0f);
        ctxWrite(wet, 0.0f);

        inOutR[i] += (wet - inOutR[i]) * amt;
    }
}
