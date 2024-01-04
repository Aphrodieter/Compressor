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
    WaveshaperImage();

    void paint(Graphics& g) override;

    void resized() override;

    void createImage(size_t i, Rectangle<int> bounds, float drive);
private:
    std::array<Image, 32> images;

};
