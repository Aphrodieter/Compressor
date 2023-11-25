/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MyCompressor.h"

//==============================================================================
/**
*/

class Compressorband
{
    public:
        juce::AudioParameterFloat* attack{ nullptr };
        juce::AudioParameterFloat* release{ nullptr };
        juce::AudioParameterFloat* threshold{ nullptr };
        juce::AudioParameterFloat* ratio{ nullptr };
        juce::AudioParameterFloat* makeup{ nullptr };
        juce::AudioParameterBool* bypass{ nullptr };
        juce::AudioParameterChoice* RCMode{ nullptr };

        juce::AudioParameterFloat* cutoffFrequency{ nullptr };

        MyCompressor<float> compressor;
        juce::dsp::LinkwitzRileyFilter<float> filter;

        void init(juce::AudioProcessorValueTreeState& apvts)
        {
            attack = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Attack"));
            jassert(attack != nullptr);
            release = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Release"));
            jassert(release != nullptr);
            threshold = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Threshold"));
            jassert(threshold != nullptr);
            ratio = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Ratio"));
            jassert(ratio != nullptr);
            makeup = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Makeup"));
            jassert(makeup != nullptr);
            bypass = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("Bypass"));
            jassert(bypass != nullptr);
            RCMode = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("RCMode"));
            jassert(RCMode != nullptr);

            cutoffFrequency = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("CutoffFrequency"));
        }
    
        void updateCompressorSettings() 
        {
            compressor.setAttack(attack->get());
            compressor.setRelease(release->get());
            compressor.setThreshold(threshold->get());
            compressor.setRatio(ratio->get());
            compressor.setMakeup(makeup->get());
            compressor.setRCMode(RCMode->getIndex());
        }

        void UpdateFilterSettings()
        {
            filter.setCutoffFrequency(cutoffFrequency->get());
        }

        void prepare(const juce::dsp::ProcessSpec& spec)
        {
            compressor.prepare(spec);
            filter.prepare(spec);
        }


        void process(juce::AudioBuffer<float>& buffer)
        {
            updateCompressorSettings();
            UpdateFilterSettings();
            auto block = juce::dsp::AudioBlock<float>(buffer);
            auto context = juce::dsp::ProcessContextReplacing<float>(block);

            if (bypass->get())
                context.isBypassed = true;

            //DBG(RCMode->getParameterIndex());

            compressor.process(context);
            filter.process(context);
        }

        Compressorband()
        {
    
        }

        Compressorband(juce::dsp::LinkwitzRileyFilterType filtertyp)
        {
            filter.setType(filtertyp);
            
        }
};

class CompressorAudioProcessor : public juce::AudioProcessor
#if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
#endif
{
public:
    //==============================================================================
    CompressorAudioProcessor();
    ~CompressorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    using APVTS = juce::AudioProcessorValueTreeState;
    APVTS::ParameterLayout createParameterLayout();

    APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout() };
private:
    //MyCompressor<float> compressor;
    Compressorband LP{ juce::dsp::LinkwitzRileyFilterType::lowpass };
    

    

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorAudioProcessor)


};


