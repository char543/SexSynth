/*
  ==============================================================================

    AudioVisualiserComponent.cpp
    Created: 17 Oct 2024 3:35:19am
    Author:  retrack

  ==============================================================================
*/

#include "CustomAudioVisualiserComponent.h"

CustomAudioVisualiserComponent::CustomAudioVisualiserComponent()
{
    startTimerHz(30);  // Update the display at 30 frames per second
}

void CustomAudioVisualiserComponent::pushNextBuffer(const juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            bufferData[currentIndex] = buffer.getSample(channel, i);
            currentIndex = (currentIndex + 1) % bufferData.size();  // Wrap around circular buffer
        }
    }
}

void CustomAudioVisualiserComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);  // Clear background with black

    const float width = getWidth();
    const float height = getHeight();
    const float halfHeight = height * 0.5f;

    juce::Path waveformPath;
    waveformPath.startNewSubPath(0, halfHeight);  // Start waveform path in the middle

    // Draw the waveform by mapping the buffer values to x/y coordinates
    for (int i = 0; i < bufferData.size(); ++i)
    {
        float x = juce::jmap(float(i), 0.0f, float(bufferData.size()), 0.0f, width);
        float y = juce::jmap(bufferData[i], -1.0f, 1.0f, height, 0.0f);
        waveformPath.lineTo(x, y);
    }

    g.setColour(juce::Colours::white);
    g.strokePath(waveformPath, juce::PathStrokeType(2.0f));  // Stroke the path with a white line
}

void CustomAudioVisualiserComponent::timerCallback()
{
    repaint();  // Trigger a repaint to update the waveform
}
