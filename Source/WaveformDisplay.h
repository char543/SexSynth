/*
  ==============================================================================

    WaveformDisplay.h
    Created: 17 Oct 2024 3:50:42am
    Author:  retrack

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <algorithm>

class WaveformDisplay : public juce::Component
{
public:
    WaveformDisplay() {}

    // Set the waveform data (called from the editor)
    void setWaveform(const std::vector<float>& newWaveform)
    {
        waveform = newWaveform;
        normalizeWaveform();  // Normalize the waveform whenever it's set
        repaint();  // Request a redraw
    }

    // Set the waveform color
    void setWaveformColor(const juce::Colour& newColor)
    {
        waveformColor = newColor;
        repaint();
    }

    // Zoom controls
    void zoomIn()
    {
        zoomLevel *= 1.2f; // Increase zoom level (show more detail)
        repaint();
    }

    void zoomOut()
    {
        zoomLevel *= 0.8f; // Decrease zoom level (show more of the waveform)
        repaint();
    }

protected:
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);  // Clear background

        if (!waveform.empty())
        {
            g.setColour(waveformColor);
            const auto width = getWidth();
            const auto height = getHeight();

            juce::Path waveformPath;

            // Calculate the number of samples to show based on zoom level
            const int numSamplesToShow = static_cast<int>(waveform.size() / zoomLevel);
            const float sampleIncrement = static_cast<float>(waveform.size()) / numSamplesToShow;

            // Iterate through the waveform data based on zoom level
            for (int i = 0; i < numSamplesToShow; ++i)
            {
                const int sampleIndex = static_cast<int>(i * sampleIncrement);
                const float sampleValue = waveform[sampleIndex];

                float x = static_cast<float>(i) / numSamplesToShow * width;

                // Map the Y-value to fit the full height of the component with padding
                float y = ((1.0f - sampleValue) * (height - topPadding - bottomPadding) * 0.5f) + topPadding;

                if (i == 0)
                    waveformPath.startNewSubPath(x, y);
                else
                    waveformPath.lineTo(x, y);
            }

            g.strokePath(waveformPath, juce::PathStrokeType(2.0f));  // Draw the waveform
        }
    }

private:
    std::vector<float> waveform;   // Holds the waveform data
    juce::Colour waveformColor = juce::Colours::white;  // Color of the waveform

    float zoomLevel = 1.0f;        // Zoom level (initially set to 1.0, meaning no zoom)

    const float topPadding = 10.0f;    // Space above the waveform
    const float bottomPadding = 10.0f; // Space below the waveform

    // Normalize the waveform to fit between -1.0 and 1.0
    void normalizeWaveform()
    {
        if (waveform.empty()) return;

        // Find the min and max values
        float minVal = *std::min_element(waveform.begin(), waveform.end());
        float maxVal = *std::max_element(waveform.begin(), waveform.end());

        // Avoid division by zero
        if (maxVal == minVal) {
            std::fill(waveform.begin(), waveform.end(), 0.0f); // Set all to zero if no variation
            return;
        }

        // Normalize values to the range [-1.0, 1.0]
        for (auto& sample : waveform)
        {
            sample = (sample - minVal) / (maxVal - minVal) * 2.0f - 1.0f; // Scale to [-1, 1]
        }
    }
};
