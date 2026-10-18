#pragma once
#include <JuceHeader.h>

class WingmanStudioAudioProcessor final : public juce::AudioProcessor
{
public:
    WingmanStudioAudioProcessor();
    ~WingmanStudioAudioProcessor() override = default;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "Wingman Studio"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 1.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    void nextProgression();
    juce::String progressionName() const;
private:
    static juce::AudioProcessorValueTreeState::ParameterLayout makeParams();
    juce::Synthesiser synth;
    juce::StringArray progressions { "Am - F - C - G", "Dm - Bb - F - C", "Em - C - G - D", "Cm - Ab - Eb - Bb" };
    std::atomic<int> progression { 0 };
    double sr = 44100.0;
    int lastBar = -1;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WingmanStudioAudioProcessor)
};
