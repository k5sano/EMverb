#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "DattorroReverb.h"
#include "SampleRateAdapter.h"
#include <vector>
#include <cmath>
#include <algorithm>

static float rms(const float* buf, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; ++i)
        sum += static_cast<double>(buf[i]) * buf[i];
    return static_cast<float>(std::sqrt(sum / n));
}

TEST_CASE("Silence input produces silence output", "[reverb]")
{
    DattorroReverb rev;
    rev.init();
    rev.setDecay(0.0f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);

    constexpr int N = 4096;
    std::vector<float> L(N, 0.0f);
    std::vector<float> R(N, 0.0f);

    rev.process(L.data(), R.data(), N);

    float rmsL = rms(L.data(), N);
    float rmsR = rms(R.data(), N);

    REQUIRE(rmsL < 1e-6f);
    REQUIRE(rmsR < 1e-6f);
}

TEST_CASE("Impulse produces reverb tail", "[reverb]")
{
    DattorroReverb rev;
    rev.init();
    rev.setDecay(0.7f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);
    rev.setDiffusion(0.625f);
    rev.setLp(0.7f);

    constexpr int N = 8192;
    std::vector<float> L(N, 0.0f);
    std::vector<float> R(N, 0.0f);

    L[0] = 1.0f;
    R[0] = 1.0f;

    rev.process(L.data(), R.data(), N);

    float tailRms = rms(L.data() + 1000, N - 1000);
    REQUIRE(tailRms > 1e-4f);
}

TEST_CASE("Low decay makes tail short", "[reverb]")
{
    DattorroReverb rev;
    rev.init();
    rev.setDecay(0.1f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);

    constexpr int N = 8192;
    std::vector<float> L(N, 0.0f);
    std::vector<float> R(N, 0.0f);
    L[0] = 1.0f;
    R[0] = 1.0f;

    rev.process(L.data(), R.data(), N);

    float lateTailRms = rms(L.data() + 6000, 2000);
    REQUIRE(lateTailRms < 0.01f);
}

TEST_CASE("Amount=0 passes dry signal", "[reverb]")
{
    DattorroReverb rev;
    rev.init();
    rev.setDecay(0.5f);
    rev.setAmount(0.0f);
    rev.setInputGain(1.0f);

    constexpr int N = 256;
    std::vector<float> L(N);
    std::vector<float> R(N);
    std::vector<float> origL(N);

    for (int i = 0; i < N; ++i)
    {
        float v = std::sin(2.0f * static_cast<float>(M_PI) * 440.0f
                           * i / 32000.0f);
        L[i] = v;
        R[i] = v;
        origL[i] = v;
    }

    rev.process(L.data(), R.data(), N);

    for (int i = 0; i < N; ++i)
    {
        REQUIRE_THAT(L[i],
            Catch::Matchers::WithinAbs(origL[i], 1e-5f));
    }
}

TEST_CASE("Clear resets reverb state", "[reverb]")
{
    DattorroReverb rev;
    rev.init();
    rev.setDecay(0.9f);
    rev.setAmount(1.0f);
    rev.setInputGain(1.0f);

    constexpr int N = 1024;
    std::vector<float> L(N, 0.0f);
    std::vector<float> R(N, 0.0f);
    L[0] = 1.0f;
    R[0] = 1.0f;

    rev.process(L.data(), R.data(), N);

    rev.clear();
    std::fill(L.begin(), L.end(), 0.0f);
    std::fill(R.begin(), R.end(), 0.0f);

    rev.process(L.data(), R.data(), N);

    float rmsL = rms(L.data(), N);
    REQUIRE(rmsL < 1e-6f);
}
