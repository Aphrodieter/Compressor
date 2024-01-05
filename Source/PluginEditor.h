/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "WaveshaperImage.h"

//==============================================================================
/**
*/
using namespace juce;
class BandLookAndFeel : public LookAndFeel_V4
{
public:
    BandLookAndFeel(const juce::Colour& color)
    {
        setColour(Slider::ColourIds::rotarySliderFillColourId, color);
        setColour(Slider::ColourIds::textBoxOutlineColourId, juce::Colours::darkolivegreen);

        setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::black);
        setColour(ToggleButton::ColourIds::textColourId, juce::Colours::black);
        setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);
        setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);

        setColour(Label::ColourIds::textColourId, juce::Colours::black);


        setColour(ComboBox::ColourIds::textColourId, juce::Colours::black);
        setColour(ComboBox::ColourIds::arrowColourId, juce::Colours::black);
        setColour(ComboBox::ColourIds::backgroundColourId, juce::Colours::burlywood);
        setColour(ComboBox::ColourIds::outlineColourId, juce::Colours::darkolivegreen);


    }

    Slider::SliderLayout getSliderLayout(Slider& slider) override
    {
        Slider::SliderLayout layout;
        layout.sliderBounds = slider.getLocalBounds();

        auto textBoxBounds = slider.getLocalBounds().withSizeKeepingCentre(40, 24);
        layout.textBoxBounds = textBoxBounds.withY(textBoxBounds.getY() + 20);
        return layout;
    }
};

using namespace juce;
class FilterLookAndFeel : public LookAndFeel_V4
{
public:
    FilterLookAndFeel(const juce::Colour& colorLeft, const juce::Colour& colorRight)
    {
        setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::black);
        setColour(Slider::ColourIds::textBoxOutlineColourId, juce::Colours::black);
        setColour(Slider::ColourIds::thumbColourId, juce::Colours::black);

        setColour(Slider::ColourIds::backgroundColourId, colorRight);
        setColour(Slider::ColourIds::trackColourId, colorLeft);


    }
};

using namespace juce;
class SliderLookAndFeel: public LookAndFeel_V4
{
public:
    SliderLookAndFeel()
    {

    }

    Slider::SliderLayout getSliderLayout(Slider& slider) override
    {
        Slider::SliderLayout layout;
        layout.sliderBounds = slider.getLocalBounds();
        layout.textBoxBounds = slider.getLocalBounds().withSizeKeepingCentre(40, 24);
        return layout;
    }
private:


};

using namespace juce;
class LabelRotarySlider : public juce::Component
{
public:
    LabelRotarySlider(const String& text, float textSize) {
        slider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 50, 20);
        label.setText(text, juce::NotificationType::dontSendNotification);
        label.setJustificationType(Justification::centred);

        Font font(textSize);
        label.setFont(font);
        
        stringWidth = font.getStringWidthFloat(text);
        stringHeight = font.getHeight();
        //slider.setLookAndFeel(&sliderlaf);
        addAndMakeVisible(label);
        addAndMakeVisible(slider);
    }

    void paint(Graphics& g) override
    {
        /*auto bounds = getLocalBounds();
        g.drawRect(bounds.getWidth() / 2 - stringWidth / 2, bounds.getHeight() / 2 - stringHeight / 2, stringWidth, stringHeight);*/
    }

    void resized() override
    {
        auto& bounds = getLocalBounds();
        slider.setBounds(bounds);

        //label.setBounds(bounds.getWidth()/2 - stringWidth/2, bounds.getHeight()/2 - stringHeight/2, stringWidth, stringHeight);
        label.setBounds(bounds.withSizeKeepingCentre(stringWidth+5, stringHeight));
    }

    Slider& getSlider()
    {
        return slider;
    }
private:
    juce::Label label;
    juce::Slider slider;
    float stringWidth;
    float stringHeight;
    SliderLookAndFeel sliderlaf;

};
using namespace juce;
class SingleBandControl : public juce::Component
{
public:
    SingleBandControl(AudioProcessorValueTreeState& apvts, const String& bandName): apvts(apvts)
    {
        for (auto& slider : sliders)
        {
            addAndMakeVisible(slider);
        }

        std::array<String, 8> paramNames;
        paramNames[0] = bandName + " Threshold";
        paramNames[1] = bandName + " Ratio";
        paramNames[2] = bandName + " Attack";
        paramNames[3] = bandName + " Release";
        paramNames[4] = bandName + " Gain";
        paramNames[5] = bandName + " RCMode";
        paramNames[6] = bandName + " Bypass";
        paramNames[7] = bandName + " Solo";


        using namespace params;

         slider_attachment0 = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[0], sliders[0].getSlider());
         slider_attachment1 = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[1], sliders[1].getSlider());
         slider_attachment2 = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[2], sliders[2].getSlider());
         slider_attachment3 = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[3], sliders[3].getSlider());
         slider_attachment4 = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[4], sliders[4].getSlider());

         comboBoxAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, paramNames[5], RCmode);
         bypassAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, paramNames[6], bypass);
         soloAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts, paramNames[7], solo);


        
        bypass.changeWidthToFitText();
        solo.changeWidthToFitText();
        addAndMakeVisible(bypass);
        addAndMakeVisible(solo);


        //RCmode.addItemList(juce::StringArray("normal", "percentual", "level"));
        RCmode.addItem("normal", 1);
        RCmode.addItem("percentual", 2);
        RCmode.addItem("level", 3);
        RCmode.setSelectedItemIndex(0);
        //RCmode.setSelectedId(0);
        addAndMakeVisible(RCmode);
    }

    void resized() override
    {

        minheight = (float)getLocalBounds().getHeight() / 7;
        minwidth = (float)getLocalBounds().getWidth();
        for (auto& slider : sliders)
        {
            box.items.add(FlexItem(slider).withWidth(minwidth).withHeight(minheight));//.withMinHeight(minheight).withMinWidth(minheight));
        }
       
        box.items.add(FlexItem(RCmode).withWidth(100).withHeight(minheight));
        box.items.add(FlexItem(bypass).withWidth(100).withHeight(minheight));
        box.items.add(FlexItem(solo).withWidth(100).withHeight(minheight));


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

    float textSize = 20.0f;
    std::array<LabelRotarySlider, 5> sliders{ LabelRotarySlider("Thres", textSize), LabelRotarySlider("Ratio", textSize), LabelRotarySlider("Att", textSize), LabelRotarySlider("Rel", textSize), LabelRotarySlider("Gain", textSize) };

    ToggleButton bypass{ "bypass" }, solo{"solo"};
    ComboBox RCmode{ "RCMode" };

    float  minheight{ 100 };
    float minwidth{ 100 };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> slider_attachment0, slider_attachment1, slider_attachment2, slider_attachment3, slider_attachment4;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment, soloAttachment;

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
       auto bounds = getLocalBounds();
       g.setColour(juce::Colours::black);
       g.fillAll();
       bounds.reduce(5, 0);
       g.setColour(color);

       g.fillRoundedRectangle(bounds.toFloat(), 5);
   }

   void resized() override
   {
       const auto& bounds = getLocalBounds();
 
       for (size_t i; i < singleBands.size(); i++)
       {
           singleBands[i].setLookAndFeel(&lookAndFeels[i]);
           ControlRow.items.add(FlexItem(singleBands[i]).withMinHeight(bounds.getHeight()).withMinWidth((float)bounds.getWidth() / 4));

       }

       ControlRow.performLayout(getLocalBounds());
   }

private:
    AudioProcessorValueTreeState& apvts;
    juce::Colour color;
    std::array < SingleBandControl, 4> singleBands = { SingleBandControl(apvts, "Lowband"), SingleBandControl(apvts, "Low-Midband") ,SingleBandControl(apvts, "High-Midband") ,SingleBandControl(apvts, "Highband")};
    FlexBox ControlRow
    {
    FlexBox::Direction::row,
    FlexBox::Wrap::noWrap,
    FlexBox::AlignContent::center,
    FlexBox::AlignItems::center,
    FlexBox::JustifyContent::center
    };

    std::array<BandLookAndFeel, 4> lookAndFeels = { BandLookAndFeel(juce::Colours::darkblue), BandLookAndFeel(juce::Colours::darkcyan) ,BandLookAndFeel(juce::Colours::lightcoral) ,BandLookAndFeel(juce::Colours::lightgoldenrodyellow) };
};

using namespace juce;
class FilterControls : public Component
{
public:
    FilterControls(const Colour& color, AudioProcessorValueTreeState& apvts): apvts(apvts), color(color)
    {
        for (size_t i = 0; i < sliders.size(); i++)
        {
            addAndMakeVisible(sliders[i]);
            sliders[i].setSliderStyle(Slider::SliderStyle::LinearHorizontal);
            sliders[i].setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
            attachements[i] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[i], sliders[i]);
            sliders[i].setLookAndFeel(&lookAndFeels[i]);
        }

        control_area_text.setText("FILTERSEKTION", NotificationType::dontSendNotification);
        control_area_text.setJustificationType(Justification::centredTop);
        control_area_text.setFont(Font(20.0f).boldened());
        addAndMakeVisible(control_area_text);


    }

    void paint(Graphics& g) override
    {
        auto bounds = getLocalBounds();
        g.setColour(juce::Colours::black);
        g.fillAll();
        bounds.reduce(5, 5);
        g.setColour(color);
        g.fillRoundedRectangle(bounds.toFloat(), 5);
    }

    void resized() override
    {
        const auto& bounds = getLocalBounds();

        for (auto& slider: sliders)
        {
            flexbox.items.add(FlexItem(slider).withWidth((float)bounds.getWidth()/3).withHeight((float)bounds.getHeight()/3));
        }

        flexbox.performLayout(getLocalBounds());
        auto text = control_area_text.getText();
        auto font = control_area_text.getFont();
        auto textWidth = font.getStringWidthFloat(text);
        auto textHeight = font.getHeight();
        control_area_text.setBounds(getWidth() / 2 - textWidth /2 , getHeight() / 6, textWidth, textHeight);
    }
private:
    std::array<Slider, 3> sliders;
    Slider& low_lowmid_crossover = sliders[0];
    Slider& lowmid_highmid_crossover = sliders[1];
    Slider& highmid_high_crossover = sliders[2];

    std::array<std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>, 3> attachements;
    std::array<String, 3> paramNames = { "Low-Lowmid-Cutoff", "Lowmid-Highmid-Cutoff","Highmid-High-Cutoff" };

    AudioProcessorValueTreeState& apvts;
    const Colour& color;

    FlexBox flexbox
    {
        FlexBox::Direction::row,
        FlexBox::Wrap::noWrap,
        FlexBox::AlignContent::center,
        FlexBox::AlignItems::center,
        FlexBox::JustifyContent::center
    };
    Label control_area_text;

    std::array<FilterLookAndFeel, 3> lookAndFeels = { FilterLookAndFeel(juce::Colours::darkblue, juce::Colours::darkcyan), FilterLookAndFeel(juce::Colours::darkcyan,juce::Colours::lightcoral) ,FilterLookAndFeel(juce::Colours::lightcoral,juce::Colours::lightgoldenrodyellow) };
};

using namespace juce;
class SaturationControls: public Component, public Slider::Listener {
public:
    SaturationControls(Colour color, AudioProcessorValueTreeState& apvts): color(color), apvts(apvts)
    {
        

        sliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, "Drive", drive.getSlider());

        waveshaperImage.setMaxDrive(drive.getSlider().getMaximum());

        drive.getSlider().addListener(this);
        addAndMakeVisible(drive);
        addAndMakeVisible(waveshaperImage);
    }

    void sliderValueChanged(Slider* slider) override
    {
        DBG(slider->getValue());
        auto imageN = waveshaperImage.getImageCount();
        auto imageIndex = (slider->getValue() - 1) * imageN / slider->getMaximum();
        waveshaperImage.setImageIndex(imageIndex);
    }

    void paint(Graphics& g) override
    {
        auto bounds = getLocalBounds();
        g.setColour(juce::Colours::black);
        g.fillAll();
        bounds.reduce(5, 5);
        g.setColour(color);
        g.fillRoundedRectangle(bounds.toFloat(), 5);

    }

    void resized() override
    {

        auto bounds = getLocalBounds();
        auto height = bounds.getHeight();
        auto width = bounds.getWidth();

        drive.setBounds(bounds.removeFromBottom(bounds.getHeight()/2).reduced(50));
        waveshaperImage.setBounds(bounds.reduced(20,20));
    }
private:
    LabelRotarySlider drive{ "D" , 40.0f};
    WaveshaperImage waveshaperImage;
    Colour color;
    AudioProcessorValueTreeState& apvts;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
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
    CompressorControls compressorControls{juce::Colours::burlywood, audioProcessor.apvts};
    FilterControls filterControls{ juce::Colours::darkolivegreen, audioProcessor.apvts };
    SaturationControls saturationControls{ juce::Colours::green , audioProcessor.apvts };
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorAudioProcessorEditor)
};
