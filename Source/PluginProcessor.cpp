#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

CloudsReverbPlugin::CloudsReverbPlugin()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", CloudsReverbParams::createLayout()),
      presetManager(apvts)
{
    decayParam     = apvts.getRawParameterValue("decay");
    dampingParam   = apvts.getRawParameterValue("damping");
    diffusionParam = apvts.getRawParameterValue("diffusion");
    amountParam    = apvts.getRawParameterValue("amount");
    inputGainParam = apvts.getRawParameterValue("input_gain");
    modSpeedParam  = apvts.getRawParameterValue("mod_speed");
}

void CloudsReverbPlugin::prepareToPlay(double sampleRate, int)
{
    reverb_.prepare(sampleRate);
}

void CloudsReverbPlugin::releaseResources()
{
    reverb_.clear();
}

void CloudsReverbPlugin::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numChannels = buffer.getNumChannels();
    const int numSamples  = buffer.getNumSamples();

    if (numChannels < 2 || numSamples == 0)
        return;

    const float gainLin = std::pow(10.0f, inputGainParam->load() / 20.0f);

    reverb_.setDecay(decayParam->load());
    reverb_.setLp(dampingParam->load());
    reverb_.setDiffusion(diffusionParam->load());
    reverb_.setAmount(amountParam->load());
    reverb_.setInputGain(gainLin);
    reverb_.setModSpeed(modSpeedParam->load());

    float* L = buffer.getWritePointer(0);
    float* R = buffer.getWritePointer(1);

    reverb_.process(L, R, numSamples);
}

juce::AudioProcessorEditor* CloudsReverbPlugin::createEditor()
{
    return new CloudsReverbEditor(*this);
}

void CloudsReverbPlugin::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CloudsReverbPlugin::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CloudsReverbPlugin();
}
