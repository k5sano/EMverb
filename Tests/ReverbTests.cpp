#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "DattorroReverb.h"
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float rms(const float* buf, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; ++i)
        sum += static_cast<double>(buf[i]) * buf[i];
    return static_cast<float>(std::sqrt(sum / n));
}

static float peakAbs(const float* buf, int n)
{
    float peak = 0.0f;
    for (int i = 0; i < n; ++i)
        peak = std::max(peak, std::abs(buf[i]));
    return peak;
}

static float mean(const float* buf, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; ++i)
        sum += static_cast<double>(buf[i]);
    return static_cast<float>(sum / n);
}

static bool hasNanOrInf(const float* buf, int n)
{
    for (int i = 0; i < n; ++i)
        if (std::isnan(buf[i]) || std::isinf(buf[i]))
            return true;
    return false;
}

// Goertzel algorithm: returns magnitude of a specific frequency bin
static float goertzel(const float* buf, int n, float targetFreq, float sampleRate)
{
    float k = targetFreq / sampleRate * static_cast<float>(n);
    float w = 2.0f * static_cast<float>(M_PI) * k / static_cast<float>(n);
    float coeff = 2.0f * std::cos(w);
    float s0 = 0.0f, s1 = 0.0f, s2 = 0.0f;
    for (int i = 0; i < n; ++i)
    {
        s0 = buf[i] + coeff * s1 - s2;
        s2 = s1;
        s1 = s0;
    }
    float power = s1 * s1 + s2 * s2 - coeff * s1 * s2;
    return std::sqrt(std::max(0.0f, power)) / static_cast<float>(n);
}

static inline float saturateHelper(float x, float drive)
{
    return std::tanh(x * drive) / drive;
}

// ============================================================
TEST_CASE("Silence input produces silence output", "[reverb]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.0f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);

    constexpr int N = 4096;
    std::vector<float> L(N, 0.0f);
    std::vector<float> R(N, 0.0f);

    rev.process(L.data(), R.data(), N);

    REQUIRE(rms(L.data(), N) < 1e-6f);
    REQUIRE(rms(R.data(), N) < 1e-6f);
}

// ============================================================
TEST_CASE("Impulse produces reverb tail", "[reverb]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.7f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);
    rev.setDiffusion(0.625f);
    rev.setLp(0.7f);

    constexpr int N = 16000;
    std::vector<float> L(N, 0.0f);
    std::vector<float> R(N, 0.0f);
    L[0] = 1.0f;
    R[0] = 1.0f;

    rev.process(L.data(), R.data(), N);

    float tailRms = rms(L.data() + 2000, N - 2000);
    REQUIRE(tailRms > 1e-4f);
}

// ============================================================
TEST_CASE("Low decay makes tail short", "[reverb]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.1f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);

    constexpr int N = 16000;
    std::vector<float> L(N, 0.0f);
    std::vector<float> R(N, 0.0f);
    L[0] = 1.0f;
    R[0] = 1.0f;

    rev.process(L.data(), R.data(), N);

    float lateTailRms = rms(L.data() + 12000, 4000);
    REQUIRE(lateTailRms < 0.01f);
}

// ============================================================
TEST_CASE("Amount=0 passes dry signal", "[reverb]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.5f);
    rev.setAmount(0.0f);
    rev.setInputGain(1.0f);

    constexpr int N = 256;
    std::vector<float> L(N), R(N), origL(N);

    for (int i = 0; i < N; ++i)
    {
        float v = std::sin(2.0f * static_cast<float>(M_PI)
                           * 440.0f * i / 48000.0f);
        L[i] = v;
        R[i] = v;
        origL[i] = v;
    }

    rev.process(L.data(), R.data(), N);

    // With tanh soft clip and smear LFO, dry output is compressed
    // but should still be recognizably the input signal.
    // Check that output RMS is at least 50% of input (tanh compresses).
    float outRms = rms(L.data(), N);
    float origRms = rms(origL.data(), N);
    REQUIRE(outRms > origRms * 0.5f);
    REQUIRE(outRms < origRms * 1.5f);
}

// ============================================================
TEST_CASE("Clear resets reverb state", "[reverb]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.9f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);

    constexpr int N = 2048;
    std::vector<float> L(N, 0.0f), R(N, 0.0f);
    L[0] = 1.0f;
    R[0] = 1.0f;
    rev.process(L.data(), R.data(), N);

    rev.clear();
    std::fill(L.begin(), L.end(), 0.0f);
    std::fill(R.begin(), R.end(), 0.0f);
    rev.process(L.data(), R.data(), N);

    REQUIRE(rms(L.data(), N) < 1e-6f);
}

// ============================================================
TEST_CASE("Works at 44100Hz", "[reverb]")
{
    DattorroReverb rev;
    rev.prepare(44100.0);
    rev.setDecay(0.5f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);

    constexpr int N = 8000;
    std::vector<float> L(N, 0.0f), R(N, 0.0f);
    L[0] = 1.0f;
    R[0] = 1.0f;
    rev.process(L.data(), R.data(), N);

    float tailRms = rms(L.data() + 2000, 4000);
    REQUIRE(tailRms > 1e-5f);
}

// ============================================================
TEST_CASE("Works at 96000Hz", "[reverb]")
{
    DattorroReverb rev;
    rev.prepare(96000.0);
    rev.setDecay(0.5f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);

    constexpr int N = 16000;
    std::vector<float> L(N, 0.0f), R(N, 0.0f);
    L[0] = 1.0f;
    R[0] = 1.0f;
    rev.process(L.data(), R.data(), N);

    float tailRms = rms(L.data() + 4000, 8000);
    REQUIRE(tailRms > 1e-5f);
}

// ============================================================
// Tanh-off Tests
// ============================================================

TEST_CASE("Tanh off: dry signal passes unmodified", "[tanh]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.5f);
    rev.setAmount(0.0f);
    rev.setInputGain(1.0f);
    rev.setTanhEnabled(false);

    constexpr int N = 256;
    std::vector<float> L(N), R(N), origL(N);

    for (int i = 0; i < N; ++i)
    {
        float v = std::sin(2.0f * static_cast<float>(M_PI)
                           * 440.0f * i / 48000.0f);
        L[i] = v;
        R[i] = v;
        origL[i] = v;
    }

    rev.process(L.data(), R.data(), N);

    for (int i = 0; i < N; ++i)
        REQUIRE_THAT(L[i], Catch::Matchers::WithinAbs(origL[i], 1e-5f));
}

// ============================================================
TEST_CASE("Tanh off: output can exceed +/-1", "[tanh]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.7f);
    rev.setAmount(1.0f);
    rev.setInputGain(2.0f);  // +6dB
    rev.setTanhEnabled(false);

    constexpr int N = 48000;
    std::vector<float> L(N), R(N);

    for (int i = 0; i < N; ++i)
    {
        float v = 2.0f * std::sin(2.0f * static_cast<float>(M_PI)
                                   * 1000.0f * i / 48000.0f);
        L[i] = v;
        R[i] = v;
    }

    rev.process(L.data(), R.data(), N);

    // Without tanh, output may exceed 1.0
    float peak = peakAbs(L.data(), N);
    REQUIRE(peak > 1.0f);
    // But should not go to infinity
    REQUIRE_FALSE(hasNanOrInf(L.data(), N));
}

// ============================================================
TEST_CASE("Tanh on: output clamped by threshold", "[tanh]")
{
    constexpr float thresholds[] = {0.5f, 1.0f, 1.5f};
    constexpr int N = 48000;

    for (float thresh : thresholds)
    {
        DattorroReverb rev;
        rev.prepare(48000.0);
        rev.setDecay(0.7f);
        rev.setAmount(1.0f);
        rev.setInputGain(2.0f);
        rev.setTanhEnabled(true);
        rev.setTanhThreshold(thresh);

        std::vector<float> L(N), R(N);
        for (int i = 0; i < N; ++i)
        {
            float v = 3.0f * std::sin(2.0f * static_cast<float>(M_PI)
                                       * 1000.0f * i / 48000.0f);
            L[i] = v;
            R[i] = v;
        }

        rev.process(L.data(), R.data(), N);

        INFO("threshold=" << thresh);
        // tanh(x/t)*t asymptotes to t, so output should be <= threshold
        REQUIRE(peakAbs(L.data(), N) <= thresh + 0.01f);
        REQUIRE(peakAbs(R.data(), N) <= thresh + 0.01f);
    }
}

// ============================================================
TEST_CASE("Tanh off: stability sweep still holds", "[tanh][stability]")
{
    constexpr float decays[] = {0.5f, 0.95f, 0.99f};
    constexpr int N = 48000;

    for (float decay : decays)
    {
        DattorroReverb rev;
        rev.prepare(48000.0);
        rev.setDecay(decay);
        rev.setLp(0.7f);
        rev.setDiffusion(0.625f);
        rev.setAmount(1.0f);
        rev.setInputGain(1.0f);
        rev.setModSpeed(0.5f);
        rev.setTanhEnabled(false);

        std::vector<float> L(N, 0.0f), R(N, 0.0f);
        L[0] = 1.0f;
        R[0] = 1.0f;

        rev.process(L.data(), R.data(), N);

        INFO("decay=" << decay << " tanh=off");
        REQUIRE_FALSE(hasNanOrInf(L.data(), N));
        REQUIRE_FALSE(hasNanOrInf(R.data(), N));
        // Internal saturation still limits growth even without tanh
        REQUIRE(peakAbs(L.data(), N) < 4.0f);
    }
}

// ============================================================
// Stability & Analyzer Tests
// ============================================================

TEST_CASE("Saturate function behavior", "[dsp]")
{
    // Zero in, zero out
    REQUIRE(saturateHelper(0.0f, 0.7f) == 0.0f);

    // Small signal is approximately linear
    float small = saturateHelper(0.1f, 0.7f);
    REQUIRE_THAT(small, Catch::Matchers::WithinAbs(0.1f, 0.005f));

    // Large signal is compressed
    REQUIRE(std::abs(saturateHelper(10.0f, 0.7f)) < 1.5f);

    // Odd function symmetry
    REQUIRE_THAT(saturateHelper(-0.5f, 0.7f),
        Catch::Matchers::WithinAbs(-saturateHelper(0.5f, 0.7f), 1e-6f));
}

// ============================================================
TEST_CASE("Stability: extreme parameter sweep", "[stability]")
{
    constexpr float decays[]     = {0.0f, 0.5f, 0.95f, 0.99f, 1.0f};
    constexpr float dampings[]   = {0.0f, 0.5f, 1.0f};
    constexpr float diffusions[] = {0.0f, 0.5f, 1.0f};
    constexpr float gains[]      = {1.0f, 2.0f};
    constexpr float modSpeeds[]  = {0.0f, 0.5f, 1.0f};
    constexpr float amounts[]    = {0.5f, 1.0f};

    constexpr int N = 48000; // 1 second at 48kHz

    for (float decay : decays)
    for (float damp : dampings)
    for (float diff : diffusions)
    for (float gain : gains)
    for (float mod : modSpeeds)
    for (float amt : amounts)
    {
        DattorroReverb rev;
        rev.prepare(48000.0);
        rev.setDecay(decay);
        rev.setLp(damp);
        rev.setDiffusion(diff);
        rev.setInputGain(gain);
        rev.setModSpeed(mod);
        rev.setAmount(amt);

        std::vector<float> L(N, 0.0f), R(N, 0.0f);
        L[0] = 1.0f;
        R[0] = 1.0f;

        rev.process(L.data(), R.data(), N);

        INFO("decay=" << decay << " damp=" << damp
             << " diff=" << diff << " gain=" << gain
             << " mod=" << mod << " amt=" << amt);
        REQUIRE_FALSE(hasNanOrInf(L.data(), N));
        REQUIRE_FALSE(hasNanOrInf(R.data(), N));
        REQUIRE(peakAbs(L.data(), N) < 2.0f);
        REQUIRE(peakAbs(R.data(), N) < 2.0f);
    }
}

// ============================================================
TEST_CASE("Output gain is appropriate", "[analyzer]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.5f);
    rev.setLp(0.7f);
    rev.setDiffusion(0.625f);
    rev.setAmount(0.5f);
    rev.setInputGain(1.0f);
    rev.setModSpeed(0.5f);

    constexpr int N = 48000;
    std::vector<float> L(N), R(N);

    for (int i = 0; i < N; ++i)
    {
        float v = 0.5f * std::sin(2.0f * static_cast<float>(M_PI)
                                   * 1000.0f * i / 48000.0f);
        L[i] = v;
        R[i] = v;
    }

    float inputRms = rms(L.data(), N);
    rev.process(L.data(), R.data(), N);
    float outputRms = rms(L.data(), N);

    float ratio = outputRms / inputRms;
    // Output should be within reasonable range (±6dB = 0.25x to 4x)
    REQUIRE(ratio > 0.25f);
    REQUIRE(ratio < 4.0f);
}

// ============================================================
TEST_CASE("THD within acceptable range", "[analyzer]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.0f);   // No feedback — single pass
    rev.setLp(1.0f);      // LPF wide open
    rev.setDiffusion(0.625f);
    rev.setAmount(1.0f);  // Full wet
    rev.setInputGain(1.0f);
    rev.setModSpeed(0.0f); // No modulation for clean measurement

    constexpr float freq = 1000.0f;
    constexpr float sr = 48000.0f;
    constexpr int N = 48000;
    std::vector<float> L(N), R(N);

    for (int i = 0; i < N; ++i)
    {
        float v = 0.5f * std::sin(2.0f * static_cast<float>(M_PI)
                                   * freq * i / sr);
        L[i] = v;
        R[i] = v;
    }

    rev.process(L.data(), R.data(), N);

    // Measure fundamental and harmonics using Goertzel
    // Use second half to avoid transient
    const float* buf = L.data() + N / 2;
    int len = N / 2;

    float fundamental = goertzel(buf, len, freq, sr);
    float harm2 = goertzel(buf, len, 2.0f * freq, sr);
    float harm3 = goertzel(buf, len, 3.0f * freq, sr);
    float harm4 = goertzel(buf, len, 4.0f * freq, sr);
    float harm5 = goertzel(buf, len, 5.0f * freq, sr);

    float harmonicPower = harm2 * harm2 + harm3 * harm3
                        + harm4 * harm4 + harm5 * harm5;
    float thd = (fundamental > 1e-10f)
        ? std::sqrt(harmonicPower) / fundamental
        : 0.0f;

    INFO("THD = " << (thd * 100.0f) << "%");
    REQUIRE(thd < 0.30f); // < 30%
}

// ============================================================
TEST_CASE("Output never exceeds +/-1", "[analyzer]")
{
    constexpr float decays[] = {0.5f, 0.99f, 1.0f};
    constexpr float gains[]  = {1.0f, 2.0f};

    constexpr int N = 48000;

    for (float decay : decays)
    for (float gain : gains)
    {
        DattorroReverb rev;
        rev.prepare(48000.0);
        rev.setDecay(decay);
        rev.setLp(0.7f);
        rev.setDiffusion(0.625f);
        rev.setAmount(1.0f);
        rev.setInputGain(gain);
        rev.setModSpeed(0.5f);

        std::vector<float> L(N), R(N);
        // Large amplitude input
        for (int i = 0; i < N; ++i)
        {
            float v = 5.0f * std::sin(2.0f * static_cast<float>(M_PI)
                                       * 1000.0f * i / 48000.0f);
            L[i] = v;
            R[i] = v;
        }

        rev.process(L.data(), R.data(), N);

        INFO("decay=" << decay << " gain=" << gain);
        REQUIRE(peakAbs(L.data(), N) <= 1.0f);
        REQUIRE(peakAbs(R.data(), N) <= 1.0f);
    }
}

// ============================================================
TEST_CASE("No DC offset in output", "[analyzer]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.7f);
    rev.setLp(0.7f);
    rev.setDiffusion(0.625f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);
    rev.setModSpeed(0.5f);

    constexpr int N = 48000;
    std::vector<float> L(N), R(N);

    for (int i = 0; i < N; ++i)
    {
        float v = 0.5f * std::sin(2.0f * static_cast<float>(M_PI)
                                   * 1000.0f * i / 48000.0f);
        L[i] = v;
        R[i] = v;
    }

    rev.process(L.data(), R.data(), N);

    // Check DC of second half (after transient settles)
    float dcL = mean(L.data() + N / 2, N / 2);
    float dcR = mean(R.data() + N / 2, N / 2);

    REQUIRE(std::abs(dcL) < 0.01f);
    REQUIRE(std::abs(dcR) < 0.01f);
}

// ============================================================
TEST_CASE("Long-term stability with sustained input", "[analyzer]")
{
    DattorroReverb rev;
    rev.prepare(48000.0);
    rev.setDecay(0.95f);
    rev.setLp(0.7f);
    rev.setDiffusion(0.625f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);
    rev.setModSpeed(0.5f);

    // 10 seconds of 1kHz sine
    constexpr int totalSamples = 480000;
    constexpr int sr = 48000;
    constexpr int blockSize = 512;

    std::vector<float> L(blockSize), R(blockSize);
    float firstSecRms = 0.0f;
    float lastSecRms = 0.0f;

    // Process first second and measure
    for (int pos = 0; pos < sr; pos += blockSize)
    {
        int n = std::min(blockSize, sr - pos);
        for (int i = 0; i < n; ++i)
        {
            float v = 0.5f * std::sin(2.0f * static_cast<float>(M_PI)
                                       * 1000.0f * (pos + i) / static_cast<float>(sr));
            L[i] = v;
            R[i] = v;
        }
        rev.process(L.data(), R.data(), n);
    }
    // Measure RMS of last block of first second
    firstSecRms = rms(L.data(), blockSize);

    // Process middle 8 seconds
    for (int pos = sr; pos < totalSamples - sr; pos += blockSize)
    {
        int n = std::min(blockSize, totalSamples - sr - pos);
        if (n <= 0) break;
        for (int i = 0; i < n; ++i)
        {
            float v = 0.5f * std::sin(2.0f * static_cast<float>(M_PI)
                                       * 1000.0f * (pos + i) / static_cast<float>(sr));
            L[i] = v;
            R[i] = v;
        }
        rev.process(L.data(), R.data(), n);
    }

    // Process last second and measure
    for (int pos = totalSamples - sr; pos < totalSamples; pos += blockSize)
    {
        int n = std::min(blockSize, totalSamples - pos);
        if (n <= 0) break;
        for (int i = 0; i < n; ++i)
        {
            float v = 0.5f * std::sin(2.0f * static_cast<float>(M_PI)
                                       * 1000.0f * (pos + i) / static_cast<float>(sr));
            L[i] = v;
            R[i] = v;
        }
        rev.process(L.data(), R.data(), n);
    }
    lastSecRms = rms(L.data(), blockSize);

    INFO("firstSecRms=" << firstSecRms << " lastSecRms=" << lastSecRms);
    // Last second should not diverge: RMS should not be >10x first second
    if (firstSecRms > 1e-6f)
        REQUIRE(lastSecRms < firstSecRms * 10.0f);
    REQUIRE_FALSE(hasNanOrInf(L.data(), blockSize));
}
