/*
  ==============================================================================

    WaveshaperImage.h
    Created: 3 Jan 2024 2:48:39pm
    Author:  ClemensBeissel

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

using namespace juce;
class WaveshaperImage: public Component {
public:
    WaveshaperImage(Colour waveform_color);

    void paint(Graphics& g) override;

    void resized() override;

    void createImage(size_t i, Rectangle<int> bounds, float drive);

    void setImageIndex(size_t imageIndex);

    size_t getImageCount();

    void setMaxDrive(float max_drive);
private:
    std::array<Image, 32> images;
    size_t imageIndex = 0;
    float MAX_DRIVE = 1.0f;
    Colour waveform_color;

};
