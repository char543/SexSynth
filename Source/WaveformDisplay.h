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

// WaveformDisplay class to visualize the waveform
class WaveformDisplay : public juce::Component
{
public:
    void setWaveform(const std::vector<float>& newWaveform)
    {
        waveform = newWaveform;
        normalizeWaveform();  // Normalize the waveform whenever it's set
        repaint();
    }

    void setWaveformColor(const juce::Colour& newColor)
    {
        waveformColor = newColor;
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

            // Iterate through waveform data
            for (size_t i = 0; i < waveform.size(); ++i)
            {
                float x = static_cast<float>(i) / (waveform.size() - 1) * width;

                // Map the Y-value to fit the full height of the component with padding
                float y = ((1.0f - waveform[i]) * (height - topPadding - bottomPadding) * 0.5f) + topPadding;

                if (i == 0)
                    waveformPath.startNewSubPath(x, y);
                else
                    waveformPath.lineTo(x, y);
            }

            g.strokePath(waveformPath, juce::PathStrokeType(2.0f));  // Draw the waveform
        }
    }

private:
    std::vector<float> waveform;  // Holds the waveform data
    juce::Colour waveformColor = juce::Colours::white;  // Color of the waveform
    const float topPadding = 10.0f;    // Space above the waveform
    const float bottomPadding = 10.0f; // Space below the waveform

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

