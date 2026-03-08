#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "Parameters.h"
#include "DSP/DattorroReverb.h"
#include "PresetManager.h"

class CloudsReverbPlugin : public juce::AudioProcessor {
public:
    CloudsReverbPlugin();
    ~CloudsReverbPlugin() override = default;

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
    PresetManager presetManager;

private:
    std::atomic<float>* decayParam = nullptr;
    std::atomic<float>* dampingParam = nullptr;
    std::atomic<float>* diffusionParam = nullptr;
    std::atomic<float>* amountParam = nullptr;
    std::atomic<float>* inputGainParam = nullptr;
    std::atomic<float>* modSpeedParam = nullptr;

    DattorroReverb reverb_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloudsReverbPlugin)
};
