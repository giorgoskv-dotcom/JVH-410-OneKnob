#pragma once
#include <JuceHeader.h>

class JVH410AudioProcessor : public juce::AudioProcessor
{
public:
    JVH410AudioProcessor();
    ~JVH410AudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "JVH-410 OneKnob"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::dsp::IIR::Filter<float> preHighPass, preMidBoost;
    juce::dsp::IIR::Filter<float> toneLow, toneMid, toneHigh;
    juce::dsp::IIR::Filter<float> cabLowCut, cabResonance, cabMidDip, cabPresence, cabHighCut;

    float gateEnvelope = 0.0f;

    inline float triodeClip(float x, float drive, float bias) {
        float s = x * drive + bias;
        s = s / std::sqrt(1.0f + s * s);
        return s - (bias / std::sqrt(1.0f + bias * bias));
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JVH410AudioProcessor)
};
