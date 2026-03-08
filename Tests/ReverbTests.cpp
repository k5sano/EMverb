#include <catch2/catch\_test\_macros.hpp> #include <catch2/matchers/catch\_matchers\_floating\_point.hpp> #include "DattorroReverb.h" #include "SampleRateAdapter.h" #include #include #include

// ============================================================ // Helper: compute RMS of a buffer // ============================================================ static float rms(const float\* buf, int n) { double sum = 0.0; for (int i = 0; i < n; ++i) sum += static\_cast(buf\[i\]) \* buf\[i\]; return static\_cast(std::sqrt(sum / n)); }

// ============================================================ // Test: Silence in → silence out // ============================================================ TEST\_CASE("Silence input produces silence output", "\[reverb\]") { DattorroReverb rev; rev.init(); rev.setDecay(0.0f); rev.setAmount(1.0f); rev.setInputGain(1.0f);

```
constexpr int N = 4096;
std::vector<float> L(N, 0.0f);
std::vector<float> R(N, 0.0f);

rev.process(L.data(), R.data(), N);

float rmsL = rms(L.data(), N);
float rmsR = rms(R.data(), N);

REQUIRE(rmsL < 1e-6f);
REQUIRE(rmsR < 1e-6f);
```

}

// ============================================================ // Test: Impulse produces non-zero output (reverb tail) // ============================================================ TEST\_CASE("Impulse produces reverb tail", "\[reverb\]") { DattorroReverb rev; rev.init(); rev.setDecay(0.7f); rev.setAmount(1.0f); rev.setInputGain(1.0f); rev.setDiffusion(0.625f); rev.setLp(0.7f);

```
constexpr int N = 8192;
std::vector<float> L(N, 0.0f);
std::vector<float> R(N, 0.0f);

// Single impulse at sample 0
L[0] = 1.0f;
R[0] = 1.0f;

rev.process(L.data(), R.data(), N);

// Check that tail exists in later portion
float tailRms = rms(L.data() + 1000, N - 1000);
REQUIRE(tailRms > 1e-4f);
```

}

// ============================================================ // Test: Decay=0 makes tail decay quickly // ============================================================ TEST\_CASE("Low decay makes tail short", "\[reverb\]") { DattorroReverb rev; rev.init(); rev.setDecay(0.1f); rev.setAmount(1.0f); rev.setInputGain(1.0f);

```
constexpr int N = 8192;
std::vector<float> L(N, 0.0f);
std::vector<float> R(N, 0.0f);
L[0] = 1.0f;
R[0] = 1.0f;

rev.process(L.data(), R.data(), N);

// Late tail should be very quiet with low decay
float lateTailRms = rms(L.data() + 6000, 2000);
REQUIRE(lateTailRms < 0.01f);
```

}

// ============================================================ // Test: Amount=0 gives dry signal through // ============================================================ TEST\_CASE("Amount=0 passes dry signal", "\[reverb\]") { DattorroReverb rev; rev.init(); rev.setDecay(0.5f); rev.setAmount(0.0f); rev.setInputGain(1.0f);

```
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

// With amount=0, output should equal input
for (int i = 0; i < N; ++i)
{
    REQUIRE_THAT(L[i],
        Catch::Matchers::WithinAbs(origL[i], 1e-5f));
}
```

}

// ============================================================ // Test: DattorroReverb clear resets state // ============================================================ TEST\_CASE("Clear resets reverb state", "\[reverb\]") { DattorroReverb rev; rev.init(); rev.setDecay(0.9f); rev.setAmount(1.0f); rev.setInputGain(1.0f);

```
constexpr int N = 1024;
std::vector<float> L(N, 0.0f);
std::vector<float> R(N, 0.0f);
L[0] = 1.0f;
R[0] = 1.0f;

rev.process(L.data(), R.data(), N);

// Now clear and process silence
rev.clear();
std::fill(L.begin(), L.end(), 0.0f);
std::fill(R.begin(), R.end(), 0.0f);

rev.process(L.data(), R.data(), N);

float rmsL = rms(L.data(), N);
REQUIRE(rmsL < 1e-6f);
```

}