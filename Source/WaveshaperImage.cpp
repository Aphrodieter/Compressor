/*
  ==============================================================================

    WaveshaperImage.cpp
    Created: 3 Jan 2024 2:48:39pm
    Author:  ClemensBeissel

  ==============================================================================
*/

#include "WaveshaperImage.h"

/*
  ==============================================================================

    WaveshaperImage.h
    Created: 3 Jan 2024 2:48:39pm
    Author:  ClemensBeissel

  ==============================================================================
*/
using namespace juce;
    WaveshaperImage::WaveshaperImage()
    {
        // create a transparent 500x500 image..
        Image myImage(Image::RGB, 500, 500, true);

        Graphics g(myImage);
        g.setColour(Colours::red);
        g.fillEllipse(20, 20, 300, 200);  // draws a red ellipse in our image.
    }

    void WaveshaperImage::paint(Graphics& g)
    {

    }

    void WaveshaperImage::resized()
    {

    }

