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

    low_band_solo = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(stringmap.at(Params::low_band_solo)));
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

    low_Band.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    high_Band.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    low_Band.prepare(spec);
    high_Band.prepare(spec);

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

bool anyOtherBandSoloed(std::array<bool, 2> solos)
{
    for (auto& solo : solos)
    {
        if (solo)
            return true;
    }
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

    if (low_compressor.bypass->get())
        return;

    lowBuffer.makeCopyOf(buffer);
    highBuffer.makeCopyOf(buffer);

    juce::AudioBuffer<float> difference_buffer;
    juce::AudioBuffer<float> copy_buffer;
    difference_buffer.makeCopyOf(buffer);
    copy_buffer.makeCopyOf(buffer);
    difference_buffer.applyGain(-1);
   
    auto block_low = juce::dsp::AudioBlock<float>(lowBuffer);
    auto context_low = juce::dsp::ProcessContextReplacing<float>(block_low);

    auto block_high = juce::dsp::AudioBlock<float>(highBuffer);
    auto context_high = juce::dsp::ProcessContextReplacing<float>(block_high);

    low_Band.setCutoffFrequency(low_lowmid_cutoff->get());
    high_Band.setCutoffFrequency(low_lowmid_cutoff->get());
    
    low_Band.process(context_low);
    high_Band.process(context_high);

    int n = buffer.getNumSamples();
    int channel_n = buffer.getNumChannels();

    std::array<bool, 2> solos = { low_band_solo->get(), high_band_solo->get() };

    std::array<juce::AudioBuffer<float>, 2> buffers = { lowBuffer, highBuffer };

    buffer.clear();
    for (int i = 0; i < channel_n; i++)
    {
        for (int j = 0; j < buffers.size(); j++)
        {
            if(solos[j] || !anyOtherBandSoloed(solos))
            buffer.addFrom(i, 0, buffers[j], i, 0, n);

        }
       /* buffer.addFrom(i, 0, lowBuffer, i, 0, n);
        buffer.addFrom(i, 0, highBuffer, i, 0, n);*/
        
        //buffer.addFrom(i, 0, difference_buffer, i, 0, n);
    }

    DBG("buffer Sample: " << buffer.getSample(0, 5));
    DBG("Differnce Sample: " << difference_buffer.getSample(0, 5));
    DBG("");
   
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
        NormalisableRange<float>(-120, 12, 0.5, 5),
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
        NormalisableRange<float>(20, 20000, 1, 1),
        200
    ));

    layout.add(std::make_unique<AudioParameterBool>(
        stringmap.at(Params::low_band_solo),
        stringmap.at(Params::low_band_solo),
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

