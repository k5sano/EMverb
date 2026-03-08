#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class CloudsReverbEditor : public juce::AudioProcessorEditor {
public:
    explicit CloudsReverbEditor(CloudsReverbPlugin&);
    ~CloudsReverbEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    CloudsReverbPlugin& processor;

    // Knobs
    juce::Slider decayKnob, dampingKnob, diffusionKnob;
    juce::Slider mixKnob, inputGainKnob, modSpeedKnob;

    // Labels
    juce::Label decayLabel, dampingLabel, diffusionLabel;
    juce::Label mixLabel, inputGainLabel, modSpeedLabel;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        decayAtt, dampingAtt, diffusionAtt,
        mixAtt, inputGainAtt, modSpeedAtt;

    // Preset controls
    juce::ComboBox presetBox;
    juce::TextButton saveButton{"Save"};
    juce::TextButton deleteButton{"Del"};

    void setupKnob(juce::Slider& knob, juce::Label& label,
                   const juce::String& text, juce::Colour colour);
    void refreshPresetList();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloudsReverbEditor)
};
