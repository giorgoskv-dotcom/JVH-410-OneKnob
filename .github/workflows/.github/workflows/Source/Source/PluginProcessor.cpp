#include "PluginProcessor.h"
#include "PluginEditor.h"

JVH410AudioProcessor::JVH410AudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

JVH410AudioProcessor::~JVH410AudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout JVH410AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("GAIN", "Gain", 0.0f, 10.0f, 6.5f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("GATE", "Noise Gate", true));
    params.push_back(std::make_unique<juce::AudioParameterBool>("CAB", "V30 Cab", true));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("OUT", "Output", -24.0f, 12.0f, 0.0f));

    return { params.begin(), params.end() };
}

void JVH410AudioProcessor::prepareToPlay (double sampleRate, int)
{
    *preHighPass.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 95.0f, 0.707f);
    *preMidBoost.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 1150.0f, 1.2f, juce::Decibels::decibelsToGain(3.5f));

    *toneLow.coefficients  = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 120.0f, 0.7f, juce::Decibels::decibelsToGain(3.0f));
    *toneMid.coefficients  = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 750.0f, 1.1f, juce::Decibels::decibelsToGain(1.5f));
    *toneHigh.coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 2600.0f, 0.7f, juce::Decibels::decibelsToGain(4.0f));

    *cabLowCut.coefficients    = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 80.0f, 0.85f);
    *cabResonance.coefficients = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 110.0f, 2.2f, juce::Decibels::decibelsToGain(4.5f));
    *cabMidDip.coefficients    = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 450.0f, 1.4f, juce::Decibels::decibelsToGain(-4.0f));
    *cabPresence.coefficients  = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 2400.0f, 1.8f, juce::Decibels::decibelsToGain(5.0f));
    *cabHighCut.coefficients   = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 5400.0f, 0.9f);
}

void JVH410AudioProcessor::releaseResources() {}

void JVH410AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float gainVal = *apvts.getRawParameterValue("GAIN");
    bool gateOn   = *apvts.getRawParameterValue("GATE") > 0.5f;
    bool cabOn    = *apvts.getRawParameterValue("CAB") > 0.5f;
    float outDb   = *apvts.getRawParameterValue("OUT");
    float outGain = juce::Decibels::decibelsToGain(outDb);

    float d1 = 1.8f + gainVal * 0.7f;
    float d2 = 2.2f + gainVal * 1.2f;
    float d3 = 2.0f + gainVal * 1.0f;
    float d4 = 1.5f + gainVal * 0.6f;

    float gateThresh = 0.004f + (gainVal * 0.0005f);

    auto* leftChannel  = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float in = leftChannel[sample];
        if (rightChannel != nullptr)
            in = (in + rightChannel[sample]) * 0.5f;

        if (gateOn) {
            float absIn = std::abs(in);
            if (absIn > gateEnvelope)
                gateEnvelope = 0.99f * gateEnvelope + 0.01f * absIn;
            else
                gateEnvelope = 0.9992f * gateEnvelope + 0.0008f * absIn;

            float gateGain = (gateEnvelope > gateThresh) ? 1.0f : (gateEnvelope / gateThresh);
            in *= (gateGain * gateGain);
        }

        float s = preHighPass.processSample(in);
        s = preMidBoost.processSample(s);

        s = triodeClip(s, d1, 0.08f);
        s = triodeClip(s, d2, -0.12f);
        s = triodeClip(s, d3, 0.14f);
        s = triodeClip(s, d4, -0.05f);

        s = toneLow.processSample(s);
        s = toneMid.processSample(s);
        s = toneHigh.processSample(s);

        if (cabOn) {
            s = cabLowCut.processSample(s);
            s = cabResonance.processSample(s);
            s = cabMidDip.processSample(s);
            s = cabPresence.processSample(s);
            s = cabHighCut.processSample(s);
            s *= 0.45f;
        } else {
            s *= 0.25f;
        }

        s *= outGain;

        leftChannel[sample] = s;
        if (rightChannel != nullptr)
            rightChannel[sample] = s;
    }
}

juce::AudioProcessorEditor* JVH410AudioProcessor::createEditor()
{
    return new JVH410AudioProcessorEditor (*this);
}

void JVH410AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void JVH410AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JVH410AudioProcessor();
}
