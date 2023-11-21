#include <JuceHeader.h>
enum class BallisticsFilterLevelCalculationType
{
    peak,
    RMS
};

/**
    A processor to apply standard attack / release ballistics to an input signal.
    This is useful in dynamics processors, envelope followers, modulated audio
    effects and for smoothing animation in data visualisation.

    @tags{DSP}
*/
template <typename SampleType>
class MyEnvelopeDetector
{
public:
    //==============================================================================
    using LevelCalculationType = BallisticsFilterLevelCalculationType;

    //==============================================================================
    /** Constructor. */
    MyEnvelopeDetector();

    //==============================================================================
    /** Sets the attack time in ms.

        Attack times less than 0.001 ms will be snapped to zero and very long attack
        times will eventually saturate depending on the numerical precision used.
    */
    void setAttackTime(SampleType attackTimeMs);

    /** Sets the release time in ms.

        Release times less than 0.001 ms will be snapped to zero and very long
        release times will eventually saturate depending on the numerical precision
        used.
    */
    void setReleaseTime(SampleType releaseTimeMs);

    /** Sets how the filter levels are calculated.

        Level calculation in digital envelope followers is usually performed using
        peak detection with a rectifier function (like std::abs) and filtering,
        which returns an envelope dependant on the peak or maximum values of the
        signal amplitude.

        To perform an estimation of the average value of the signal you can use
        an RMS (root mean squared) implementation of the ballistics filter instead.
        This is useful in some compressor and noise-gate designs, or in specific
        types of volume meters.
    */
    void setLevelCalculationType(LevelCalculationType newCalculationType);

    //==============================================================================
    /** Initialises the filter. */
    void prepare(const juce::dsp::ProcessSpec& spec);

    /** Resets the internal state variables of the filter. */
    void reset();

    /** Resets the internal state variables of the filter to the given initial value. */
    void reset(SampleType initialValue);

    void setTC(int mode); 

    //==============================================================================
    /** Processes the input and output samples supplied in the processing context. */
    template <typename ProcessContext>
    void process(const ProcessContext& context) noexcept
    {
        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();
        const auto numChannels = outputBlock.getNumChannels();
        const auto numSamples = outputBlock.getNumSamples();

        jassert(inputBlock.getNumChannels() <= yold.size());
        jassert(inputBlock.getNumChannels() == numChannels);
        jassert(inputBlock.getNumSamples() == numSamples);

        if (context.isBypassed)
        {
            outputBlock.copyFrom(inputBlock);
            return;
        }

        for (size_t channel = 0; channel < numChannels; ++channel)
        {
            auto* inputSamples = inputBlock.getChannelPointer(channel);
            auto* outputSamples = outputBlock.getChannelPointer(channel);

            for (size_t i = 0; i < numSamples; ++i)
                outputSamples[i] = processSample((int)channel, inputSamples[i]);
        }

#if JUCE_DSP_ENABLE_SNAP_TO_ZERO
        snapToZero();
#endif
    }

    /** Processes one sample at a time on a given channel. */
    SampleType processSample(int channel, SampleType inputValue);

    /** Ensure that the state variables are rounded to zero if the state
        variables are denormals. This is only needed if you are doing
        sample by sample processing.
    */
    void snapToZero() noexcept;

private:
    //==============================================================================
    SampleType calculateLimitedCte(SampleType) const noexcept;

    //==============================================================================
    std::vector<SampleType> yold;


    
    double TC_normal = log(0.368);
    double TC_10_90 = TC_normal * log(9);

    std::array<double, 3> TC_MAP = { TC_normal, TC_10_90, (double)0.0 };

    double TC = TC_normal;


    double sampleRate = 44100.0, expFactor = -0.142;
    
    SampleType attackTime = 1.0, releaseTime = 100.0, cteAT = 0.0, cteRL = 0.0;
    LevelCalculationType levelType = LevelCalculationType::peak;
};

