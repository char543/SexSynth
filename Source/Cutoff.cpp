/*
  ==============================================================================

    Cutoff.cpp
    Created: 17 Oct 2024 2:21:41am
    Author:  retrack

  ==============================================================================
*/

#include "Cutoff.h"

CutoffFilter::CutoffFilter() : cutoffFrequency(2000.0f) {}  // Default cutoff at 2000 Hz

void CutoffFilter::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels)
{
    filters.resize(numChannels);  // Ensure we have a filter for each channel

    // Initialize the filters for each channel with a low-pass filter
    for (auto& filter : filters)
    {
        filter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, cutoffFrequency));
    }
}

void CutoffFilter::processBlock(juce::AudioBuffer<float>& buffer)
{
    const int numChannels = buffer.getNumChannels();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        filters[channel].processSamples(channelData, buffer.getNumSamples());
    }
}

void CutoffFilter::setCutoffFrequency(float newCutoff)
{
    cutoffFrequency = newCutoff;

    // Update the filter coefficients for all channels
    for (auto& filter : filters)
    {
        filter.setCoefficients(juce::IIRCoefficients::makeLowPass(44100.0, cutoffFrequency)); // Default 44.1kHz sample rate
    }
}
