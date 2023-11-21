#include <JuceHeader.h>
#include "MyEnvelopeDetector.h"

template <typename SampleType>
MyEnvelopeDetector<SampleType>::MyEnvelopeDetector()
{
    setAttackTime(attackTime);
    setReleaseTime(releaseTime);
    setTC(TC);
}

template <typename SampleType>
void MyEnvelopeDetector<SampleType>::setAttackTime(SampleType attackTimeMs)
{
    attackTime = attackTimeMs;
    cteAT = calculateLimitedCte(static_cast<SampleType> (attackTime));
}

template <typename SampleType>
void MyEnvelopeDetector<SampleType>::setReleaseTime(SampleType releaseTimeMs)
{
    releaseTime = releaseTimeMs;
    cteRL = calculateLimitedCte(static_cast<SampleType> (releaseTime));
}

template <typename SampleType>
void MyEnvelopeDetector<SampleType>::setLevelCalculationType(LevelCalculationType newLevelType)
{
    levelType = newLevelType;
    reset();
}

template <typename SampleType>
void MyEnvelopeDetector<SampleType>::setTC(int mode) {
    if (TC != TC_MAP[mode]) {
        TC = TC_MAP[mode];
        setAttackTime(attackTime);
        setReleaseTime(releaseTime);
    }
    
}

template <typename SampleType>
void MyEnvelopeDetector<SampleType>::prepare(const juce::dsp::ProcessSpec& spec)
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    sampleRate = spec.sampleRate;

    setAttackTime(attackTime);
    setReleaseTime(releaseTime);
    setTC(TC);

    yold.resize(spec.numChannels);

    reset();
}

template <typename SampleType>
void MyEnvelopeDetector<SampleType>::reset()
{
    reset(0);
}

template <typename SampleType>
void MyEnvelopeDetector<SampleType>::reset(SampleType initialValue)
{
    for (auto& old : yold)
        old = initialValue;
}

template <typename SampleType>
SampleType MyEnvelopeDetector<SampleType>::processSample(int channel, SampleType inputValue)
{
    jassert(juce::isPositiveAndBelow(channel, yold.size()));

    if (levelType == LevelCalculationType::RMS)
        inputValue *= inputValue;
    else
        inputValue = std::abs(inputValue);

    SampleType cte = (inputValue > yold[(size_t)channel] ? cteAT : cteRL);

    SampleType result = inputValue + cte * (yold[(size_t)channel] - inputValue);
    yold[(size_t)channel] = result;

    if (levelType == LevelCalculationType::RMS)
        return std::sqrt(result);

    return result;
}

template <typename SampleType>
void MyEnvelopeDetector<SampleType>::snapToZero() noexcept
{
    for (auto& old : yold)
        juce::dsp::util::snapToZero(old);
}

template <typename SampleType>
SampleType MyEnvelopeDetector<SampleType>::calculateLimitedCte(SampleType timeMs) const noexcept
{
    return timeMs < static_cast<SampleType> (1.0e-3) ? 0
        : static_cast<SampleType> (std::exp(TC / (timeMs * sampleRate * 0.001)));//static_cast<SampleType> (std::exp (expFactor / timeMs));
}

//==============================================================================
template class MyEnvelopeDetector<float>;
template class MyEnvelopeDetector<double>;