#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

EMVerbPlugin::EMVerbPlugin()
    : AudioProcessor(BusesProperties()
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", EMVerbParams::createLayout()),
      presetManager(apvts)
{
    decayParam     = apvts.getRawParameterValue("decay");
    dampingParam   = apvts.getRawParameterValue("damping");
    loCutParam     = apvts.getRawParameterValue("lo_cut");
    diffusionParam = apvts.getRawParameterValue("diffusion");
    amountParam    = apvts.getRawParameterValue("amount");
    inputGainParam = apvts.getRawParameterValue("input_gain");
    modSpeedParam      = apvts.getRawParameterValue("mod_speed");
    tanhEnabledParam   = apvts.getRawParameterValue("tanh_enabled");
    tanhThresholdParam = apvts.getRawParameterValue("tanh_threshold");
}

void EMVerbPlugin::prepareToPlay(double sampleRate, int)
{
    reverb_.prepare(sampleRate);
}

void EMVerbPlugin::releaseResources()
{
    reverb_.clear();
}

void EMVerbPlugin::processBlock(juce::AudioBuffer<float>& buffer,
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
    reverb_.setHp(loCutParam->load());
    reverb_.setDiffusion(diffusionParam->load());
    reverb_.setAmount(amountParam->load());
    reverb_.setInputGain(gainLin);
    reverb_.setModSpeed(modSpeedParam->load());
    reverb_.setTanhEnabled(tanhEnabledParam->load() >= 0.5f);
    reverb_.setTanhThreshold(tanhThresholdParam->load());

    float* L = buffer.getWritePointer(0);
    float* R = buffer.getWritePointer(1);

    reverb_.process(L, R, numSamples);
}

juce::AudioProcessorEditor* EMVerbPlugin::createEditor()
{
    return new EMVerbEditor(*this);
}

void EMVerbPlugin::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void EMVerbPlugin::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EMVerbPlugin();
}
