/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/

class ControlWindow : public juce::Component
{
public:
    juce::Colour color;

    ControlWindow(int x, int y, int z) : color(juce::Colour(x, y, z))
    {

    }
    void paint(juce::Graphics& g) override
    {
        g.fillAll(color);
    }
};

using namespace juce;
class SingleBandControl : public juce::Component
{
public:
    SingleBandControl(AudioProcessorValueTreeState& apvts): apvts(apvts)
    {
        for (auto& slider : sliders)
        {
            slider.setSliderStyle(Slider::SliderStyle::Rotary);
            slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
            addAndMakeVisible(slider);
        }

        const auto& stringMap = params::getStringMap();
        using namespace params;

        AudioProcessorValueTreeState::SliderAttachment(apvts, stringMap.at(Params::low_band_threshold), sliders[0]);


        bypass.changeWidthToFitText();
        addAndMakeVisible(bypass);

        RCmode.addItemList(juce::StringArray("normal", "percentual", "level"), -1);
        //RCmode.setSelectedId(0);
        addAndMakeVisible(RCmode);
    }

    void resized() override
    {
        minheight = getLocalBounds().getHeight() / 7;
        for (auto& slider : sliders)
        {
            box.items.add(FlexItem(slider).withMinHeight(minheight).withMinWidth(minheight));
        }
        box.items.add(FlexItem(bypass).withMinHeight(minheight).withMinWidth(minheight));
        box.items.add(FlexItem(RCmode).withMinHeight(minheight).withMinWidth(minheight));


        box.performLayout(getLocalBounds());
    }

private:
    AudioProcessorValueTreeState& apvts;
    FlexBox box
    {
        FlexBox::Direction::column,
        FlexBox::Wrap::noWrap,
        FlexBox::AlignContent::center,
        FlexBox::AlignItems::center,
        FlexBox::JustifyContent::center
    };

    std::array<Slider, 5> sliders;

    ToggleButton bypass{ "bypass" };
    ComboBox RCmode{ "RCMode" };

    float  minheight{ 50 };
};


using namespace juce;
class CompressorControls : public juce::Component
{
public:
   

   CompressorControls(juce::Colour c, juce::AudioProcessorValueTreeState& apvts): color(c), apvts(apvts)
   {
       for (auto& band : singleBands)
       {
           addAndMakeVisible(band);

       }
   }

   void paint(Graphics& g) override
   {
       g.fillAll(color);
   }

   void resized() override
   {
       const auto& bounds = getLocalBounds();
 
       for (auto& band : singleBands)
       {
           ControlRow.items.add(FlexItem(band).withMinHeight(bounds.getHeight()).withMinWidth(bounds.getWidth()/4).withMargin(FlexItem::Margin(0)));
       }

       ControlRow.performLayout(getLocalBounds());
   }

private:
    AudioProcessorValueTreeState& apvts;
    juce::Colour color;
    std::array < SingleBandControl, 1 > singleBands = { SingleBandControl(apvts)};
    FlexBox ControlRow
    {
    FlexBox::Direction::row,
    FlexBox::Wrap::noWrap,
    FlexBox::AlignContent::center,
    FlexBox::AlignItems::center,
    FlexBox::JustifyContent::center
    };
};



class CompressorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CompressorAudioProcessorEditor (CompressorAudioProcessor&);
    ~CompressorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CompressorAudioProcessor& audioProcessor;
    ControlWindow BandControls{ 200, 200, 200 };
    CompressorControls cc{juce::Colours::burlywood, audioProcessor.apvts};
    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorAudioProcessorEditor)
};
