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
using namespace juce;
class BandLookAndFeel : public LookAndFeel_V4
{
public:
    BandLookAndFeel(const juce::Colour& color)
    {
        setColour(Slider::ColourIds::rotarySliderFillColourId, color);
        setColour(Slider::ColourIds::textBoxOutlineColourId, juce::Colours::darkolivegreen);

        setColour(Slider::ColourIds::thumbColourId, juce::Colours::black);
        setColour(Slider::ColourIds::textBoxTextColourId, juce::Colours::black);
        setColour(ToggleButton::ColourIds::textColourId, juce::Colours::black);
        setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);
        setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);

        setColour(Label::ColourIds::textColourId, juce::Colours::black);


        setColour(ComboBox::ColourIds::textColourId, juce::Colours::black);
        setColour(ComboBox::ColourIds::arrowColourId, juce::Colours::black);
        setColour(ComboBox::ColourIds::backgroundColourId, juce::Colours::burlywood);
        setColour(ComboBox::ColourIds::focusedOutlineColourId, juce::Colours::burlywood);


        setColour(ComboBox::ColourIds::outlineColourId, juce::Colours::darkolivegreen);


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
class LabelRotarySlider : public juce::Slider
{
public:
    LabelRotarySlider(const String& text) {
        setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
        setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 50);
        label.setText(text, juce::NotificationType::dontSendNotification);
        label.setJustificationType(Justification::bottom);
        label.attachToComponent(this, true);
        //label.attachToComponent(this, false);
        addAndMakeVisible(label);
    }

    //void resized() override
    //{
    //    //auto& bounds = getLocalBounds();
    //    //label.setBounds(bounds.removeFromBottom(bounds.getHeight()/4));
    //    //setBounds(bounds);
    //}
private:
    juce::Label label;

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

        const auto& stringMap = params::getStringMap();
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

         slider_attachment0 = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[0], sliders[0]);
         slider_attachment1 = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[1], sliders[1]);
         slider_attachment2 = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[2], sliders[2]);
         slider_attachment3 = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[3], sliders[3]);
         slider_attachment4 = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[4], sliders[4]);

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
        minheight = getLocalBounds().getHeight() / 7;
        for (auto& slider : sliders)
        {
            box.items.add(FlexItem(slider).withWidth(200).withHeight(200));//.withMinHeight(minheight).withMinWidth(minheight));
        }
       
        box.items.add(FlexItem(RCmode).withWidth(100).withHeight(200));
        box.items.add(FlexItem(bypass).withWidth(200).withHeight(200));
        box.items.add(FlexItem(solo).withWidth(200).withHeight(200));


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

    std::array<LabelRotarySlider, 5> sliders{ LabelRotarySlider("Thres"), LabelRotarySlider("Ratio"), LabelRotarySlider("Att"), LabelRotarySlider("Rel"), LabelRotarySlider("Gain") };

    ToggleButton bypass{ "bypass" }, solo{"solo"};
    ComboBox RCmode{ "RCMode" };

    float  minheight{ 100 };

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
       auto& bounds = getLocalBounds();
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
           ControlRow.items.add(FlexItem(singleBands[i]).withMinHeight(bounds.getHeight()).withMinWidth(bounds.getWidth() / 4).withMargin(FlexItem::Margin(0)));

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
        control_area_text.setFont(control_area_text.getFont().boldened());
        addAndMakeVisible(control_area_text);


        addAndMakeVisible(filterBorder);
        filterBorder.setText("Filter");
        filterBorder.setTextLabelPosition(Justification::centredTop);


    }

    void paint(Graphics& g) override
    {

        auto& bounds = getLocalBounds();
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
            auto item = FlexItem(slider);
            flexbox.items.add(item.withWidth(bounds.getWidth()/3).withHeight(bounds.getHeight()/3));
        }


        flexbox.performLayout(getLocalBounds());
        //filterBorder.setBounds(flexbox.items.getLast().currentBounds.toNearestInt().expanded(0,10).withTrimmedRight(10));

        auto width = 200;
        auto height = 20;
        control_area_text.setBounds(getWidth() / 2 - width/2 , getHeight() / 6, width, height);
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
    GroupComponent filterBorder;
};

using namespace juce;
class WaveshaperGUI : public Component
{
    public:
        class Point : public Component
        {
            public:
                Point(int _x, int _y)
                {
                    x = _x;
                    y = _y;
                }
                void paint(Graphics& g) override
                {
                    g.setColour(juce::Colours::orange);
                    g.fillEllipse(getLocalBounds().toFloat());
                }
                int getX()
                {
                    return x;
                }
                int getY()
                {
                    return y;
                }
            private:
                int x, y;
        };

        WaveshaperGUI() 
        {
        }
        void paint(Graphics& g) override
        {
            auto bounds = getLocalBounds();
            g.setColour(juce::Colours::black);
            g.fillRoundedRectangle(bounds.toFloat(), 5);
            g.setColour(juce::Colours::orange);
            g.drawLine(bounds.getBottomLeft().getX(), bounds.getBottomLeft().getY(), bounds.getTopRight().getX(), bounds.getTopRight().getY());
        }
        void resized() override
        {

        }
        void mouseDoubleClick(const MouseEvent& event) override
        {
            int mouse_x = event.getMouseDownX();
            int mouse_y = event.getMouseDownY();
            int point_x = mouse_x;
            int point_y = mouse_y;

            if (points.size() > 0) {
                Point* lastPoint = points[points.size() - 1];
                point_x = std::max(lastPoint->getX(), mouse_x);
            }
            Point* newPoint = new Point(point_x,point_y);

            newPoint->setBounds(point_x, point_y, pointSize, pointSize);
            addAndMakeVisible(newPoint);

            points.push_back(newPoint);


        }

        void addPoint()
        {

        }
       



    private:
        std::vector<Point*> points;
        Point point{10,10};
        static constexpr int pointSize = 10;
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
    //CompressorControls compressorControls{juce::Colours::burlywood, audioProcessor.apvts};
    //FilterControls filterControls{ juce::Colours::darkolivegreen, audioProcessor.apvts };
    WaveshaperGUI waveshaperGui;
    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompressorAudioProcessorEditor)
};
