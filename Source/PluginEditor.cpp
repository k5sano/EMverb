#include "PluginEditor.h"

static const juce::Colour kReverbColour   {0xFF5599DD};  // blue
static const juce::Colour kMixColour      {0xFF44BB66};  // green
static const juce::Colour kModColour      {0xFFDD7744};  // orange
static const juce::Colour kBgColour       {0xFF1A1A2E};  // dark navy

CloudsReverbEditor::CloudsReverbEditor(CloudsReverbPlugin& p)
    : AudioProcessorEditor(p), processor(p)
{
    // Reverb block (blue)
    setupKnob(decayKnob,     decayLabel,     "Decay",     kReverbColour);
    setupKnob(dampingKnob,   dampingLabel,   "Damping",   kReverbColour);
    setupKnob(diffusionKnob, diffusionLabel, "Diffusion", kReverbColour);

    // Mix block (green)
    setupKnob(mixKnob,       mixLabel,       "Mix",       kMixColour);
    setupKnob(inputGainKnob, inputGainLabel, "Gain",      kMixColour);

    // Modulation block (orange)
    setupKnob(modSpeedKnob,  modSpeedLabel,  "Mod",       kModColour);

    // Attachments
    decayAtt     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "decay",      decayKnob);
    dampingAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "damping",    dampingKnob);
    diffusionAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "diffusion",  diffusionKnob);
    mixAtt       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "amount",     mixKnob);
    inputGainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "input_gain", inputGainKnob);
    modSpeedAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "mod_speed",  modSpeedKnob);

    // Preset controls
    addAndMakeVisible(presetBox);
    presetBox.setTextWhenNothingSelected("-- Presets --");
    presetBox.onChange = [this]
    {
        auto name = presetBox.getText();
        if (name.isNotEmpty() && name != "-- Presets --")
            processor.presetManager.loadPreset(name);
    };

    addAndMakeVisible(saveButton);
    saveButton.onClick = [this]
    {
        auto name = processor.presetManager.getCurrentPresetName();
        auto dlg = std::make_shared<juce::AlertWindow>(
            "Save Preset", "Enter preset name:",
            juce::MessageBoxIconType::NoIcon, this);
        dlg->addTextEditor("name", name, "Name:");
        dlg->addButton("Save", 1);
        dlg->addButton("Cancel", 0);
        dlg->enterModalState(true,
            juce::ModalCallbackFunction::create(
                [this, dlg](int result)
                {
                    if (result == 1)
                    {
                        auto n = dlg->getTextEditorContents("name").trim();
                        if (n.isNotEmpty())
                        {
                            processor.presetManager.savePreset(n);
                            refreshPresetList();
                        }
                    }
                }));
    };

    addAndMakeVisible(deleteButton);
    deleteButton.onClick = [this]
    {
        auto name = presetBox.getText();
        if (name.isNotEmpty())
        {
            processor.presetManager.deletePreset(name);
            refreshPresetList();
        }
    };

    refreshPresetList();
    setSize(620, 340);
}

void CloudsReverbEditor::setupKnob(juce::Slider& knob,
                                     juce::Label& label,
                                     const juce::String& text,
                                     juce::Colour colour)
{
    addAndMakeVisible(knob);
    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    knob.setColour(juce::Slider::rotarySliderFillColourId, colour);
    knob.setColour(juce::Slider::thumbColourId, colour.brighter(0.3f));
    knob.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    knob.setColour(juce::Slider::textBoxOutlineColourId,
                   juce::Colours::transparentBlack);

    addAndMakeVisible(label);
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, colour.brighter(0.4f));
    label.setFont(juce::FontOptions(16.0f, juce::Font::bold));
}

void CloudsReverbEditor::refreshPresetList()
{
    presetBox.clear(juce::dontSendNotification);
    auto list = processor.presetManager.getPresetList();
    for (int i = 0; i < list.size(); ++i)
        presetBox.addItem(list[i], i + 1);

    auto current = processor.presetManager.getCurrentPresetName();
    if (current.isNotEmpty())
    {
        int idx = list.indexOf(current);
        if (idx >= 0)
            presetBox.setSelectedId(idx + 1, juce::dontSendNotification);
    }
}

void CloudsReverbEditor::paint(juce::Graphics& g)
{
    g.fillAll(kBgColour);

    // Section backgrounds
    auto bounds = getLocalBounds().reduced(8);
    auto presetArea = bounds.removeFromTop(36);
    (void)presetArea;
    bounds.removeFromTop(8);

    int knobW = bounds.getWidth() / 6;

    // Reverb section (3 knobs)
    auto reverbArea = bounds.removeFromLeft(knobW * 3);
    g.setColour(kReverbColour.withAlpha(0.08f));
    g.fillRoundedRectangle(reverbArea.toFloat().reduced(2), 8.0f);
    g.setColour(kReverbColour.withAlpha(0.3f));
    g.drawRoundedRectangle(reverbArea.toFloat().reduced(2), 8.0f, 1.0f);

    // Mix section (2 knobs)
    auto mixArea = bounds.removeFromLeft(knobW * 2);
    g.setColour(kMixColour.withAlpha(0.08f));
    g.fillRoundedRectangle(mixArea.toFloat().reduced(2), 8.0f);
    g.setColour(kMixColour.withAlpha(0.3f));
    g.drawRoundedRectangle(mixArea.toFloat().reduced(2), 8.0f, 1.0f);

    // Mod section (1 knob)
    auto modArea = bounds;
    g.setColour(kModColour.withAlpha(0.08f));
    g.fillRoundedRectangle(modArea.toFloat().reduced(2), 8.0f);
    g.setColour(kModColour.withAlpha(0.3f));
    g.drawRoundedRectangle(modArea.toFloat().reduced(2), 8.0f, 1.0f);

    // Title
    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(juce::FontOptions(13.0f));
    g.drawText("CloudsReverb", getLocalBounds().removeFromBottom(20),
               juce::Justification::centred);
}

void CloudsReverbEditor::resized()
{
    auto bounds = getLocalBounds().reduced(8);

    // Preset bar
    auto presetArea = bounds.removeFromTop(36);
    presetBox.setBounds(presetArea.removeFromLeft(300));
    presetArea.removeFromLeft(8);
    saveButton.setBounds(presetArea.removeFromLeft(60));
    presetArea.removeFromLeft(4);
    deleteButton.setBounds(presetArea.removeFromLeft(50));

    bounds.removeFromTop(8);

    int knobW = bounds.getWidth() / 6;
    int labelH = 22;
    int knobH = bounds.getHeight() - labelH - 8;

    auto placeKnob = [&](juce::Slider& knob, juce::Label& label,
                          juce::Rectangle<int> area)
    {
        label.setBounds(area.removeFromTop(labelH));
        knob.setBounds(area.reduced(4));
    };

    // Reverb: Decay, Damping, Diffusion
    auto col = bounds.removeFromLeft(knobW);
    placeKnob(decayKnob, decayLabel, col);
    col = bounds.removeFromLeft(knobW);
    placeKnob(dampingKnob, dampingLabel, col);
    col = bounds.removeFromLeft(knobW);
    placeKnob(diffusionKnob, diffusionLabel, col);

    // Mix: Mix, Gain
    col = bounds.removeFromLeft(knobW);
    placeKnob(mixKnob, mixLabel, col);
    col = bounds.removeFromLeft(knobW);
    placeKnob(inputGainKnob, inputGainLabel, col);

    // Mod: Mod Speed
    col = bounds;
    placeKnob(modSpeedKnob, modSpeedLabel, col);
}
