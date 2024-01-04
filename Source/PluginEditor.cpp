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
    //addAndMakeVisible(compressorControls);
    //addAndMakeVisible(filterControls);
    addAndMakeVisible(saturationControls);
    setSize (1500, 1000);

    


    setResizable(true, true);



    getLookAndFeel().setColour(Label::ColourIds::textColourId, juce::Colours::black);

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
    //compressorControls.setBounds(bounds.removeFromTop(getHeight()/1.5));
    //filterControls.setBounds(bounds);
    saturationControls.setBounds(bounds);
}
