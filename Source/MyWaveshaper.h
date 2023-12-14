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

/**
    Applies waveshaping to audio samples as single samples or AudioBlocks.

    @tags{DSP}
*/
using namespace juce::dsp;
template <typename FloatType, typename Function = FloatType (*) (FloatType)>
class MyWaveShaper
{
public:
    MyWaveShaper() {

        
        initializeMap();


        functionToUse = [&](float input_value) {
            input_value = input_value * 0.5;
            int index = std::floor(std::abs(input_value) * sampleSize);
            index = std::min(sampleSize - 1, index);
            auto output_value = sampledValues[index];
            if (input_value > 0)
                return output_value;
            return output_value * -1;
            };

       /* functionToUse = [&](float input_value) {
            return readGraph(input_value);
            };*/
    };

    void initializeMap()
    {
        float interval_value = (float)1 / sampleSize;
        auto val = 0.0f;
        for (int i = 0; i < sampleSize; i++) {
            sampledValues[i] = val;
            val += interval_value;
        }
    }

    std::vector<juce::Point<int>*> points;
    juce::Path path;
    float windowWidth, windowHeight;

    //==============================================================================
    /** Called before processing starts. */
    void prepare (const ProcessSpec&) noexcept   {}

    //==============================================================================
    /** Returns the result of processing a single sample. */
    template <typename SampleType>
    SampleType JUCE_VECTOR_CALLTYPE processSample (SampleType inputSample) const noexcept
    {
        return functionToUse (inputSample);
    }

    /** Processes the input and output buffers supplied in the processing context. */
    template <typename ProcessContext>
    void process (const ProcessContext& context) const noexcept
    {
        if (context.isBypassed)
        {
            if (context.usesSeparateInputAndOutputBlocks())
                context.getOutputBlock().copyFrom (context.getInputBlock());
        }
        else
        {
            AudioBlock<FloatType>::process (context.getInputBlock(),
                                            context.getOutputBlock(),
                                            functionToUse);
        }
    }


    //fill map
    void readGraph()
    {
        auto interval_value = (float)sampleSize / windowWidth;

        auto x_value = 0.0f;
        for (int i = 0; i < sampleSize; i++) {
            Line<float>* currentLine = nullptr;
            PathFlatteningIterator it(path);

            while (it.next())
            {
                currentLine = &Line<float>(it.x1, it.y1, it.x2, it.y2);
                auto sampleLine = Line<float>(x_value, windowHeight, x_value, 0);

                if (currentLine->intersects(sampleLine)) {
                    juce::Point<float> intersection = currentLine->getIntersection(sampleLine);
                    sampledValues[i] = std::abs(intersection.getY() / windowHeight - 1);
                }
            }

            x_value += interval_value;

        }
    }

    //read single values
    float readGraph(float input_value)
    {
       /* auto x_value = std::abs(input_value * windowWidth);
        Line<float>* currentLine = nullptr;
        
        auto output_value = 0.0f;*/
        PathFlatteningIterator it(path);
        while (it.next())
        {
        /*    currentLine = &Line<float>(it.x1, it.y1, it.x2, it.y2);
            auto sampleLine = Line<float>(x_value, windowHeight, x_value, 0);

            if (currentLine->intersects(sampleLine)) {
                juce::Point<float> intersection = currentLine->getIntersection(sampleLine);
                output_value = std::abs(intersection.getY() / windowHeight - 1);

                if (input_value > 0)
                    return output_value;
                else
                    return output_value * -1;
            }*/
        }
        return input_value;
    }

    void reset() noexcept {}

private:
    Function functionToUse;
    static constexpr int sampleSize = 1024;
    std::array<float, sampleSize> sampledValues;
};
