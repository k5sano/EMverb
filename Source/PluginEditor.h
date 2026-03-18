#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class EMVerbEditor : public juce::AudioProcessorEditor {
public:
    explicit EMVerbEditor(EMVerbPlugin&);
    ~EMVerbEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    EMVerbPlugin& processor;

    // Knobs
    juce::Slider decayKnob, dampingKnob, diffusionKnob;
    juce::Slider mixKnob, inputGainKnob, modSpeedKnob;

    // Labels
    juce::Label decayLabel, dampingLabel, diffusionLabel;
    juce::Label mixLabel, inputGainLabel, modSpeedLabel;

    // Tanh controls
    juce::ToggleButton tanhToggle{"Tanh"};
    juce::Slider tanhThresholdKnob;
    juce::Label tanhThresholdLabel;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        decayAtt, dampingAtt, diffusionAtt,
        mixAtt, inputGainAtt, modSpeedAtt, tanhThresholdAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>
        tanhEnabledAtt;

    // Preset controls
    juce::ComboBox presetBox;
    juce::TextButton saveButton{"Save"};
    juce::TextButton deleteButton{"Del"};

    // Background image
    juce::Image bgImage;
    juce::TextButton loadImageButton{"Img"};
    juce::Slider opacityKnob;
    juce::Label opacityLabel;
    float imageOpacity_ = 0.3f;

    void setupKnob(juce::Slider& knob, juce::Label& label,
                   const juce::String& text, juce::Colour colour);
    void refreshPresetList();
    void loadBackgroundImage();

    // Persist image path
    juce::File getImageSettingsFile() const;
    void saveImagePath(const juce::File& imageFile);
    void restoreImagePath();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EMVerbEditor)
};
