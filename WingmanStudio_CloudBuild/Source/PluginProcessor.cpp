#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace {
class SimpleVoice final : public juce::SynthesiserVoice {
public:
    bool canPlaySound(juce::SynthesiserSound* s) override { return dynamic_cast<juce::SynthesiserSound*>(s) != nullptr; }
    void startNote(int midiNote, float velocity, juce::SynthesiserSound*, int) override {
        phase = 0.0; level = velocity * 0.15f; frequency = juce::MidiMessage::getMidiNoteInHertz(midiNote);
        active = true; release = false;
    }
    void stopNote(float, bool allowTailOff) override { if (allowTailOff) release = true; else { clearCurrentNote(); active = false; } }
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& out, int start, int count) override {
        if (!active) return;
        for (int i=0;i<count;++i) {
            const float env = release ? std::max(0.0f, 1.0f - (float)i / (float)juce::jmax(1, count)) : 1.0f;
            const float s = std::sin((float)phase) * level * env;
            phase += juce::MathConstants<double>::twoPi * frequency / getSampleRate();
            if (phase > juce::MathConstants<double>::twoPi) phase -= juce::MathConstants<double>::twoPi;
            for (int ch=0; ch<out.getNumChannels(); ++ch) out.addSample(ch, start+i, s);
        }
        if (release) { active=false; clearCurrentNote(); }
    }
private:
    double phase=0.0, frequency=440.0;
    float level=0.0f;
    bool active=false, release=false;
};
class SimpleSound final : public juce::SynthesiserSound {
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
}

WingmanStudioAudioProcessor::WingmanStudioAudioProcessor()
: AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
  parameters(*this, nullptr, "PARAMETERS", makeParams())
{
    for (int i=0;i<8;++i) synth.addVoice(new SimpleVoice());
    synth.addSound(new SimpleSound());
}
juce::AudioProcessorValueTreeState::ParameterLayout WingmanStudioAudioProcessor::makeParams()
{
    juce::AudioProcessorValueTreeState::ParameterLayout p;
    p.add(std::make_unique<juce::AudioParameterFloat>("gain","Output",juce::NormalisableRange<float>(0.0f,1.0f,0.001f),0.7f));
    p.add(std::make_unique<juce::AudioParameterFloat>("tempo","Tempo",juce::NormalisableRange<float>(60.0f,180.0f,1.0f),120.0f));
    p.add(std::make_unique<juce::AudioParameterChoice>("key","Key / Scale",juce::StringArray{"C Minor","A Minor","D Minor","E Minor","G Major"},1));
    p.add(std::make_unique<juce::AudioParameterChoice>("mode","Generator",juce::StringArray{"Chord pulse","Arpeggio","Bass pulse"},0));
    p.add(std::make_unique<juce::AudioParameterBool>("internalSound","Built-in sound",true));
    return p;
}
void WingmanStudioAudioProcessor::prepareToPlay(double sampleRate,int)
{
    sr=sampleRate; synth.setCurrentPlaybackSampleRate(sr); lastBar=-1;
}
bool WingmanStudioAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    return l.getMainOutputChannelSet()==juce::AudioChannelSet::mono() || l.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();
}
void WingmanStudioAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    auto* ph=getPlayHead();
    if (ph) {
        auto pos=ph->getPosition();
        if (pos && pos->getPpqPosition()) {
            const double ppq=*pos->getPpqPosition();
            const int bar=(int)std::floor(ppq/4.0);
            if (bar!=lastBar && bar>=0) {
                lastBar=bar;
                const int key=(int)parameters.getRawParameterValue("key")->load();
                const int mode=(int)parameters.getRawParameterValue("mode")->load();
                const int roots[]={48,45,50,52,55};
                const int root=roots[juce::jlimit(0,4,key)];
                const int triad[]={0,3,7,10};
                const int chord=(bar%4);
                const int noteRoot=root + ((chord==1)?5:((chord==2)?7:((chord==3)?2:0)));
                const int n=mode==2 ? noteRoot-12 : noteRoot;
                midi.addEvent(juce::MidiMessage::noteOn(1,n,(juce::uint8)95),0);
                if(mode==0) for(int k=1;k<3;++k) midi.addEvent(juce::MidiMessage::noteOn(1,n+triad[k],(juce::uint8)78),0);
                if(mode==1) midi.addEvent(juce::MidiMessage::noteOn(1,n+triad[(bar%3)+1],(juce::uint8)85),0);
                midi.addEvent(juce::MidiMessage::noteOff(1,n),buffer.getNumSamples()-1);
            }
        }
    }
    if (parameters.getRawParameterValue("internalSound")->load()>0.5f)
        synth.renderNextBlock(buffer,midi,0,buffer.getNumSamples());
    buffer.applyGain(parameters.getRawParameterValue("gain")->load());
}
void WingmanStudioAudioProcessor::nextProgression(){ progression.store((progression.load()+1)%progressions.size()); }
juce::String WingmanStudioAudioProcessor::progressionName() const { return progressions[progression.load()]; }
void WingmanStudioAudioProcessor::getStateInformation(juce::MemoryBlock& d)
{
    auto s=parameters.copyState(); s.setProperty("progression",progression.load(),nullptr);
    std::unique_ptr<juce::XmlElement> x(s.createXml()); copyXmlToBinary(*x,d);
}
void WingmanStudioAudioProcessor::setStateInformation(const void* data,int size)
{
    std::unique_ptr<juce::XmlElement> x(getXmlFromBinary(data,size));
    if(x && x->hasTagName(parameters.state.getType())) {
        auto s=juce::ValueTree::fromXml(*x); parameters.replaceState(s);
        progression.store((int)s.getProperty("progression",0)%progressions.size());
    }
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){ return new WingmanStudioAudioProcessor(); }
