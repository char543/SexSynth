/*
 The block below describes the properties of this PIP. A PIP is a short snippet
 of code that can be read by the Projucer and used to generate a JUCE project.

 BEGIN_JUCE_PIP_METADATA

 name:             SexSynth
 version:          6.9.420
 vendor:           Bojo Corp
 website:          http://juce.com
 description:      Multi-out synth audio plugin.

 dependencies:     juce_audio_basics, juce_audio_devices, juce_audio_formats,
 juce_audio_plugin_client, juce_audio_processors,
 juce_audio_utils, juce_core, juce_data_structures,
 juce_events, juce_graphics, juce_gui_basics, juce_gui_extra
 exporters:        xcode_mac, vs2019, linux_make

 type:             AudioProcessor
 mainClass:        MultiOutSynth

 useLocalCopy:     1

 END_JUCE_PIP_METADATA
 */

#pragma once

#include <JuceHeader.h>
#include "Cutoff.h"
#include "ADSR.h"
//#include "SynthEditor.h" // Include the SynthEditor header
//class SynthEditor;
class Wavetable
{
    public:
    Wavetable() : phaseIndex(0.0f), phaseIncrement(0.0f), waveformSize(0) {}

    // Set the waveform and reset phase parameters
    void setWaveform(const std::vector<float>& newWaveform)
    {
        waveform = newWaveform;
        waveformSize = waveform.size(); // Store the size of the waveform
        phaseIndex = 0.0f; // Reset phase index
        phaseIncrement = 0.0f; // Reset phase increment
        std::cout << "Waveform size set to: " << waveformSize << std::endl; // Log waveform size
    }

    // Set the frequency and calculate the phase increment
    void setFrequency(float frequency, double sampleRate)
    {
        if (waveformSize > 0 && sampleRate > 0.0) // Ensure valid sample rate
        {
            phaseIncrement = (frequency / sampleRate) * waveformSize; // Updated to control frequency
        }
        else
        {
            phaseIncrement = 0.0f; // Prevent phase increment from being non-zero if sampleRate is invalid
        }
    }

    // Retrieve the next sample from the waveform
    float getNextSample()
    {
        // Ensure we wrap phaseIndex correctly
        int sampleIndex = static_cast<int>(phaseIndex) % waveformSize; // Wrap phase index within bounds

        // Increment phase index
        phaseIndex += phaseIncrement;

        // Wrap around if needed
        if (phaseIndex >= waveformSize)
        {
            phaseIndex -= waveformSize; // Reset to the beginning
        }

        return waveform[sampleIndex]; // Return the current sample from the waveform
    }

    // Return the size of the waveform
    size_t getWaveformSize() const
    {
        return waveformSize; // Return the size of the waveform
    }

    // Create a sine wave
    static std::vector<float> createSineWave(int waveSize = 512)
    {
        std::vector<float> wave(waveSize);
        for (int i = 0; i < waveSize; ++i)
        {
            wave[i] = std::sin((2.0 * M_PI * i) / waveSize);
        }
        return wave;
    }

    // Create a square wave
    static std::vector<float> createSquareWave(int waveSize = 512)
    {
        std::vector<float> wave(waveSize);
        for (int i = 0; i < waveSize; ++i)
        {
            wave[i] = (i < waveSize / 2) ? 1.0f : -1.0f; // High for first half, low for second half
        }
        return wave;
    }

    // Create a sawtooth wave
    static std::vector<float> createSawtoothWave(int waveSize = 512)
    {
        std::vector<float> wave(waveSize);
        for (int i = 0; i < waveSize; ++i)
        {
            wave[i] = (2.0f * i / waveSize) - 1.0f; // Linear ramp from -1 to 1
        }
        return wave;
    }

    std::vector<float> getWaveform() const
    {
        return waveform; // Return the current waveform samples
    }

    private:
    std::vector<float> waveform;   // Vector to hold the waveform samples
    float phaseIndex;              // Current index in the waveform
    float phaseIncrement;          // Increment value for phase index
    size_t waveformSize;           // Size of the waveform
};

class CustomSynthVoice : public juce::SynthesiserVoice
{
    public:
    CustomSynthVoice()
    {
        setWaveform(Wavetable::createSineWave());
        DBG("CustomSynthVoice initialized.");
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void setWaveform(const std::vector<float>& newWaveform)
    {
        wavetable.setWaveform(newWaveform);
        DBG("Waveform set with size: " << newWaveform.size());
    }

    void setCurrentPlaybackSampleRate(double sampleRate) override
    {
        oscillatorSampleRate = sampleRate;
        DBG("Preparing to play at sample rate: " << sampleRate);
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        auto frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

        if (oscillatorSampleRate > 0.0)  // Prevent division by zero
        {
            wavetable.setFrequency(frequency, oscillatorSampleRate); // Set frequency for wavetable
            phaseIncrement = frequency / oscillatorSampleRate; // Calculate phase increment
        }

        amplitude = velocity;

        // Trigger the amplitude ADSR envelope
        amplitudeEnvelope.noteOn();

        DBG("Starting note: " << midiNoteNumber << ", Frequency: " << frequency << ", Amplitude: " << amplitude);  // Log details
    }

    void stopNote(float velocity, bool allowTailOff) override
    {
        // Trigger note release for amplitude ADSR envelope
        amplitudeEnvelope.noteOff();

        if (!allowTailOff || !amplitudeEnvelope.isActive())
        {
            clearCurrentNote();
            amplitude = 0.0f; // Reset amplitude
        }

        DBG("Note stopped, amplitude set to zero.");  // Debugging statement
    }

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        juce::ScopedNoDenormals noDenormals;

        if (!amplitudeEnvelope.isActive())  // Skip processing if the amplitude envelope is not active
            return;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Fetch the current sample from the wavetable
            auto wavetableSample = wavetable.getNextSample(); // Get sample from wavetable
            auto envelopeValue = amplitudeEnvelope.getNextSample();  // Get current amplitude envelope value

            // Modulate wavetable sample by amplitude envelope
            auto finalSample = wavetableSample * amplitude * envelopeValue;

            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            {
                // Accumulate the output instead of overwriting it (supports polyphony)
                outputBuffer.addSample(channel, startSample + sample, finalSample);
            }

            // Increment phase index for next sample
            phaseIndex += phaseIncrement;
            if (phaseIndex >= 1.0f)
                phaseIndex -= 1.0f; // Wrap phase index
        }

    }

    bool canPlaySound(juce::SynthesiserSound*) override
    {
        return true; // Allow any sound
    }

    void setAmplitudeADSRParameters(const juce::ADSR::Parameters& params)
    {
        amplitudeEnvelope.setParameters(params); // Update amplitude ADSR parameters
    }

    // Add a new ADSR envelope for other uses (like filter cutoff)
    void addEnvelope(const ADSREnvelope& newEnvelope)
    {
        envelopes.push_back(newEnvelope);
    }

    Wavetable* getWavetable() { return &wavetable; } // Expose Wavetable for external use

    private:
    Wavetable wavetable;
    float oscillatorSampleRate = 0.0f;
    float amplitude = 0.0f;
    float phaseIncrement = 0.0f; // Phase increment for wavetable
    float phaseIndex = 0.0f; // Phase index for wavetable

    // Amplitude ADSR (dedicated for amplitude modulation)
    ADSREnvelope amplitudeEnvelope;

    // Collection of all envelopes, including any dynamically assigned ones
    std::vector<ADSREnvelope> envelopes;

    // Helper to check if any envelope is still active
    bool isAnyEnvelopeActive() const
    {
        for (const auto& envelope : envelopes)
        {
            if (envelope.isActive())
                return true;
        }
        return false;
    }
};

// CustomSynthSound Class
class CustomSynthSound : public juce::SynthesiserSound
{
    public:
    bool appliesToNote (int) override { return true; }  // Can play any MIDI note
    bool appliesToChannel (int) override { return true; }  // Can play on any MIDI channel
};

// MultiOutSynth Class
class MultiOutSynth : public juce::AudioProcessor
{
    public:
    enum
    {
        maxNumberOfVoices = 5
    };

    MultiOutSynth()
    : AudioProcessor(BusesProperties()
                     .withOutput("Output #1", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output #2", juce::AudioChannelSet::stereo(), false))
    {
        for (int i = 0; i < maxNumberOfVoices; ++i)
            synth.addVoice(new CustomSynthVoice());

        synth.addSound(new CustomSynthSound());

        //Initialize ADSR parameters with default values
        adsrParams.attack = 0.1f;
        adsrParams.decay = 0.1f;
        adsrParams.sustain = 0.3f;
        adsrParams.release = 0.2f;
    }

    ~MultiOutSynth() override {}

    const juce::String getName() const override { return "SexSynth"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override { juce::ignoreUnused(index); }
    const juce::String getProgramName(int index) override { return "Default"; }
    void changeProgramName(int index, const juce::String& newName) override { juce::ignoreUnused(index, newName); }

    // Create the editor
    juce::AudioProcessorEditor* createEditor() override
    {
        return new SynthEditor(*this);
    }

    // Indicate that the processor has an editor
    bool hasEditor() const override{ return true; }

    void getStateInformation(juce::MemoryBlock& destData) override
    {
        // Save your parameters here (e.g., ADSR settings)
        // Serialize to destData
    }

    void setStateInformation(const void* data, int sizeInBytes) override
    {
        // Load your parameters here
        // Deserialize from data
    }

    void setADSRParameter(const juce::String& parameter, float value)
    {
        juce::ADSR::Parameters params; // Create a temporary parameters object

        // Update the specific parameter value
        if (parameter == "attack")
            adsrParams.attack = value;
        else if (parameter == "decay")
            adsrParams.decay = value;
        else if (parameter == "sustain")
            adsrParams.sustain = value;
        else if (parameter == "release")
            adsrParams.release = value;

        // Update ADSR parameters for each voice using the ADSREnvelope class
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* customVoice = dynamic_cast<CustomSynthVoice*>(synth.getVoice(i)))
            {
                customVoice->setAmplitudeADSRParameters(adsrParams); // Update ADSR parameters for each voice
            }
        }
    }

    // Example ADSR getter methods
    float getADSRParameter(const std::string& parameterName)
    {
        if (parameterName == "attack") return adsrParams.attack; // Replace with your actual ADSR members
        if (parameterName == "decay") return adsrParams.decay;
        if (parameterName == "sustain") return adsrParams.sustain;
        if (parameterName == "release") return adsrParams.release;
        return 0.0f; // or throw an error if parameter not found
    }


    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::ignoreUnused(samplesPerBlock);
        synth.setCurrentPlaybackSampleRate(sampleRate);

        // Set up the reese filter coefficients
        reeseFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, reeseFrequency));

        // Prepare the cutoff filter with the current sample rate
        cutoffFilter.prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());

        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<CustomSynthVoice*>(synth.getVoice(i)))
            {
                voice->setCurrentPlaybackSampleRate(sampleRate);  // Prepare each voice
            }
        }
    }

    void setCutoffFrequency(float frequency)
    {
        cutoffFilter.setCutoffFrequency(frequency);
    }

    void setReeseParameter(float frequency)
    {
        reeseFrequency = frequency;
        // Update the filter coefficients
        if (getSampleRate() > 0)
            reeseFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), reeseFrequency));
    }

    bool isReeseEnabled = false;

    void setReeseEnabled(bool isEnabled)
    {
        isReeseEnabled = isEnabled;
    }

    void setVolume(float newVolume)
    {
        volume = newVolume;
    }

    void releaseResources() override {}

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        juce::ScopedNoDenormals noDenormals;

        // Clear buffer for new audio processing
        buffer.clear();

        // Render audio from synthesizer with incoming MIDI messages
        synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

        // Apply the Reese filter if enabled
        if (isReeseEnabled)
        {
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                auto* channelData = buffer.getWritePointer(channel); // Get channel audio data
                reeseFilter.processSamples(channelData, buffer.getNumSamples()); // Process with Reese filter
            }
        }

        // Apply the cutoff filter to all channels (if relevant)
        cutoffFilter.processBlock(buffer); // Assuming this is handled inside the CutoffFilter class

        // Additional effects (e.g., volume adjustment)
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel); // Get the audio data for each channel
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                channelData[sample] *= volume; // Apply volume adjustment to each sample
            }
        }
        // Store the mixed waveform data for visualisation
        updateCurrentWaveformData(buffer);
    }

    // New getter for waveform data
    std::vector<float> getCurrentWaveformData() const
    {
        return currentWaveformData;
    }

    // Provide a method to access the synth for waveform manipulation
    juce::Synthesiser& getSynth() { return synth; }

    void setWaveform(int waveformType)
    {
        std::vector<float> waveform;

        // Define waveforms
        if (waveformType == 1) // Sine
            waveform = Wavetable::createSineWave();
        else if (waveformType == 2) // Saw
            waveform = Wavetable::createSawtoothWave();
        else if (waveformType == 3) // Square
            waveform = Wavetable::createSquareWave();

        // Set the waveform for each voice
        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* customVoice = dynamic_cast<CustomSynthVoice*>(synth.getVoice(i)))
            {
                customVoice->setWaveform(waveform);
            }
        }
    }

    void updateCurrentWaveformData(const juce::AudioBuffer<float>& buffer)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();

        currentWaveformData.clear();
        currentWaveformData.resize(numSamples, 0.0f);  // Resize the vector to match buffer size

        // Combine all channels into a single mixed waveform (if stereo/mono synth)
        for (int channel = 0; channel < numChannels; ++channel)
        {
            const float* channelData = buffer.getReadPointer(channel);
            for (int sample = 0; sample < numSamples; ++sample)
            {
                currentWaveformData[sample] += channelData[sample];
            }
        }

        // Normalize the waveform to avoid clipping
        float maxVal = *std::max_element(currentWaveformData.begin(), currentWaveformData.end(),
                                         [](float a, float b) { return std::abs(a) < std::abs(b); });
        if (maxVal > 0.0f)
        {
            for (auto& sample : currentWaveformData)
            {
                sample /= maxVal;
            }
        }
    }

    private:
    std::vector<float> currentWaveformData; // to hold the current audio waveform data
    juce::Synthesiser synth;
    juce::ADSR::Parameters adsrParams;
    CutoffFilter cutoffFilter;

    juce::IIRFilter reeseFilter; // Low-pass filter for reese
    float reeseFrequency = 2000.0f; // Default frequency reese

    float volume = 1.0f;  // Default volume is full (1.0)

    // Your ADSR parameters
    float attack = 0.1f;   // Default values
    float decay = 0.1f;
    float sustain = 0.5f;
    float release = 0.1f;

    #include "SynthEditor.h"
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiOutSynth)
};
