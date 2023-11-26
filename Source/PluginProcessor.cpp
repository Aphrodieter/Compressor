/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CompressorAudioProcessor::CompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    using namespace params;
    const auto stringmap = getStringMap();


    low_lowmid_cutoff = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(stringmap.at(Params::low_lowmid_cutoff)));
    lowmid_highmid_cutoff = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(stringmap.at(Params::lowmid_highmid_cutoff)));
    highmid_high_cutoff = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(stringmap.at(Params::highmid_high_cutoff)));

    low_band_solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(stringmap.at(Params::low_band_solo)));
    lowmid_band_solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(stringmap.at(Params::lowmid_band_solo)));
    highmid_band_solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(stringmap.at(Params::highmid_band_solo)));
    high_band_solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(stringmap.at(Params::high_band_solo)));


    low_compressor.init(apvts);

}

CompressorAudioProcessor::~CompressorAudioProcessor()
{
}

//==============================================================================
const juce::String CompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CompressorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String CompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void CompressorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void CompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    LP1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    LP2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    LP3.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);

    HP1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    HP2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    HP3.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    AP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    AP3.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    AP4.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    invertedAP1.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    invertedAP2.setType(juce::dsp::LinkwitzRileyFilterType::allpass);
    invertedAP3.setType(juce::dsp::LinkwitzRileyFilterType::allpass);

    LP1.prepare(spec);
    LP2.prepare(spec);
    LP3.prepare(spec);

    HP1.prepare(spec);
    HP2.prepare(spec);
    HP3.prepare(spec);

    AP2.prepare(spec);
    AP3.prepare(spec);
    AP4.prepare(spec);


    invertedAP1.prepare(spec);
    invertedAP2.prepare(spec);
    invertedAP3.prepare(spec);

    low_compressor.prepare(spec);
    
}

void CompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

bool anyOtherBandSoloed(const std::array<bool, 4>& solos)
{
    for (auto& solo : solos)
        if (solo)
            return true;
    return false;
}


void CompressorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    

   
    buffers[0] = buffer;
    buffers[1] = buffer;

    auto block0 = juce::dsp::AudioBlock<float>(buffers[0]);
    auto block1 = juce::dsp::AudioBlock<float>(buffers[1]);
    auto block2 = juce::dsp::AudioBlock<float>(buffers[2]);
    auto block3 = juce::dsp::AudioBlock<float>(buffers[3]);
    
         
    auto context0 = juce::dsp::ProcessContextReplacing<float>(block0);
    auto context1 = juce::dsp::ProcessContextReplacing<float>(block1);
    auto context2 = juce::dsp::ProcessContextReplacing<float>(block2);
    auto context3 = juce::dsp::ProcessContextReplacing<float>(block3);
    
    auto cutoff_low_lowmid = low_lowmid_cutoff->get();
    auto cutoff_lowmid_highmid = lowmid_highmid_cutoff->get();
    auto cutoff_highmid_high = highmid_high_cutoff->get();

    LP1.setCutoffFrequency(cutoff_low_lowmid);
    HP1.setCutoffFrequency(cutoff_low_lowmid);
    AP2.setCutoffFrequency(cutoff_lowmid_highmid);
    LP2.setCutoffFrequency(cutoff_highmid_high);
    HP2.setCutoffFrequency(cutoff_highmid_high);
    AP3.setCutoffFrequency(cutoff_highmid_high);
    LP3.setCutoffFrequency(cutoff_lowmid_highmid);
    HP3.setCutoffFrequency(cutoff_lowmid_highmid);
    AP4.setCutoffFrequency(cutoff_lowmid_highmid);

    invertedAP1.setCutoffFrequency(cutoff_low_lowmid);
    invertedAP2.setCutoffFrequency(cutoff_lowmid_highmid);

    invertedAP3.setCutoffFrequency(cutoff_highmid_high);

    invertedBuffer.makeCopyOf(buffer);
    auto invertedBlock = juce::dsp::AudioBlock<float>(invertedBuffer);
    auto invertedContext = juce::dsp::ProcessContextReplacing<float>(invertedBlock);

    invertedAP1.process(invertedContext);
    invertedAP2.process(invertedContext);
    invertedAP3.process(invertedContext);

    invertedBuffer.applyGain(-1);



    //create lowband
    LP1.process(context0);
    AP2.process(context0);
    AP3.process(context0);

    //create upperband
    HP1.process(context1);

    //create highband
    buffers[2] = buffers[1];
    HP2.process(context2);
    AP4.process(context2);

    //create center bands
    LP2.process(context1);

    //create highmid band
    buffers[3] = buffers[1];
    HP3.process(context3);

    //create lowmid band
    LP3.process(context1);


   





    int n = buffer.getNumSamples();
    int channel_n = buffer.getNumChannels();

    std::array<bool, 4> solos = { low_band_solo->get(), lowmid_band_solo->get(), high_band_solo->get() , highmid_band_solo->get()};

    buffer.clear();
    for (int i = 0; i < channel_n; i++)
    {
        for (int j = 0; j < buffers.size(); j++)
            if(solos[j] || !anyOtherBandSoloed(solos))
            buffer.addFrom(i, 0, buffers[j], i, 0, n);
    }

    if (low_compressor.bypass->get())
        for (int i = 0; i < channel_n; i++)
                buffer.addFrom(i, 0, invertedBuffer, i, 0, n);
   
}


//==============================================================================
bool CompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CompressorAudioProcessor::createEditor()
{
    //return new CompressorAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void CompressorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CompressorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}



juce::AudioProcessorValueTreeState::ParameterLayout CompressorAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;

    using namespace juce;
    const auto stringmap = params::getStringMap();

    using namespace params;

    layout.add(std::make_unique<AudioParameterFloat>(
        stringmap.at(Params::low_band_threshold),
        stringmap.at(Params::low_band_threshold),
        NormalisableRange<float>(-60, 12, 1, 1),
        0));

    layout.add(std::make_unique<AudioParameterFloat>(
        stringmap.at(Params::low_band_ratio),
        stringmap.at(Params::low_band_ratio),
        NormalisableRange<float>(1, 100, 0.5, 0.2),
        4));

    layout.add(std::make_unique<AudioParameterFloat>(
        stringmap.at(Params::low_band_attack),
              stringmap.at(Params::low_band_attack),
              NormalisableRange<float>(5, 500, 1, 1),
        5));

    layout.add(std::make_unique<AudioParameterFloat>(
        stringmap.at(Params::low_band_release),
              stringmap.at(Params::low_band_release),
        NormalisableRange<float>(5, 500, 1, 1),
        200));

    layout.add(std::make_unique<AudioParameterFloat>(
        stringmap.at(Params::low_band_makeup),
        stringmap.at(Params::low_band_makeup),
        NormalisableRange<float>(-120, 12, 0.5, 10),
        1));

    layout.add(std::make_unique<AudioParameterBool>(
        stringmap.at(Params::low_band_bypass),
        stringmap.at(Params::low_band_bypass),
        false
    ));

    layout.add(std::make_unique<AudioParameterChoice>(
        stringmap.at(Params::low_band_RCmode),
        stringmap.at(Params::low_band_RCmode),
        juce::StringArray("Normal RC", "Procentual RC", "Level RC"),
        0
    ));

   

    layout.add(std::make_unique<AudioParameterFloat>(
        stringmap.at(Params::low_lowmid_cutoff),
        stringmap.at(Params::low_lowmid_cutoff),
        NormalisableRange<float>(20, 999, 1, 0.2),
        200
    ));

    layout.add(std::make_unique<AudioParameterFloat>(
        stringmap.at(Params::lowmid_highmid_cutoff),
        stringmap.at(Params::lowmid_highmid_cutoff),
        NormalisableRange<float>(999, 3999, 1, 0.2),
        200
    ));

    layout.add(std::make_unique<AudioParameterFloat>(
        stringmap.at(Params::highmid_high_cutoff),
        stringmap.at(Params::highmid_high_cutoff),
        NormalisableRange<float>(4000, 20000, 1, 0.2),
        200
    ));

    
    layout.add(std::make_unique<AudioParameterBool>(
        stringmap.at(Params::low_band_solo),
        stringmap.at(Params::low_band_solo),
        false
    ));

    layout.add(std::make_unique<AudioParameterBool>(
        stringmap.at(Params::lowmid_band_solo),
        stringmap.at(Params::lowmid_band_solo),
        false
    ));
    layout.add(std::make_unique<AudioParameterBool>(
        stringmap.at(Params::highmid_band_solo),
        stringmap.at(Params::highmid_band_solo),
        false
    ));

    layout.add(std::make_unique<AudioParameterBool>(
        stringmap.at(Params::high_band_solo),
        stringmap.at(Params::high_band_solo),
        false
    ));



    return layout;
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CompressorAudioProcessor();
}

