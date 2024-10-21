/*
  ==============================================================================

    ADSR.h
    Created: 18 Oct 2024 2:58:55pm
    Author:  retrack

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <functional>  // Include this for std::function

class ADSREnvelope
{
    public:
    ADSREnvelope()
    {
        // Default values
        adsrParams.attack = 0.01f;
        adsrParams.decay = 0.01f;
        adsrParams.sustain = 0.1f;
        adsrParams.release = 0.01f;
        adsr.setParameters(adsrParams);
    }

    // Set ADSR parameters
    void setParameters(const juce::ADSR::Parameters& newParams)
    {
        adsrParams = newParams;
        adsr.setParameters(adsrParams);
    }

    // Get the current ADSR parameters
    juce::ADSR::Parameters getParameters() const
    {
        return adsrParams;
    }

    // Start the envelope (trigger note on)
    void noteOn()
    {
        adsr.noteOn();
    }

    // Stop the envelope (trigger note off)
    void noteOff()
    {
        adsr.noteOff();
    }

    // Check if the envelope is active
    bool isActive() const
    {
        return adsr.isActive();
    }

    // Get the next sample value for the envelope
    float getNextSample()
    {
        return adsr.getNextSample();
    }

    // Assign this envelope to modulate a specific parameter
    void assignToParameter(std::function<void(float)> modFunction)
    {
        parameterModulationFunction = modFunction;
    }

    // Apply the envelope value to the assigned parameter
    void updateEnvelopeValue()
    {
        float envelopeValue = adsr.getNextSample();
        if (parameterModulationFunction)
        {
            // Pass the envelope value to the parameter
            parameterModulationFunction(envelopeValue);
        }
    }

    private:
    juce::ADSR adsr;               // JUCE's ADSR envelope generator
    juce::ADSR::Parameters adsrParams; // ADSR parameters

    std::function<void(float)> parameterModulationFunction;  // Callback for modulating a parameter
};

