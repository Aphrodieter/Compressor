/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MyCompressor.h"
#include "MyWaveshaper.h"

//==============================================================================
/**
*/

namespace params {
    enum Params
    {
        low_band_threshold,
        low_band_ratio,
        low_band_attack,
        low_band_release,
        low_band_makeup,
        low_band_bypass,
        low_band_solo,
        low_band_RCmode,


        lowmid_band_threshold,
        lowmid_band_ratio,
        lowmid_band_attack,
        lowmid_band_release,
        lowmid_band_makeup,
        lowmid_band_bypass,
        lowmid_band_solo,
        lowmid_band_RCmode,

        highmid_band_threshold,
        highmid_band_ratio,
        highmid_band_attack,
        highmid_band_release,
        highmid_band_makeup,
        highmid_band_bypass,
        highmid_band_solo,
        highmid_band_RCmode,

        high_band_threshold,
        high_band_ratio,
        high_band_attack,
        high_band_release,
        high_band_makeup,
        high_band_bypass,
        high_band_solo,
        high_band_RCmode,

        low_lowmid_cutoff,
        lowmid_highmid_cutoff,
        highmid_high_cutoff,
    };

    static const std::map<Params, juce::String> getStringMap()
    {
        const std::map<Params, juce::String> map = 
        {
            {low_band_threshold, "Lowband Threshold"},
            {low_band_ratio, "Lowband Ratio"},
            {low_band_attack, "Lowband Attack"},
            {low_band_release, "Lowband Release"},
            {low_band_makeup, "Lowband Gain"},
            {low_band_bypass, "Lowband Bypass"},
            {low_band_solo, "Lowband Solo"},
            {low_band_RCmode, "Lowband RCMode"},

            {lowmid_band_threshold, "Low-Midband Threshold"},
            {lowmid_band_ratio, "Low-Midband Ratio"},
            {lowmid_band_attack, "Low-Midband Attack"},
            {lowmid_band_release, "Low-Midband Release"},
            {lowmid_band_makeup, "Low-Midband Gain"},
            {lowmid_band_bypass, "Low-Midband Bypass"},
            {lowmid_band_solo, "Low-Midband Solo"},
            {lowmid_band_RCmode, "Low-Midband RCMode"},
            
            {highmid_band_threshold, "High-Midband Threshold"},
            {highmid_band_ratio, "High-Midband Ratio"},
            {highmid_band_attack, "High-Midband Attack"},
            {highmid_band_release, "High-Midband Release"},
            {highmid_band_makeup, "High-Midband Gain"},
            {highmid_band_bypass, "High-Midband Bypass"},
            {highmid_band_solo, "High-Midband Solo"},
            {highmid_band_RCmode, "High-Midband RCMode"},
           
            {high_band_threshold, "Highband Threshold"},
            {high_band_ratio, "Highband Ratio"},
            {high_band_attack, "Highband Attack"},
            {high_band_release, "Highband Release"},
            {high_band_makeup, "Highband Gain"},
            {high_band_bypass, "Highband Bypass"},
            {high_band_solo, "Highband Solo"},
            {high_band_RCmode, "Highband RCMode"},
            
            {low_lowmid_cutoff, "Low-Lowmid-Cutoff"},
            {lowmid_highmid_cutoff, "Lowmid-Highmid-Cutoff"},
            {highmid_high_cutoff, "Highmid-High-Cutoff"},
        };

        return map;
    }
}


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

        MyCompressor<float> compressor;
    
        void updateCompressorSettings() 
        {
            compressor.setAttack(attack->get());
            compressor.setRelease(release->get());
            compressor.setThreshold(threshold->get());
            compressor.setRatio(ratio->get());
            compressor.setMakeup(makeup->get());
            compressor.setRCMode(RCMode->getIndex());
        }

        void prepare(const juce::dsp::ProcessSpec& spec)
        {
            compressor.prepare(spec);
        }


        void process(juce::dsp::ProcessContextReplacing<float>& context)
        {
            if (bypass->get())
                context.isBypassed = true;
            compressor.process(context);
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

    //void setWaveshaperFunction(float (*function)(float));



    using APVTS = juce::AudioProcessorValueTreeState;
    APVTS::ParameterLayout createParameterLayout();

    APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout() };
    static constexpr int mapSize = 1024;
    std::array<float, mapSize> waveshaperMap{ 0.0f };
    MyWaveShaper<float, std::function<float(float)>> waveshaper;

private:
    //MyCompressor<float> compressor;
    std::array<Compressorband, 4> compressors;

    Compressorband& low_compressor = compressors[0];
    Compressorband& lowmid_compressor = compressors[1];
    Compressorband& highmid_compressor = compressors[2];
    Compressorband& high_compressor = compressors[3];

    juce::dsp::LinkwitzRileyFilter<float> LP1, AP2, AP3,
                                          HP1, LP2, LP3,
                                                    HP3,
                                               HP2, AP4;

    juce::dsp::LinkwitzRileyFilter<float> invertedAP1, invertedAP2, invertedAP3;

    juce::AudioParameterFloat* low_lowmid_cutoff{ nullptr };
    juce::AudioParameterFloat* lowmid_highmid_cutoff{ nullptr };
    juce::AudioParameterFloat* highmid_high_cutoff{ nullptr };

    juce::AudioParameterBool* low_band_solo{ nullptr };
    juce::AudioParameterBool* lowmid_band_solo{ nullptr };
    juce::AudioParameterBool* highmid_band_solo{ nullptr };
    juce::AudioParameterBool* high_band_solo{ nullptr };
    

    juce::AudioBuffer<float> invertedBuffer;
    std::array<juce::AudioBuffer<float>, 4> buffers;

    

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorAudioProcessor)


};


