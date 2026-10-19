#include "PluginEditor.h"

WingmanStudioAudioProcessorEditor::WingmanStudioAudioProcessorEditor(WingmanStudioAudioProcessor& p)
    : AudioProcessorEditor(&p), proc(p)
{
    setSize(920, 620);

    auto label = [&](juce::Label& l, juce::String s, float sz) {
        l.setText(s, juce::dontSendNotification);
        l.setColour(juce::Label::textColourId, juce::Colour(0xffe5f4f7));
        l.setFont(juce::Font(sz, juce::Font::plain));
        addAndMakeVisible(l);
    };

    label(heading, "WINGMAN STUDIO", 28.0f);
    label(status, "MUSICAL IDEAS  /  AUDIO + MIDI WORKBENCH", 12.0f);
    label(progLabel, proc.progressionName(), 21.0f);
    label(keyLabel, "KEY / SCALE", 12.0f);
    label(genLabel, "GENERATOR", 12.0f);
    label(gainLabel, "OUTPUT", 12.0f);

    next.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff00a9a5));
    next.onClick = [this] {
        proc.nextProgression();
        progLabel.setText(proc.progressionName(), juce::dontSendNotification);
        repaint();
    };
    addAndMakeVisible(next);

    key.addItemList({ "C Minor", "A Minor", "D Minor", "E Minor", "G Major" }, 1);
    generator.addItemList({ "Chord pulse", "Arpeggio", "Bass pulse" }, 1);
    addAndMakeVisible(key);
    addAndMakeVisible(generator);
    addAndMakeVisible(builtIn);

    gain.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 20);
    addAndMakeVisible(gain);

    tempo.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    tempo.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 20);
    addAndMakeVisible(tempo);

    gainAtt = std::make_unique<SA>(proc.parameters, "gain", gain);
    tempoAtt = std::make_unique<SA>(proc.parameters, "tempo", tempo);
    keyAtt = std::make_unique<CA>(proc.parameters, "key", key);
    genAtt = std::make_unique<CA>(proc.parameters, "mode", generator);
    soundAtt = std::make_unique<BA>(proc.parameters, "internalSound", builtIn);
}

void WingmanStudioAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff07131b));
    g.setColour(juce::Colour(0xff102630));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(12), 18.0f);
    g.setColour(juce::Colour(0xff00d3cf));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(12), 18.0f, 1.5f);

    g.setColour(juce::Colour(0xff0a1b24));
    g.fillRoundedRectangle(juce::Rectangle<float>(30.0f, 112.0f, static_cast<float>(getWidth()) - 60.0f, 170.0f), 12.0f);
    g.fillRoundedRectangle(juce::Rectangle<float>(30.0f, 300.0f, static_cast<float>(getWidth()) - 60.0f, 270.0f), 12.0f);

    g.setColour(juce::Colour(0xff88aeba));
    g.setFont(13.0f);
    g.drawText("IDEA LANES  •  CHORDS  •  BASS  •  DRUMS  •  MELODY", 46, 305, getWidth() - 92, 24, juce::Justification::left);

    juce::Colour cs[] = {
        juce::Colour(0xff7e82ff),
        juce::Colour(0xffffa34f),
        juce::Colour(0xff25d9a2),
        juce::Colour(0xffe85aa8)
    };

    for (int lane = 0; lane < 4; ++lane)
        for (int n = 0; n < 16; ++n) {
            const float x = 48.0f + n * (static_cast<float>(getWidth()) - 105.0f) / 16.0f;
            const float y = 348.0f + lane * 48.0f + ((n + lane) % 3) * 5.0f;
            g.setColour(cs[lane]);
            g.fillRoundedRectangle(x, y, (static_cast<float>(getWidth()) - 145.0f) / 16.0f, 24.0f, 6.0f);
        }

    g.setColour(juce::Colour(0xff7ea4b2));
    g.setFont(12.0f);
    g.drawText("MIDI output depends on host routing. Built-in tone is a simple starter instrument.", 46, getHeight() - 38, getWidth() - 92, 20, juce::Justification::left);
}

void WingmanStudioAudioProcessorEditor::resized()
{
    heading.setBounds(36, 22, 360, 42);
    status.setBounds(38, 65, 450, 20);
    progLabel.setBounds(46, 126, 330, 36);
    next.setBounds(getWidth() - 230, 122, 180, 42);
    keyLabel.setBounds(48, 174, 130, 20);
    key.setBounds(48, 195, 180, 30);
    genLabel.setBounds(250, 174, 130, 20);
    generator.setBounds(250, 195, 180, 30);
    tempo.setBounds(470, 170, 100, 100);
    gainLabel.setBounds(getWidth() - 180, 170, 100, 20);
    gain.setBounds(getWidth() - 180, 185, 100, 95);
    builtIn.setBounds(600, 218, 140, 26);
}
