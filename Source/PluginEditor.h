#pragma once #include <juce\_audio\_processors/juce\_audio\_processors.h> #include "PluginProcessor.h"

class CloudsReverbEditor : public juce::AudioProcessorEditor { public: explicit CloudsReverbEditor(CloudsReverbPlugin&); ~CloudsReverbEditor() override = default;

```
void paint(juce::Graphics&) override;
void resized() override;
```

private: CloudsReverbPlugin& processor; juce::GenericAudioProcessorEditor genericEditor;

```
JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CloudsReverbEditor)
```

};