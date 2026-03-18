#include "PluginEditor.h"

static const juce::Colour kReverbColour   {0xFF5599DD};
static const juce::Colour kMixColour      {0xFF44BB66};
static const juce::Colour kModColour      {0xFFDD7744};
static const juce::Colour kBgColour       {0xFF1A1A2E};

EMVerbEditor::EMVerbEditor(EMVerbPlugin& p)
    : AudioProcessorEditor(p), processor(p)
{
    // Reverb block (blue)
    setupKnob(decayKnob,     decayLabel,     "Decay",     kReverbColour);
    setupKnob(dampingKnob,   dampingLabel,   "Hi Cut",    kReverbColour);
    setupKnob(loCutKnob,     loCutLabel,     "Lo Cut",    kReverbColour);
    setupKnob(diffusionKnob, diffusionLabel, "Diffusion", kReverbColour);

    // Mix block (green)
    setupKnob(mixKnob,       mixLabel,       "Mix",       kMixColour);
    setupKnob(inputGainKnob, inputGainLabel, "Gain",      kMixColour);

    // Modulation block (orange)
    setupKnob(modSpeedKnob,  modSpeedLabel,  "Mod",       kModColour);

    // Attachments
    decayAtt     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "decay",      decayKnob);
    dampingAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "damping",    dampingKnob);
    loCutAtt     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "lo_cut",     loCutKnob);
    diffusionAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "diffusion",  diffusionKnob);
    mixAtt       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "amount",     mixKnob);
    inputGainAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "input_gain", inputGainKnob);
    modSpeedAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "mod_speed",  modSpeedKnob);

    // Tanh controls
    addAndMakeVisible(tanhToggle);
    tanhToggle.setColour(juce::ToggleButton::tickColourId, kModColour);
    tanhEnabledAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.apvts, "tanh_enabled", tanhToggle);

    setupKnob(tanhThresholdKnob, tanhThresholdLabel, "Thresh", kModColour);
    tanhThresholdAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "tanh_threshold", tanhThresholdKnob);

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

    // Image load button
    addAndMakeVisible(loadImageButton);
    loadImageButton.onClick = [this] { loadBackgroundImage(); };

    // Opacity knob (small)
    addAndMakeVisible(opacityKnob);
    opacityKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    opacityKnob.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    opacityKnob.setRange(0.0, 1.0, 0.01);
    opacityKnob.setValue(0.3);
    opacityKnob.setColour(juce::Slider::rotarySliderFillColourId,
                          juce::Colours::white.withAlpha(0.5f));
    opacityKnob.setColour(juce::Slider::thumbColourId,
                          juce::Colours::white);
    opacityKnob.onValueChange = [this]
    {
        imageOpacity_ = static_cast<float>(opacityKnob.getValue());
        repaint();
    };

    addAndMakeVisible(opacityLabel);
    opacityLabel.setText("Op", juce::dontSendNotification);
    opacityLabel.setJustificationType(juce::Justification::centred);
    opacityLabel.setColour(juce::Label::textColourId,
                           juce::Colours::white.withAlpha(0.5f));
    opacityLabel.setFont(juce::FontOptions(11.0f));

    refreshPresetList();
    restoreImagePath();
    setSize(810, 340);
}

void EMVerbEditor::setupKnob(juce::Slider& knob,
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

void EMVerbEditor::refreshPresetList()
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

void EMVerbEditor::loadBackgroundImage()
{
    auto chooser = std::make_shared<juce::FileChooser>(
        "Select Background Image",
        juce::File::getSpecialLocation(juce::File::userPicturesDirectory),
        "*.png;*.jpg;*.jpeg;*.gif;*.bmp");

    chooser->launchAsync(juce::FileBrowserComponent::openMode
                       | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser&)
        {
            auto file = chooser->getResult();
            if (file.existsAsFile())
            {
                bgImage = juce::ImageFileFormat::loadFrom(file);
                saveImagePath(file);
                repaint();
            }
        });
}

juce::File EMVerbEditor::getImageSettingsFile() const
{
    return juce::File::getSpecialLocation(
        juce::File::userApplicationDataDirectory)
        .getChildFile("EMVerb")
        .getChildFile("bg_image.txt");
}

void EMVerbEditor::saveImagePath(const juce::File& imageFile)
{
    auto settingsFile = getImageSettingsFile();
    settingsFile.getParentDirectory().createDirectory();
    settingsFile.replaceWithText(imageFile.getFullPathName());
}

void EMVerbEditor::restoreImagePath()
{
    auto settingsFile = getImageSettingsFile();
    if (settingsFile.existsAsFile())
    {
        auto path = settingsFile.loadFileAsString().trim();
        juce::File imageFile(path);
        if (imageFile.existsAsFile())
            bgImage = juce::ImageFileFormat::loadFrom(imageFile);
    }
}

void EMVerbEditor::paint(juce::Graphics& g)
{
    g.fillAll(kBgColour);

    // Draw background image if loaded
    if (bgImage.isValid() && imageOpacity_ > 0.001f)
    {
        auto area = getLocalBounds().toFloat();
        float imgW = static_cast<float>(bgImage.getWidth());
        float imgH = static_cast<float>(bgImage.getHeight());
        float scale = std::max(area.getWidth() / imgW,
                               area.getHeight() / imgH);
        float drawW = imgW * scale;
        float drawH = imgH * scale;
        float x = (area.getWidth()  - drawW) * 0.5f;
        float y = (area.getHeight() - drawH) * 0.5f;

        g.setOpacity(imageOpacity_);
        g.drawImage(bgImage,
                    x, y, drawW, drawH,
                    0, 0, bgImage.getWidth(), bgImage.getHeight());
        g.setOpacity(1.0f);
    }

    // Section backgrounds
    auto bounds = getLocalBounds().reduced(8);
    bounds.removeFromTop(36);
    bounds.removeFromTop(8);

    int knobW = bounds.getWidth() / 9;

    auto reverbArea = bounds.removeFromLeft(knobW * 4);
    g.setColour(kReverbColour.withAlpha(0.08f));
    g.fillRoundedRectangle(reverbArea.toFloat().reduced(2), 8.0f);
    g.setColour(kReverbColour.withAlpha(0.3f));
    g.drawRoundedRectangle(reverbArea.toFloat().reduced(2), 8.0f, 1.0f);

    auto mixArea = bounds.removeFromLeft(knobW * 2);
    g.setColour(kMixColour.withAlpha(0.08f));
    g.fillRoundedRectangle(mixArea.toFloat().reduced(2), 8.0f);
    g.setColour(kMixColour.withAlpha(0.3f));
    g.drawRoundedRectangle(mixArea.toFloat().reduced(2), 8.0f, 1.0f);

    auto modArea = bounds;
    g.setColour(kModColour.withAlpha(0.08f));
    g.fillRoundedRectangle(modArea.toFloat().reduced(2), 8.0f);
    g.setColour(kModColour.withAlpha(0.3f));
    g.drawRoundedRectangle(modArea.toFloat().reduced(2), 8.0f, 1.0f);

    g.setColour(juce::Colours::white.withAlpha(0.7f));
    g.setFont(juce::FontOptions(13.0f));
    g.drawText("EMVerb", getLocalBounds().reduced(10, 6).removeFromTop(20),
               juce::Justification::right);
}

void EMVerbEditor::resized()
{
    auto bounds = getLocalBounds().reduced(8);

    // Preset bar
    auto presetArea = bounds.removeFromTop(36);
    presetBox.setBounds(presetArea.removeFromLeft(260));
    presetArea.removeFromLeft(6);
    saveButton.setBounds(presetArea.removeFromLeft(50));
    presetArea.removeFromLeft(4);
    deleteButton.setBounds(presetArea.removeFromLeft(42));

    // Image controls on the right side of preset bar
    presetArea.removeFromLeft(8);
    loadImageButton.setBounds(presetArea.removeFromLeft(40));
    presetArea.removeFromLeft(4);
    opacityLabel.setBounds(presetArea.removeFromLeft(20).withHeight(16));
    opacityKnob.setBounds(presetArea.removeFromLeft(34));

    bounds.removeFromTop(8);

    int knobW = bounds.getWidth() / 9;
    int labelH = 11;

    auto placeKnob = [&](juce::Slider& knob, juce::Label& label,
                          juce::Rectangle<int> area)
    {
        label.setBounds(area.removeFromTop(labelH));
        knob.setBounds(area.reduced(2));
    };

    auto col = bounds.removeFromLeft(knobW);
    placeKnob(decayKnob, decayLabel, col);
    col = bounds.removeFromLeft(knobW);
    placeKnob(dampingKnob, dampingLabel, col);
    col = bounds.removeFromLeft(knobW);
    placeKnob(loCutKnob, loCutLabel, col);
    col = bounds.removeFromLeft(knobW);
    placeKnob(diffusionKnob, diffusionLabel, col);

    col = bounds.removeFromLeft(knobW);
    placeKnob(mixKnob, mixLabel, col);
    col = bounds.removeFromLeft(knobW);
    placeKnob(inputGainKnob, inputGainLabel, col);

    col = bounds.removeFromLeft(knobW);
    placeKnob(modSpeedKnob, modSpeedLabel, col);

    // Tanh section
    col = bounds.removeFromLeft(knobW);
    auto tanhCol = col;
    tanhToggle.setBounds(tanhCol.removeFromTop(labelH + 16));
    tanhCol.removeFromTop(2);

    col = bounds;
    placeKnob(tanhThresholdKnob, tanhThresholdLabel, col);
}
