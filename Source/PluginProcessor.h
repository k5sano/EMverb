#pragma once #include <juce\_audio\_processors/juce\_audio\_processors.h> #include "Parameters.h" #include "DSP/DattorroReverb.h" #include "SampleRateAdapter.h"

class CloudsReverbPlugin : public juce::AudioProcessor { public: CloudsReverbPlugin(); ~CloudsReverbPlugin() override = default;

```
void prepareToPlay(double sampleRate, int samplesPerBlock) override;
void releaseResources() override;
void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

juce::AudioProcessorEditor* createEditor() override;
bool hasEditor() const override { return true; }

const juce::String getName() const override { return "CloudsReverb"; }
bool acceptsMidi() const override { return false; }
bool producesMidi() const override { return false; }
double getTailLengthSeconds() const override { return 5.0; }

int getNumPrograms() override { return 1; }
int getCurrentProgram() override { return 0; }
void setCurrentProgram(int) override {}
const juce::String getProgramName(int) override { return {}; }
void changeProgramName(int, const juce::String&) override {}

void getStateInformation(juce::MemoryBlock& destData) override;
void setStateInformation(const void* data, int sizeInBytes) override;

juce::AudioProcessorValueTreeState apvts;
```

private: // Parameter pointers (atomic) std::atomic\* decayParam = nullptr; std::atomic\* dampingParam = nullptr; std::atomic\* diffusionParam = nullptr; std::atomic\* amountParam = nullptr; std::atomic\* inputGainParam = nullptr; std::atomic\* modSpeedParam = nullptr;

```
// DSP
DattorroReverb reverb_;
SampleRateAdapter adapter_;

// Dry buffers (pre-allocated)
juce::AudioBuffer<float> dryBuffer_;

JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloudsReverbPlugin)
```

};