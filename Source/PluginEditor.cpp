/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"





//==============================================================================
CompressorAudioProcessorEditor::CompressorAudioProcessorEditor (CompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(cc);
    //addAndMakeVisible(BandControls);
    //slider.setSliderStyle(Slider::SliderStyle::Rotary);
    //addAndMakeVisible(slider);
    setSize (1000, 1000);
}

CompressorAudioProcessorEditor::~CompressorAudioProcessorEditor()
{
}

//==============================================================================
void CompressorAudioProcessorEditor::paint (juce::Graphics& g)
{

    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    /*g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);*/
}

void CompressorAudioProcessorEditor::resized()
{
    auto& bounds = getBounds();
    cc.setBounds(bounds.removeFromTop(getHeight()/1.5));
    //slider.setBounds(bounds.removeFromLeft(200));
    //cc.setBounds(bounds.removeFromTop(300));
    //BandControls.setBounds(bounds);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
