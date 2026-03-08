#include "PluginProcessor.h" #include "PluginEditor.h" #include

CloudsReverbPlugin::CloudsReverbPlugin()

AudioProcessor(BusesProperties() .withInput("Input", juce::AudioChannelSet::stereo(), true) .withOutput("Output", juce::AudioChannelSet::stereo(), true)), apvts(\*this, nullptr, "PARAMETERS", CloudsReverbParams::createLayout()) { decayParam = apvts.getRawParameterValue("decay"); dampingParam = apvts.getRawParameterValue("damping"); diffusionParam = apvts.getRawParameterValue("diffusion"); amountParam = apvts.getRawParameterValue("amount"); inputGainParam = apvts.getRawParameterValue("input\_gain"); modSpeedParam = apvts.getRawParameterValue("mod\_speed"); }

void CloudsReverbPlugin::prepareToPlay(double sampleRate, int samplesPerBlock) { reverb\_.init(); adapter\_.prepare(sampleRate, samplesPerBlock); dryBuffer\_.setSize(2, samplesPerBlock); }

void CloudsReverbPlugin::releaseResources() { reverb\_.clear(); }

void CloudsReverbPlugin::processBlock(juce::AudioBuffer& buffer, juce::MidiBuffer&) { juce::ScopedNoDenormals noDenormals;

```
const int numChannels = buffer.getNumChannels();
const int numSamples  = buffer.getNumSamples();

if (numChannels < 2 || numSamples == 0)
    return;

// --- Read parameters ---
const float decay     = decayParam->load();
const float damping   = dampingParam->load();
const float diffusion = diffusionParam->load();
const float amount    = amountParam->load();
const float gainDb    = inputGainParam->load();
const float modSpeed  = modSpeedParam->load();

// Convert dB to linear
const float gainLin = std::pow(10.0f, gainDb / 20.0f);

// --- Update reverb parameters ---
reverb_.setDecay(decay);
reverb_.setLp(damping);
reverb_.setDiffusion(diffusion);
reverb_.setAmount(amount);
reverb_.setInputGain(gainLin);
reverb_.setModSpeed(modSpeed);

// --- Save dry signal ---
dryBuffer_.setSize(2, numSamples, false, false, true);
for (int ch = 0; ch < 2; ++ch)
    dryBuffer_.copyFrom(ch, 0, buffer, ch, 0, numSamples);

// --- Process through sample rate adapter + reverb ---
float* outL = buffer.getWritePointer(0);
float* outR = buffer.getWritePointer(1);
const float* inL = dryBuffer_.getReadPointer(0);
const float* inR = dryBuffer_.getReadPointer(1);

adapter_.process(inL, inR, outL, outR, numSamples, reverb_);
```

}

juce::AudioProcessorEditor\* CloudsReverbPlugin::createEditor() { return new CloudsReverbEditor(\*this); }

void CloudsReverbPlugin::getStateInformation(juce::MemoryBlock& destData) { auto state = apvts.copyState(); std::unique\_ptrjuce::XmlElement xml(state.createXml()); copyXmlToBinary(\*xml, destData); }

void CloudsReverbPlugin::setStateInformation(const void\* data, int sizeInBytes) { std::unique\_ptrjuce::XmlElement xml( getXmlFromBinary(data, sizeInBytes)); if (xml != nullptr && xml->hasTagName(apvts.state.getType())) apvts.replaceState(juce::ValueTree::fromXml(\*xml)); }

juce::AudioProcessor\* JUCE\_CALLTYPE createPluginFilter() { return new CloudsReverbPlugin(); }