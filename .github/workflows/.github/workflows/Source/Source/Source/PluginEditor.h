#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class MarshallKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, const float rotaryStartAngle,
                           const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 10.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        juce::Path backgroundArc;
        backgroundArc.addCentredArc (centreX, centreY, radius + 6.0f, radius + 6.0f,
                                     0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (juce::Colour (0xff2a2a2e));
        g.strokePath (backgroundArc, juce::PathStrokeType (4.0f));

        juce::Path valueArc;
        valueArc.addCentredArc (centreX, centreY, radius + 6.0f, radius + 6.0f,
                                0.0f, rotaryStartAngle, angle, true);
        g.setColour (juce::Colour (0xffd4af37));
        g.strokePath (valueArc, juce::PathStrokeType (4.0f));

        g.setColour (juce::Colour (0xff141416));
        g.fillEllipse (centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

        float capRadius = radius * 0.72f;
        g.setColour (juce::Colour (0xffc59b27));
        g.fillEllipse (centreX - capRadius, centreY - capRadius, capRadius * 2.0f, capRadius * 2.0f);

        g.setColour (juce::Colour (0xff111111));
        g.drawEllipse (centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f, 3.0f);

        juce::Path p;
        auto pointerLength = radius * 0.85f;
        auto pointerThickness = 4.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius + 4.0f, pointerThickness, pointerLength * 0.45f);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        g.setColour (juce::Colours::black);
        g.fillPath (p);
    }
};

class JVH410AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    JVH410AudioProcessorEditor (JVH410AudioProcessor&);
    ~JVH410AudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JVH410AudioProcessor& audioProcessor;
    MarshallKnobLookAndFeel customLook;

    juce::Slider gainSlider;
    juce::ToggleButton gateButton { "GATE" };
    juce::ToggleButton cabButton  { "V30 CAB" };
    juce::Slider outSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> gateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> cabAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JVH410AudioProcessorEditor)
};
