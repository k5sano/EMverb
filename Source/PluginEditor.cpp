#include "PluginEditor.h"

CloudsReverbEditor::CloudsReverbEditor(CloudsReverbPlugin& p)
    : AudioProcessorEditor(p),
      processor(p),
      genericEditor(p)
{
    addAndMakeVisible(genericEditor);
    setSize(400, 300);
}

void CloudsReverbEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void CloudsReverbEditor::resized()
{
    genericEditor.setBounds(getLocalBounds());
}
