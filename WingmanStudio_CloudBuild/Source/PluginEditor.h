#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class WingmanStudioAudioProcessorEditor final : public juce::AudioProcessorEditor {
public:
    explicit WingmanStudioAudioProcessorEditor(WingmanStudioAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    WingmanStudioAudioProcessor& proc;
    juce::Label heading, status, progLabel, keyLabel, genLabel, gainLabel;
    juce::TextButton next{"Generate variation"};
    juce::ComboBox key, generator;
    juce::ToggleButton builtIn{"Built-in sound"};
    juce::Slider gain, tempo;
    using SA=juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA=juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using BA=juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<SA> gainAtt,tempoAtt;
    std::unique_ptr<CA> keyAtt,genAtt;
    std::unique_ptr<BA> soundAtt;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WingmanStudioAudioProcessorEditor)
};
