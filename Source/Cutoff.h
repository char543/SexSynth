/*
  ==============================================================================

    Cutoff.h
    Created: 17 Oct 2024 2:21:49am
    Author:  retrack

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

class CutoffFilter
{
public:
    // Constructor to initialize filter
    CutoffFilter();

    // Prepares the filter for the given sample rate and block size
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);

    // Processes the audio block with the filter
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Set cutoff frequency and update all filters
    void setCutoffFrequency(float newCutoff);

private:
    std::vector<juce::IIRFilter> filters;  // Filters, one per channel
    float cutoffFrequency;                  // Current cutoff frequency
};
