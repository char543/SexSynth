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

//#pragma once
//
//#include <JuceHeader.h>
//#include "Cutoff.h"
////#include "SynthEditor.h" // Include the SynthEditor header
//class SynthEditor; // forward declaration
//// Wavetable Class
//class Wavetable
//{
//public:
//    Wavetable() : phaseIndex(0.0f), phaseIncrement(0.0f), waveformSize(0) {}
//
//    // Set the waveform and reset phase parameters
//    void setWaveform(const std::vector<float>& newWaveform)
//    {
//        waveform = newWaveform;
//        waveformSize = waveform.size(); // Store the size of the waveform
//        phaseIndex = 0.0f; // Reset phase index
//        phaseIncrement = 0.0f; // Reset phase increment
//        std::cout << "Waveform size set to: " << waveformSize << std::endl; // Log waveform size
//    }
//
//    // Set the frequency and calculate the phase increment
//    void setFrequency(float frequency, double sampleRate)
//    {
//        if (waveformSize > 0 && sampleRate > 0.0) // Ensure valid sample rate
//        {
//            phaseIncrement = (frequency / sampleRate) * waveformSize; // Updated to control frequency
//        }
//        else
//        {
//            phaseIncrement = 0.0f; // Prevent phase increment from being non-zero if sampleRate is invalid
//        }
//    }
//
//    // Retrieve the next sample from the waveform
//    float getNextSample()
//    {
//        // Ensure we wrap phaseIndex correctly
//        int sampleIndex = static_cast<int>(phaseIndex) % waveformSize; // Wrap phase index within bounds
//
//        // Increment phase index
//        phaseIndex += phaseIncrement;
//
//        // Wrap around if needed
//        if (phaseIndex >= waveformSize)
//        {
//            phaseIndex -= waveformSize; // Reset to the beginning
//        }
//
//        return waveform[sampleIndex]; // Return the current sample from the waveform
//    }
//
//    // Return the size of the waveform
//    size_t getWaveformSize() const
//    {
//        return waveformSize; // Return the size of the waveform
//    }
//
//    // Create a sine wave
//    static std::vector<float> createSineWave(int waveSize = 512)
//    {
//        std::vector<float> wave(waveSize);
//        for (int i = 0; i < waveSize; ++i)
//        {
//            wave[i] = std::sin((2.0 * M_PI * i) / waveSize);
//        }
//        return wave;
//    }
//
//    // Create a square wave
//    static std::vector<float> createSquareWave(int waveSize = 512)
//    {
//        std::vector<float> wave(waveSize);
//        for (int i = 0; i < waveSize; ++i)
//        {
//            wave[i] = (i < waveSize / 2) ? 1.0f : -1.0f; // High for first half, low for second half
//        }
//        return wave;
//    }
//
//    // Create a sawtooth wave
//    static std::vector<float> createSawtoothWave(int waveSize = 512)
//    {
//        std::vector<float> wave(waveSize);
//        for (int i = 0; i < waveSize; ++i)
//        {
//            wave[i] = (2.0f * i / waveSize) - 1.0f; // Linear ramp from -1 to 1
//        }
//        return wave;
//    }
//
//    std::vector<float> getWaveform() const
//    {
//        return waveform; // Return the current waveform samples
//    }
//
//private:
//    std::vector<float> waveform;   // Vector to hold the waveform samples
//    float phaseIndex;              // Current index in the waveform
//    float phaseIncrement;          // Increment value for phase index
//    size_t waveformSize;           // Size of the waveform
//};
//
//
//// CustomSynthVoice Class
//class CustomSynthVoice : public juce::SynthesiserVoice
//{
//public:
//    CustomSynthVoice()
//    {
//        setWaveform(createSineWave());
//        adsrParams.attack = 0.01f;   // Default values
//        adsrParams.decay = 0.01f;
//        adsrParams.sustain = 0.1f;
//        adsrParams.release = 0.01f;
//        adsr.setParameters(adsrParams);
//
//        DBG("CustomSynthVoice initialized.");  // Debugging statement
//    }
//
//    void pitchWheelMoved(int) override {}
//    void controllerMoved(int, int) override {}
//
//    void setWaveform(const std::vector<float>& newWaveform)
//    {
//        wavetable.setWaveform(newWaveform);
//        DBG("Waveform set with size: " << newWaveform.size());  // Debugging statement
//    }
//
//    void setCurrentPlaybackSampleRate(double sampleRate) override
//    {
//        oscillatorSampleRate = sampleRate;
//        DBG("Preparing to play at sample rate: " << sampleRate);  // Debugging statement
//    }
//
//    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
//    {
//        auto frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
//
//        if (oscillatorSampleRate > 0.0)  // Prevent division by zero
//        {
//            wavetable.setFrequency(frequency, oscillatorSampleRate); // Set frequency for wavetable
//            phaseIncrement = frequency * wavetable.getWaveformSize() / oscillatorSampleRate; // Not needed anymore, remove this line
//        }
//
//        amplitude = velocity;
//        adsr.noteOn();  // Trigger the ADSR envelope
//
//        DBG("Starting note: " << midiNoteNumber << ", Frequency: " << frequency << ", Amplitude: " << amplitude);  // Log details
//    }
//
//
//    void stopNote(float velocity, bool allowTailOff) override
//    {
//        adsr.noteOff();  // Trigger note release
//        if (!allowTailOff || !adsr.isActive())  // Stop if no tail-off or envelope is done
//        {
//            clearCurrentNote();
//            amplitude = 0.0f;  // Reset amplitude
//        }
//
//        DBG("Note stopped, amplitude set to zero.");  // Debugging statement
//    }
//
//    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
//    {
//        juce::ScopedNoDenormals noDenormals;
//
//        if (!adsr.isActive())  // Skip processing if envelope is not active
//            return;
//
//        for (int sample = 0; sample < numSamples; ++sample)
//        {
//            // Ensure the current sample is fetched from the wavetable based on the phase index
//            auto wavetableSample = wavetable.getNextSample(); // Get sample from wavetable at current phase index
//            auto envelopeValue = adsr.getNextSample();  // Get current envelope value
//            auto finalSample = wavetableSample * amplitude * envelopeValue;  // Apply envelope
//
//            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
//            {
//                outputBuffer.setSample(channel, startSample + sample, finalSample);
//            }
//
//            // Update phase index for wavetable
//            phaseIndex += phaseIncrement;
//            if (phaseIndex >= wavetable.getWaveformSize())
//                phaseIndex -= wavetable.getWaveformSize(); // Wrap phase index
//        }
//    }
//
//    bool canPlaySound(juce::SynthesiserSound*) override
//    {
//        return true; // Allow any sound
//    }
//
//    void setADSRParameters(const juce::ADSR::Parameters& params)
//    {
//        adsrParams = params; // Store the ADSR parameters
//        adsr.setParameters(adsrParams); // Update ADSR with new parameters
//    }
//
//    Wavetable* getWavetable() { return &wavetable; } // Add this method to expose Wavetable
//
//
//private:
//    Wavetable wavetable;
//    float phaseIncrement = 0.0f;
//    float phaseIndex = 0.0f; // Phase index for wavetable
//    float oscillatorSampleRate = 0.0f;
//    float amplitude = 0.0f;
//    juce::ADSR adsr;  // Envelope generator
//    juce::ADSR::Parameters adsrParams;
//
//    std::vector<float> createSineWave()
//    {
//        const int waveSize = 512;  // Size of the sine wave
//        std::vector<float> wave(waveSize);
//        for (int i = 0; i < waveSize; ++i)
//        {
//            wave[i] = std::sin((2.0 * M_PI * i) / waveSize);
//        }
//        return wave;
//    }
//};
//
//
//
//// CustomSynthSound Class
//class CustomSynthSound : public juce::SynthesiserSound
//{
//public:
//    bool appliesToNote (int) override { return true; }  // Can play any MIDI note
//    bool appliesToChannel (int) override { return true; }  // Can play on any MIDI channel
//};
//
//// MultiOutSynth Class
//class MultiOutSynth : public juce::AudioProcessor
//{
//public:
//    enum
//    {
//        maxNumberOfVoices = 5
//    };
//
//    MultiOutSynth()
//        : AudioProcessor(BusesProperties()
//                         .withOutput("Output #1", juce::AudioChannelSet::stereo(), true)
//                         .withOutput("Output #2", juce::AudioChannelSet::stereo(), false))
//    {
//        for (int i = 0; i < maxNumberOfVoices; ++i)
//            synth.addVoice(new CustomSynthVoice());
//
//        synth.addSound(new CustomSynthSound());
//
//        // Initialize ADSR parameters
//        adsrParams.attack = 0.1f;
//        adsrParams.decay = 0.1f;
//        adsrParams.sustain = 0.5f;
//        adsrParams.release = 0.1f;
//    }
//
//    ~MultiOutSynth() override {}
//
//    const juce::String getName() const override { return "MultiOutSynth"; }
//    bool acceptsMidi() const override { return true; }
//    bool producesMidi() const override { return true; }
//    bool isMidiEffect() const override { return false; }
//    double getTailLengthSeconds() const override { return 0.0; }
//
//    int getNumPrograms() override { return 1; }
//    int getCurrentProgram() override { return 0; }
//    void setCurrentProgram(int index) override { juce::ignoreUnused(index); }
//    const juce::String getProgramName(int index) override { return "Default"; }
//    void changeProgramName(int index, const juce::String& newName) override { juce::ignoreUnused(index, newName); }
//
//    void prepareToPlay(double newSampleRate, int samplesPerBlock) override
//    {
//        juce::ignoreUnused(samplesPerBlock);
//        synth.setCurrentPlaybackSampleRate(newSampleRate);
//
//        // Set up the reese filter coefficients
//        reeseFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(newSampleRate, reeseFrequency));
//
//        // Prepare the cutoff filter with the current sample rate
//        cutoffFilter.prepareToPlay(newSampleRate, samplesPerBlock, getTotalNumOutputChannels());
//
//    }
//
//    void setCutoffFrequency(float frequency)
//    {
//        cutoffFilter.setCutoffFrequency(frequency);
//    }
//
//    void setReeseParameter(float frequency)
//    {
//        reeseFrequency = frequency;
//        // Update the filter coefficients
//        if (getSampleRate() > 0)
//            reeseFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), reeseFrequency));
//    }
//
//    bool isReeseEnabled = false;
//
//    void setReeseEnabled(bool isEnabled)
//    {
//        isReeseEnabled = isEnabled;
//    }
//
//    void setVolume(float newVolume)
//    {
//        volume = newVolume;
//    }
//
//    void releaseResources() override {}
//
//    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiBuffer) override
//    {
//        buffer.clear();  // Clear buffer before rendering audio
//        synth.renderNextBlock(buffer, midiBuffer, 0, buffer.getNumSamples());
//
//        if (isReeseEnabled)
//        {
//            // Apply the reese filter
//            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
//            {
//                auto* channelData = buffer.getWritePointer(channel);
//                reeseFilter.processSamples(channelData, buffer.getNumSamples());
//            }
//        }
//
//        cutoffFilter.processBlock(buffer);
//
//        // Apply volume control
//        buffer.applyGain(volume);
//    }
//
//
//    juce::AudioProcessorEditor* createEditor() override { return new SynthEditor(*this); }
//    bool hasEditor() const override { return true; }
//
//    void getStateInformation(juce::MemoryBlock& destData) override
//    {
//        // Save your parameters here (e.g., ADSR settings)
//        // Serialize to destData
//    }
//
//    void setStateInformation(const void* data, int sizeInBytes) override
//    {
//        // Load your parameters here
//        // Deserialize from data
//    }
//
//    void setADSRParameter(const juce::String& parameter, float value)
//    {
//        if (parameter == "attack")
//            adsrParams.attack = value;
//        else if (parameter == "decay")
//            adsrParams.decay = value;
//        else if (parameter == "sustain")
//            adsrParams.sustain = value;
//        else if (parameter == "release")
//            adsrParams.release = value;
//
//        // Update ADSR parameters for each voice
//        for (int i = 0; i < synth.getNumVoices(); ++i)
//        {
//            if (auto* customVoice = dynamic_cast<CustomSynthVoice*>(synth.getVoice(i)))
//            {
//                customVoice->setADSRParameters(adsrParams); // Update ADSR parameters for each voice
//            }
//        }
//    }
//
//    std::vector<float> getWaveformData() const
//    {
//        // Assuming you want to access the first voice's wavetable
//        if (auto* voice = dynamic_cast<CustomSynthVoice*>(synth.getVoice(0)))
//        {
//            return voice->getWavetable()->getWaveform(); // Assuming getWavetable() returns Wavetable*
//        }
//        return {};
//    }
//
//    void setWaveform(int waveformType)
//    {
//        std::vector<float> waveform;
//
//        // Define waveforms
//        if (waveformType == 1) // Sine
//            waveform = createSineWave();
//        else if (waveformType == 2) // Saw
//            waveform = createSawWave();
//        else if (waveformType == 3) // Square
//            waveform = createSquareWave();
//
//        // Set the waveform for each voice
//        for (int i = 0; i < synth.getNumVoices(); ++i)
//        {
//            if (auto* customVoice = dynamic_cast<CustomSynthVoice*>(synth.getVoice(i)))
//            {
//                customVoice->setWaveform(waveform);
//            }
//        }
//    }
//
//
//private:
//    juce::Synthesiser synth;
//    juce::ADSR::Parameters adsrParams;
//
//    CutoffFilter cutoffFilter;
//
//    juce::IIRFilter reeseFilter; // Low-pass filter for reese
//    float reeseFrequency = 2000.0f; // Default frequency reese
//
//    float volume = 1.0f;  // Default volume is full (1.0)
//
//    std::vector<float> createSineWave()
//    {
//        const int waveSize = 512;
//        std::vector<float> wave(waveSize);
//        for (int i = 0; i < waveSize; ++i)
//        {
//            wave[i] = std::sin((2.0 * M_PI * i) / waveSize);
//        }
//        return wave;
//    }
//
//    std::vector<float> createSawWave()
//    {
//        const int waveSize = 512;
//        std::vector<float> wave(waveSize);
//        for (int i = 0; i < waveSize; ++i)
//        {
//            wave[i] = (2.0f * i / waveSize) - 1.0f; // Linear ramp from -1 to 1
//        }
//        return wave;
//    }
//
//    std::vector<float> createSquareWave()
//    {
//        const int waveSize = 512;
//        std::vector<float> wave(waveSize);
//        for (int i = 0; i < waveSize; ++i)
//        {
//            wave[i] = (i < waveSize / 2) ? 1.0f : -1.0f; // Square wave
//        }
//        return wave;
//    }
//#include "SynthEditor.h"
//    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiOutSynth)
//};
/////////
///////end working h file

#pragma once

#include <JuceHeader.h>
#include "Cutoff.h"
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


// CustomSynthVoice Class
class CustomSynthVoice : public juce::SynthesiserVoice
{
public:
    CustomSynthVoice()
    {
        setWaveform(Wavetable::createSineWave());
        adsrParams.attack = 0.01f;   // Default values
        adsrParams.decay = 0.01f;
        adsrParams.sustain = 0.1f;
        adsrParams.release = 0.01f;
        adsr.setParameters(adsrParams);

        DBG("CustomSynthVoice initialized.");  // Debugging statement
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void setWaveform(const std::vector<float>& newWaveform)
    {
        wavetable.setWaveform(newWaveform);
        DBG("Waveform set with size: " << newWaveform.size());  // Debugging statement
    }

    void setCurrentPlaybackSampleRate(double sampleRate) override
    {
        oscillatorSampleRate = sampleRate;
        DBG("Preparing to play at sample rate: " << sampleRate);  // Debugging statement
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        auto frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        
        if (oscillatorSampleRate > 0.0)  // Prevent division by zero
        {
            wavetable.setFrequency(frequency, oscillatorSampleRate); // Set frequency for wavetable
        }

        amplitude = velocity;
        adsr.noteOn();  // Trigger the ADSR envelope

        DBG("Starting note: " << midiNoteNumber << ", Frequency: " << frequency << ", Amplitude: " << amplitude);  // Log details
    }

    void stopNote(float velocity, bool allowTailOff) override
    {
        adsr.noteOff();  // Trigger note release
        if (!allowTailOff || !adsr.isActive())  // Stop if no tail-off or envelope is done
        {
            clearCurrentNote();
            amplitude = 0.0f;  // Reset amplitude
        }

        DBG("Note stopped, amplitude set to zero.");  // Debugging statement
    }

    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        juce::ScopedNoDenormals noDenormals;

        if (!adsr.isActive())  // Skip processing if the envelope is not active
            return;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Fetch the current sample from the wavetable and apply the ADSR envelope
            auto wavetableSample = wavetable.getNextSample(); // Get sample from wavetable
            auto envelopeValue = adsr.getNextSample();  // Get current ADSR envelope value
            auto finalSample = wavetableSample * amplitude * envelopeValue;  // Apply amplitude and ADSR envelope

            for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            {
                // Accumulate the output instead of overwriting it (supports polyphony)
                outputBuffer.addSample(channel, startSample + sample, finalSample);
            }
        }
    }

    bool canPlaySound(juce::SynthesiserSound*) override
    {
        return true; // Allow any sound
    }

    void setADSRParameters(const juce::ADSR::Parameters& params)
    {
        adsrParams = params; // Store the ADSR parameters
        adsr.setParameters(adsrParams); // Update ADSR with new parameters
    }
    
    Wavetable* getWavetable() { return &wavetable; } // Add this method to expose Wavetable
    
private:
    Wavetable wavetable;
    float oscillatorSampleRate = 0.0f;
    float amplitude = 0.0f;
    float phaseIncrement = 0.0f;
    float phaseIndex = 0.0f; // Phase index for wavetable
    juce::ADSR adsr;  // Envelope generator
    juce::ADSR::Parameters adsrParams;
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

        // Initialize ADSR parameters
        adsrParams.attack = 0.1f;
        adsrParams.decay = 0.1f;
        adsrParams.sustain = 0.3f;
        adsrParams.release = 0.1f;
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
    bool hasEditor() const override
    {
        return true;
    }
    
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
            if (parameter == "attack")
                adsrParams.attack = value;
            else if (parameter == "decay")
                adsrParams.decay = value;
            else if (parameter == "sustain")
                adsrParams.sustain = value;
            else if (parameter == "release")
                adsrParams.release = value;
            
            // Update ADSR parameters for each voice
            for (int i = 0; i < synth.getNumVoices(); ++i)
            {
                if (auto* customVoice = dynamic_cast<CustomSynthVoice*>(synth.getVoice(i)))
                {
                    customVoice->setADSRParameters(adsrParams); // Update ADSR parameters for each voice
                }
            }
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
    
    std::vector<float> getWaveformData() const
    {
        std::vector<float> mixedWaveform; // Vector to store the mixed waveform
        const int bufferSize = 512; // Define a buffer size (can be adjusted)

        // Initialize the mixed waveform with zeros
        mixedWaveform.resize(bufferSize, 0.0f);

        for (int i = 0; i < synth.getNumVoices(); ++i)
        {
            if (auto* voice = dynamic_cast<CustomSynthVoice*>(synth.getVoice(i)))
            {
                // Get the waveform from the voice
                const auto& voiceWaveform = voice->getWavetable()->getWaveform();
                
                // Mix this voice's waveform into the mixedWaveform
                for (int j = 0; j < std::min(bufferSize, static_cast<int>(voiceWaveform.size())); ++j)
                {
                    mixedWaveform[j] += voiceWaveform[j]; // Sum the waveforms
                }
            }
        }

        // Normalize mixedWaveform to prevent clipping
        float maxVal = *std::max_element(mixedWaveform.begin(), mixedWaveform.end());
        if (maxVal > 0.0f)
        {
            for (auto& sample : mixedWaveform)
            {
                sample /= maxVal; // Normalize to the range [-1.0, 1.0]
            }
        }

        return mixedWaveform; // Return the mixed waveform
    }

private:
    juce::Synthesiser synth;
    juce::ADSR::Parameters adsrParams;
    CutoffFilter cutoffFilter;
    
    juce::IIRFilter reeseFilter; // Low-pass filter for reese
    float reeseFrequency = 2000.0f; // Default frequency reese
    
    float volume = 1.0f;  // Default volume is full (1.0)
    
#include "SynthEditor.h"
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiOutSynth)
};
