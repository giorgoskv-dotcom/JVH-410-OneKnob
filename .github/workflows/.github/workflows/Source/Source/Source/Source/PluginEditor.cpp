#include "PluginProcessor.h"
#include "PluginEditor.h"

JVH410AudioProcessorEditor::JVH410AudioProcessorEditor (JVH410AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (340, 460);

    gainSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 22);
    gainSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffd4af37));
    gainSlider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    gainSlider.setLookAndFeel (&customLook);
    addAndMakeVisible (gainSlider);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "GAIN", gainSlider);

    gateButton.setColour (juce::ToggleButton::textColourId, juce::Colours::lightgrey);
    gateButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xffd4af37));
    addAndMakeVisible (gateButton);
    gateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "GATE", gateButton);

    cabButton.setColour (juce::ToggleButton::textColourId, juce::Colours::lightgrey);
    cabButton.setColour (juce::ToggleButton::tickColourId, juce::Colour (0xffd4af37));
    addAndMakeVisible (cabButton);
    cabAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "CAB", cabButton);

    outSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    outSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    outSlider.setColour (juce::Slider::thumbColourId, juce::Colour (0xffd4af37));
    outSlider.setColour (juce::Slider::trackColourId, juce::Colour (0xff3a3a40));
    addAndMakeVisible (outSlider);
    outAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "OUT", outSlider);
}

JVH410AudioProcessorEditor::~JVH410AudioProcessorEditor()
{
    gainSlider.setLookAndFeel (nullptr);
}

void JVH410AudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff121215));

    auto bounds = getLocalBounds().reduced (10);
    g.setColour (juce::Colour (0xffd4af37));
    g.drawRoundedRectangle (bounds.toFloat(), 8.0f, 2.0f);

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (26.0f, juce::Font::bold));
    g.drawText ("JVH-410", 0, 26, getWidth(), 30, juce::Justification::centred);

    g.setColour (juce::Colour (0xffd4af37));
    g.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    g.drawText ("ONEKNOB // HIGH GAIN AMP", 0, 54, getWidth(), 20, juce::Justification::centred);

    g.setColour (juce::Colours::grey);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText ("GAIN", 0, 325, getWidth(), 20, juce::Justification::centred);
    g.drawText ("OUT", 215, 385, 90, 20, juce::Justification::centred);
}

void JVH410AudioProcessorEditor::resized()
{
    gainSlider.setBounds (60, 95, 220, 225);

    gateButton.setBounds (25, 395, 85, 30);
    cabButton.setBounds (115, 395, 95, 30);
    outSlider.setBounds (215, 402, 95, 20);
}
