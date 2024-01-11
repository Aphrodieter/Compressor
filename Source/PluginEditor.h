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
static const juce::Colour FILTERCONTROL_COLOR = juce::Colours::darkolivegreen;
static const juce::Colour BANDCONTROL_COLOR = juce::Colours::burlywood;
static const juce::Colour GENERALCONTROL_COLOR = juce::Colours::darksalmon;
static const juce::Colour SATURATIONCONTROL_COLOR = juce::Colours::darkolivegreen;


using namespace juce;
class BandLookAndFeel : public LookAndFeel_V4
{
public:
	BandLookAndFeel(const juce::Colour& color)
	{
		Colour transparent = Colour::Colour(0.0f, 0.0f, 0.0f, 0.0f);
		setColour(Slider::ColourIds::rotarySliderFillColourId, color);
		setColour(Slider::ColourIds::textBoxOutlineColourId, transparent);

		setColour(Slider::ColourIds::textBoxTextColourId, Colours::black);


		/*setColour(ToggleButton::ColourIds::textColourId, juce::Colours::black);
		setColour(ToggleButton::ColourIds::tickColourId, juce::Colours::black);
		setColour(ToggleButton::ColourIds::tickDisabledColourId, juce::Colours::black);*/

		setColour(Label::ColourIds::textColourId, juce::Colours::black);


		/*setColour(ComboBox::ColourIds::textColourId, juce::Colours::black);
		setColour(ComboBox::ColourIds::arrowColourId, juce::Colours::black);
		setColour(ComboBox::ColourIds::backgroundColourId, juce::Colours::burlywood);
		setColour(ComboBox::ColourIds::outlineColourId, juce::Colours::darkolivegreen);*/


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
class SliderLookAndFeel : public LookAndFeel_V4
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
		label.setBounds(bounds.withSizeKeepingCentre(stringWidth + 5, stringHeight));
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
class SaturationControls : public Component, public Slider::Listener {
public:
	SaturationControls(Colour color, AudioProcessorValueTreeState& apvts) : color(color), apvts(apvts)
	{


		sliderAttachment = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, "Drive", drive.getSlider());

		waveshaperImage.setMaxDrive(drive.getSlider().getMaximum());

		drive.getSlider().addListener(this);
		drive.setLookAndFeel(&laf);
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
		int borderThickness = 5;
		//bounds.reduce(5, 5);
		bounds.removeFromBottom(borderThickness);
		bounds.removeFromRight(borderThickness);
		bounds.removeFromTop(borderThickness);
		g.setColour(color);
		g.fillRoundedRectangle(bounds.toFloat(), 5);

	}

	void resized() override
	{

		auto bounds = getLocalBounds();
		auto height = bounds.getHeight();
		auto width = bounds.getWidth();

		drive.setBounds(bounds.removeFromBottom(bounds.getHeight() / 2));
		waveshaperImage.setBounds(bounds.reduced(20, 20));
	}
private:
	LabelRotarySlider drive{ "D" , 40.0f };
	WaveshaperImage waveshaperImage;
	Colour color;
	AudioProcessorValueTreeState& apvts;
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
	BandLookAndFeel laf{ Colours::black };
};

using namespace juce;
class SingleBandControl : public juce::Component
{
public:
	SingleBandControl(const Colour& color, AudioProcessorValueTreeState& apvts, const String& bandName) : apvts(apvts), color(color)
	{
		for (auto& slider : sliders)
		{
			addAndMakeVisible(slider);
		}

		makeAttachements(bandName);
		


		bypass.setClickingTogglesState(true);
		solo.setClickingTogglesState(true);

		bypass.changeWidthToFitText();
		solo.changeWidthToFitText();

		bypass.setColour(TextButton::ColourIds::buttonOnColourId, Colours::red);
		solo.setColour(TextButton::ColourIds::buttonOnColourId, Colours::red);

		addAndMakeVisible(bypass);
		addAndMakeVisible(solo);


		RCmode.addItem("normal", 1);
		RCmode.addItem("percentual", 2);
		RCmode.addItem("level", 3);
		RCmode.setSelectedItemIndex(0);

		addAndMakeVisible(RCmode);
	}

	void makeAttachements(const String& bandName)
	{
		slider_attachment0.reset();
		slider_attachment1.reset();
		slider_attachment2.reset();
		slider_attachment3.reset();
		slider_attachment4.reset();
		comboBoxAttachment.reset();
		bypassAttachment.reset();
		soloAttachment.reset();


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
		FlexBox sliderBox;
		sliderBox.flexDirection = FlexBox::Direction::row;
		auto bounds = getLocalBounds();
		height = (float)bounds.getHeight();
		width = (float)bounds.getWidth() /6;

		for (auto& slider : sliders)
		{
			sliderBox.items.add(FlexItem(slider).withFlex(1.0f));//.withWidth(width).withHeight(height));//.withMinHeight(minheight).withMinWidth(minheight));
		}

		FlexBox buttonBox;
		buttonBox.flexDirection = FlexBox::Direction::column;

		auto buttonHeight = height / 3;
		buttonBox.items.add(FlexItem(RCmode).withFlex(1.0f));//.withWidth(width).withHeight(buttonHeight));
		buttonBox.items.add(FlexItem(bypass).withFlex(1.0f));
		buttonBox.items.add(FlexItem(solo).withFlex(1.0f));
		
		auto sliderWidthPercentage = 0.9;

		container.items.add(FlexItem(sliderBox).withWidth(bounds.getWidth()* sliderWidthPercentage).withHeight(bounds.getHeight()));
		container.items.add(FlexItem(buttonBox).withWidth(bounds.getWidth() * (1- sliderWidthPercentage)).withHeight(bounds.getHeight()));

		container.performLayout(bounds.reduced(5));
	}

private:
	AudioProcessorValueTreeState& apvts;
	FlexBox container
	{
		FlexBox::Direction::row,
		FlexBox::Wrap::noWrap,
		FlexBox::AlignContent::center,
		FlexBox::AlignItems::center,
		FlexBox::JustifyContent::center
	};

	float textSize = 20.0f;
	std::array<LabelRotarySlider, 5> sliders{ LabelRotarySlider("Threshold", textSize), LabelRotarySlider("Ratio", textSize), LabelRotarySlider("Attack", textSize), LabelRotarySlider("Release", textSize), LabelRotarySlider("Gain", textSize) };

	TextButton bypass{ "bypass" }, solo{ "solo" };
	ComboBox RCmode{ "RCMode" };

	float  height{ 100 };
	float width{ 100 };

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> slider_attachment0, slider_attachment1, slider_attachment2, slider_attachment3, slider_attachment4;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment, soloAttachment;

	const Colour& color;
};

//using namespace juce;
//class CompressorControls : public juce::Component
//{
//public:
//
//
//	CompressorControls(juce::Colour c, juce::AudioProcessorValueTreeState& apvts) : color(c), apvts(apvts)
//	{
//		for (auto& band : singleBands)
//		{
//			addAndMakeVisible(band);
//
//		}
//	}
//
//	SingleBandControl& getBand()
//	{
//		return singleBands[0];
//	}
//
//	void paint(Graphics& g) override
//	{
//		auto bounds = getLocalBounds();
//		g.setColour(juce::Colours::black);
//		g.fillAll();
//		bounds.reduce(5, 0);
//		g.setColour(color);
//
//		g.fillRoundedRectangle(bounds.toFloat(), 5);
//	}
//
//	void resized() override
//	{
//		const auto& bounds = getLocalBounds();
//
//		for (size_t i; i < singleBands.size(); i++)
//		{
//			singleBands[i].setLookAndFeel(&lookAndFeels[i]);
//			ControlRow.items.add(FlexItem(singleBands[i]).withMinHeight(bounds.getHeight()).withMinWidth((float)bounds.getWidth() / 4));
//
//		}
//
//		ControlRow.performLayout(getLocalBounds());
//	}
//
//private:
//	AudioProcessorValueTreeState& apvts;
//	juce::Colour color;
//	std::array < SingleBandControl, 4> singleBands = { SingleBandControl(apvts, "Lowband"), SingleBandControl(apvts, "Low-Midband") ,SingleBandControl(apvts, "High-Midband") ,SingleBandControl(apvts, "Highband") };
//	FlexBox ControlRow
//	{
//	FlexBox::Direction::row,
//	FlexBox::Wrap::noWrap,
//	FlexBox::AlignContent::center,
//	FlexBox::AlignItems::center,
//	FlexBox::JustifyContent::center
//	};
//
//	std::array<BandLookAndFeel, 4> lookAndFeels = { BandLookAndFeel(juce::Colours::darkblue), BandLookAndFeel(juce::Colours::darkcyan) ,BandLookAndFeel(juce::Colours::lightcoral) ,BandLookAndFeel(juce::Colours::lightgoldenrodyellow) };
//};


using namespace juce;
class FilterControls : public Component, public TextButton::Listener
{
public:
	FilterControls(const Colour& color, AudioProcessorValueTreeState& apvts, SingleBandControl& singleBand) : apvts(apvts), color(color), singleBand(singleBand)
	{
		low_selector.setComponentID("Lowband");
		low_mid_selector.setComponentID("Low-Midband");
		high_mid_selector.setComponentID("High-Midband");
		high_selector.setComponentID("Highband");

		singleBand.setLookAndFeel(&BandLookAndFeels[0]);
		for (size_t i = 0; i < bandSelectors.size(); i++)
		{
			bandSelectors[i].setRadioGroupId(1);
			bandSelectors[i].setClickingTogglesState(true);
			addAndMakeVisible(bandSelectors[i]);
			bandSelectors[i].addListener(this);
		}

		for (size_t i = 0; i < sliders.size(); i++)
		{
			addAndMakeVisible(sliders[i]);
			sliders[i].setSliderStyle(Slider::SliderStyle::LinearHorizontal);
			sliders[i].setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
			attachements[i] = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, paramNames[i], sliders[i]);
			sliders[i].setLookAndFeel(&FilterLookAndFeels[i]);
		}

		control_area_text.setText("FILTERSEKTION", NotificationType::dontSendNotification);
		control_area_text.setJustificationType(Justification::centredTop);
		control_area_text.setFont(Font(20.0f).boldened());
		//addAndMakeVisible(control_area_text);


	}

	void buttonClicked(Button* button) override
	{
		auto buttonID = button->getComponentID();
		singleBand.makeAttachements(buttonID);
		

		singleBand.setLookAndFeel(&BandLookAndFeels[bandNames[buttonID]]);
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
		auto& bounds = getLocalBounds();
		auto bandSelectorBounds = bounds.removeFromTop(bounds.getHeight() / 6);
		FlexBox bandSelectorBox(FlexBox::Direction::row,
			FlexBox::Wrap::noWrap,
			FlexBox::AlignContent::center,
			FlexBox::AlignItems::center,
			FlexBox::JustifyContent::center);

		for (auto& button : bandSelectors)
		{
			bandSelectorBox.items.add(FlexItem(button).withWidth((float)bounds.getWidth() / 4).withHeight(bandSelectorBounds.getHeight()));
		}

		bandSelectorBox.performLayout(bandSelectorBounds.reduced(20));

		for (auto& slider : sliders)
		{
			flexbox.items.add(FlexItem(slider).withWidth((float)bounds.getWidth() / 3).withHeight((float)bounds.getHeight() / 3));
		}

		flexbox.performLayout(bounds);
		auto text = control_area_text.getText();
		auto font = control_area_text.getFont();
		auto textWidth = font.getStringWidthFloat(text);
		auto textHeight = font.getHeight();
		control_area_text.setBounds(getWidth() / 2 - textWidth / 2, getHeight() / 6, textWidth, textHeight);
	}
private:
	std::array<Slider, 3> sliders;
	Slider& low_lowmid_crossover = sliders[0];
	Slider& lowmid_highmid_crossover = sliders[1];
	Slider& highmid_high_crossover = sliders[2];

	std::array<TextButton, 4> bandSelectors = { TextButton("Low"), TextButton("Low-Mid"), TextButton("High-Mid"), TextButton("High")};

	TextButton& low_selector = bandSelectors[0];
	TextButton& low_mid_selector = bandSelectors[1];
	TextButton& high_mid_selector = bandSelectors[2];
	TextButton& high_selector = bandSelectors[3];

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

	std::array<FilterLookAndFeel, 3> FilterLookAndFeels = { FilterLookAndFeel(juce::Colours::darkblue, juce::Colours::darkcyan), FilterLookAndFeel(juce::Colours::darkcyan,juce::Colours::lightcoral) ,FilterLookAndFeel(juce::Colours::lightcoral,juce::Colours::lightgoldenrodyellow) };

	std::array<BandLookAndFeel, 4> BandLookAndFeels = { BandLookAndFeel(juce::Colours::darkblue), BandLookAndFeel(juce::Colours::darkcyan) ,BandLookAndFeel(juce::Colours::lightcoral) ,BandLookAndFeel(juce::Colours::lightgoldenrodyellow) };

	SingleBandControl& singleBand;

	std::map<String, int> bandNames = {
		{"Lowband", 0},
		{"Low-Midband", 1},
		{"High-Midband", 2},
		{"Highband", 3}
	};

};

using namespace juce;
class GeneralControls : public Component
{
public:
	GeneralControls(const Colour& color, AudioProcessorValueTreeState& apvts): apvts(apvts), color(color)
	{
		auto stringMap = params::getStringMap();
		sliderAttachement = std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts, stringMap.at(params::dry_wet), dry_wet.getSlider());
		dry_wet.setLookAndFeel(&laf);
		addAndMakeVisible(dry_wet);
	}

	void paint(Graphics& g) override
	{
		auto bounds = getLocalBounds();
		g.setColour(juce::Colours::black);
		g.fillAll();
		int borderThickness = 5;
		//bounds.reduce(5, 5);
		bounds.removeFromBottom(borderThickness);
		bounds.removeFromLeft(borderThickness);
		bounds.removeFromRight(borderThickness);
		g.setColour(color);
		g.fillRoundedRectangle(bounds.toFloat(), 5);
	}

	void resized() override
	{
		FlexBox fb;
		fb.flexDirection = FlexBox::Direction::rowReverse;
		
		
		fb.items.add(FlexItem(dry_wet).withFlex(1.0f));
		fb.performLayout(getLocalBounds());
	}

private:
	AudioProcessorValueTreeState& apvts;
	const Colour& color;
	LabelRotarySlider dry_wet{"Dry/Wet", 15.0f};
	std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> sliderAttachement;
	BandLookAndFeel laf{ Colours::black };
};




class CompressorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
	CompressorAudioProcessorEditor(CompressorAudioProcessor&);
	~CompressorAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	CompressorAudioProcessor& audioProcessor;
	//CompressorControls compressorControls{ juce::Colours::burlywood, audioProcessor.apvts };
	SingleBandControl compressorControls{ BANDCONTROL_COLOR, audioProcessor.apvts, "Lowband"};
	FilterControls filterControls{ FILTERCONTROL_COLOR, audioProcessor.apvts , compressorControls};
	GeneralControls generalControls{ FILTERCONTROL_COLOR, audioProcessor.apvts};
	SaturationControls saturationControls{ BANDCONTROL_COLOR, audioProcessor.apvts };



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompressorAudioProcessorEditor)
};
