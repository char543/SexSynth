/*
  ==============================================================================

    AudioVisualiserComponent.h
    Created: 17 Oct 2024 3:35:32am
    Author:  retrack

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

class CustomAudioVisualiserComponent : public juce::Component, public juce::Timer
{
public:
    CustomAudioVisualiserComponent();

    // Push the next buffer of audio data into the circular buffer
    void pushNextBuffer(const juce::AudioBuffer<float>& buffer);

    // Overriding the paint method to draw the waveform
    void paint(juce::Graphics& g) override;

    // Called periodically to repaint the waveform
    void timerCallback() override;

private:
    std::vector<float> bufferData { std::vector<float>(2048, 0.0f) };  // Circular buffer
    int currentIndex = 0;
};
