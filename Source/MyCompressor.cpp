/*
  ==============================================================================

    compressor.cpp
    Created: 21 Nov 2023 9:28:21am
    Author:  cleme

  ==============================================================================
*/

#include "MyCompressor.h"
/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   By using JUCE, you agree to the terms of both the JUCE 7 End-User License
   Agreement and JUCE Privacy Policy.

   End User License Agreement: www.juce.com/juce-7-licence
   Privacy Policy: www.juce.com/juce-privacy-policy

   Or: You may also use this code under the terms of the GPL v3 (see
   www.gnu.org/licenses).

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/
#include <JuceHeader.h>

//==============================================================================
template <typename SampleType>
MyCompressor<SampleType>::MyCompressor()
{
    update();
}

//==============================================================================
template <typename SampleType>
void MyCompressor<SampleType>::setThreshold(SampleType newThreshold)
{
    thresholddB = newThreshold;
    update();
}

template <typename SampleType>
void MyCompressor<SampleType>::setRatio(SampleType newRatio)
{
    jassert(newRatio >= static_cast<SampleType> (1.0));

    ratio = newRatio;
    update();
}

template <typename SampleType>
void MyCompressor<SampleType>::setAttack(SampleType newAttack)
{
    attackTime = newAttack;
    update();
}

template <typename SampleType>
void MyCompressor<SampleType>::setRelease(SampleType newRelease)
{
    releaseTime = newRelease;
    update();
}

template <typename SampleType>
void MyCompressor<SampleType>::setMakeup(SampleType makeup) {
    makeupgain = makeup;
    update();
}


//==============================================================================
template <typename SampleType>
void MyCompressor<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    envelopeFilter.prepare(spec);

    update();
    reset();
}

template <typename SampleType>
void MyCompressor<SampleType>::reset()
{
    envelopeFilter.reset();
}

//==============================================================================
template <typename SampleType>
SampleType MyCompressor<SampleType>::processSample(int channel, SampleType inputValue)
{
    auto x_g = juce::Decibels::gainToDecibels(std::abs(inputValue), minus_inf);
    auto y_g = (x_g < thresholddB) ? x_g : thresholddB + ((x_g - thresholddB) / ratio);

    auto x_l = x_g - y_g;
    auto y_l = envelopeFilter.processSample(channel ,x_l);

    auto c = juce::Decibels::decibelsToGain(SampleType (makeupgain-y_l), minus_inf);


    /*auto env = envelopeFilter.processSample(channel, inputValue);
    env = juce::Decibels::gainToDecibels(env, minus_inf);

    auto y = (env < thresholddB) ? env : thresholddB + ((env - thresholddB) / ratio);
    auto gain = juce::Decibels::decibelsToGain(y - env, minus_inf);*/
    
    return inputValue * c;
}

template <typename SampleType>
void MyCompressor<SampleType>::setRCMode(int mode) {
    envelopeFilter.setTC(mode);
}

template <typename SampleType>
void MyCompressor<SampleType>::update()
{
    threshold = juce::Decibels::decibelsToGain(thresholddB, minus_inf);
    //DBG(threshold);
    thresholdInverse = static_cast<SampleType> (1.0) / threshold;
    ratioInverse = static_cast<SampleType> (1.0) / ratio;

    envelopeFilter.setAttackTime(attackTime);
    envelopeFilter.setReleaseTime(releaseTime);
}

//==============================================================================
template class MyCompressor<float>;
template class MyCompressor<double>;
