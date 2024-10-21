//
//  SynthEditor.h
//  SexSynth
//
//  Created by retrack on 16/10/2024.
//  Copyright © 2024 Bojo Corp. All rights reserved.
//

#pragma once

#include "SexSynth.h"
#include <JuceHeader.h>
#include "WaveformDisplay.h"

class SynthEditor : public juce::AudioProcessorEditor,
public juce::Slider::Listener,
public juce::ComboBox::Listener,
private juce::Timer
{
public:
    SynthEditor(MultiOutSynth& p)
    : AudioProcessorEditor(&p), processor(p)
    {
        // Load the background image (assuming "background.png" was added to Projucer)
        backgroundImage = juce::ImageCache::getFromMemory(BinaryData::squat_jpg, BinaryData::squat_jpgSize);

        // Initialize UI components
        initializeSliders();
        initializeComboBox();
        initializeWaveformDisplay();

        // Add update interval slider
        addAndMakeVisible(updateIntervalSlider);
        configureUpdateIntervalSlider();

        // Add zoom slider
        addAndMakeVisible(zoomSlider);
        configureZoomSlider();

        // Start the timer to update the waveform display
        //
        //i dont know what this should be, if it needs to be set lower to be able to go lower or if its just the default value
        startTimer(1);

        // then call this func
        initializeADSRSliders();

        // Set the size of the editor window
        setSize(900, 700);
    }

    ~SynthEditor() override {}

    void timerCallback() override
    {
        // Fetch the latest waveform data from your audio processor
        auto waveformData = processor.getCurrentWaveformData();
        waveformDisplay.setWaveform(waveformData);

        // Update ADSR sliders from processor values (in case they changed externally, for example from host automation etc.)
        attackSlider.setValue(processor.getADSRParameter("attack"), juce::dontSendNotification);
        decaySlider.setValue(processor.getADSRParameter("decay"), juce::dontSendNotification);
        sustainSlider.setValue(processor.getADSRParameter("sustain"), juce::dontSendNotification);
        releaseSlider.setValue(processor.getADSRParameter("release"), juce::dontSendNotification);
    }

    void paint(juce::Graphics& g) override
    {
        if (backgroundImage.isValid())
        {
            // Draw the background image to fill the entire editor
        //    g.drawImage(backgroundImage, getLocalBounds().toFloat());
           // g.fillAll(juce::Colours::darkgrey);
        }
        else
        {
            // If there's no image, fill with a default color
            g.fillAll(juce::Colours::darkgrey);
        }
    }

    void resized() override
    {
        // setBounds : (x,y,width,height)
        // x pixels from left edge, y pixels from top
        // int's for width and height of component

        // Set bounds for ADSR sliders
        attackSlider.setBounds(10, 30, 70, 200);
        decaySlider.setBounds(100, 30, 70, 200);
        sustainSlider.setBounds(190, 30, 70, 200);
        releaseSlider.setBounds(280, 30, 70, 200);

        // Set bounds for labels
        attackLabel.setBounds(10, 230, 70, 20);
        decayLabel.setBounds(100, 230, 70, 20);
        sustainLabel.setBounds(190, 230, 70, 20);
        releaseLabel.setBounds(280, 230, 70, 20);

        // Waveform selector and label
        waveformSelector.setBounds(10, 260, 100, 20);
        currentWaveformLabel.setBounds(120, 260, 200, 20);

        // Adjust the waveform display size
        waveformDisplay.setBounds(10, 290, 400, 100);  // Reduce height

        // Set bounds for reese controls
        reeseSlider.setBounds(10, 450, 400, 40);
        reeseLabel.setBounds(10, 400, 120, 20);
        reeseToggleButton.setBounds(10, 480, 100, 20);

        // Volume knob
        volumeKnob.setBounds(450, 400, 80, 80);
        volumeLabel.setBounds(450, 480, 80, 20);

        // Cutoff slider
        cutoffSlider.setBounds(450, 50, 150, 50);
        cutoffLabel.setBounds(450, 30, 100, 20);

        // Position the update interval slider
        updateIntervalSlider.setBounds(420, 340, 210, 30);

        // Position the zoom slider
        zoomSlider.setBounds(420, 300, 210, 30);  // Adjust the position and size as needed
    }

    void sliderValueChanged(juce::Slider* slider) override
    {
        if (slider == &attackSlider)
            processor.setADSRParameter("attack", attackSlider.getValue());
        else if (slider == &decaySlider)
            processor.setADSRParameter("decay", decaySlider.getValue());
        else if (slider == &sustainSlider)
            processor.setADSRParameter("sustain", sustainSlider.getValue());
        else if (slider == &releaseSlider)
            processor.setADSRParameter("release", releaseSlider.getValue());
        else if (slider == &reeseSlider)
            processor.setReeseParameter(reeseSlider.getValue());
        else if (slider == &volumeKnob)
            processor.setVolume(volumeKnob.getValue());
        else if (slider == &cutoffSlider)
            processor.setCutoffFrequency(cutoffSlider.getValue());
        else if (slider == &updateIntervalSlider)
            setUpdateInterval(static_cast<int>(updateIntervalSlider.getValue()));  // For the update interval
        else if (slider == &zoomSlider)
            waveformDisplay.setZoomLevel(zoomSlider.getValue());  // For zooming the waveform
    }

    void comboBoxChanged(juce::ComboBox* comboBox) override
    {
        if (comboBox == &waveformSelector)
        {
            int waveformType = waveformSelector.getSelectedId();
            processor.setWaveform(waveformType); // Update the waveform in the processor

            switch (waveformType)
            {
                case 1: currentWaveformLabel.setText("Current Waveform: Sine", juce::dontSendNotification); break;
                case 2: currentWaveformLabel.setText("Current Waveform: Saw", juce::dontSendNotification); break;
                case 3: currentWaveformLabel.setText("Current Waveform: Square", juce::dontSendNotification); break;
            }
        }
    }

    // Call this method to update the waveform display from audio buffer data
    void updateWaveformDisplay(const std::vector<float>& newWaveformData)
    {
        waveformDisplay.setWaveform(newWaveformData);
    }

private:
    MultiOutSynth& processor;

    // Private method to initialize ADSR sliders with processor values
    void initializeADSRSliders()
    {
        attackSlider.setValue(processor.getADSRParameter("attack"), juce::dontSendNotification);
        decaySlider.setValue(processor.getADSRParameter("decay"), juce::dontSendNotification);
        sustainSlider.setValue(processor.getADSRParameter("sustain"), juce::dontSendNotification);
        releaseSlider.setValue(processor.getADSRParameter("release"), juce::dontSendNotification);
    }


    // Declare UI components
    juce::Slider attackSlider;
    juce::Label attackLabel;

    juce::Slider decaySlider;
    juce::Label decayLabel;

    juce::Slider sustainSlider;
    juce::Label sustainLabel;

    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    juce::ComboBox waveformSelector;

    juce::Label currentWaveformLabel;
    WaveformDisplay waveformDisplay;

    juce::Slider reeseSlider;
    juce::Label reeseLabel;

    juce::ToggleButton reeseToggleButton;

    juce::Slider volumeKnob;
    juce::Label volumeLabel;

    juce::Slider cutoffSlider;
    juce::Label cutoffLabel;

    juce::Image backgroundImage;

    juce::Slider updateIntervalSlider;  // Slider to control the update interval
    juce::Slider zoomSlider; // Slider for zooming waveform

    void initializeSliders()
    {
        addAndMakeVisible(attackSlider);
        attackSlider.setRange(0.01f, 5.0f, 0.001f);
        attackSlider.setSkewFactorFromMidPoint(1.0f);
        attackSlider.setSliderStyle(juce::Slider::LinearVertical);
        attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        attackSlider.addListener(this);
        attackLabel.setText("Attack", juce::dontSendNotification);
        addAndMakeVisible(attackLabel);

        addAndMakeVisible(decaySlider);
        decaySlider.setRange(0.01f, 5.0f, 0.001f);
        decaySlider.setSliderStyle(juce::Slider::LinearVertical);
        decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        decaySlider.addListener(this);
        decayLabel.setText("Decay", juce::dontSendNotification);
        addAndMakeVisible(decayLabel);

        addAndMakeVisible(sustainSlider);
        sustainSlider.setRange(0.0f, 1.0f, 0.001f);
        sustainSlider.setSliderStyle(juce::Slider::LinearVertical);
        sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        sustainSlider.addListener(this);
        sustainLabel.setText("Sustain", juce::dontSendNotification);
        addAndMakeVisible(sustainLabel);

        addAndMakeVisible(releaseSlider);
        releaseSlider.setRange(0.01f, 5.0f, 0.001f);
        releaseSlider.setSkewFactorFromMidPoint(1.0f);
        releaseSlider.setSliderStyle(juce::Slider::LinearVertical);
        releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        releaseSlider.addListener(this);
        releaseLabel.setText("Release", juce::dontSendNotification);
        addAndMakeVisible(releaseLabel);

        addAndMakeVisible(reeseSlider);
        reeseSlider.setRange(20.0f, 20000.0f); // Range from 20 Hz to 20 kHz
        reeseSlider.setValue(2000.0f); // Default value
        reeseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        reeseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        reeseSlider.addListener(this);
        reeseLabel.setText("Reese", juce::dontSendNotification);
        addAndMakeVisible(reeseLabel);

        addAndMakeVisible(reeseToggleButton);
        reeseToggleButton.setButtonText("Reese On/Off");
        reeseToggleButton.onClick = [this]() { processor.setReeseEnabled(reeseToggleButton.getToggleState()); };

        addAndMakeVisible(volumeKnob);
        volumeKnob.setRange(0.0f, 1.0f);  // Range from 0 (mute) to 1 (full volume)
        volumeKnob.setValue(1.0f);        // Default value (full volume)
        volumeKnob.setSliderStyle(juce::Slider::Rotary);  // Set as a knob
        volumeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20); // Textbox under the knob
        volumeKnob.addListener(this);     // Attach listener
        volumeLabel.setText("Volume", juce::dontSendNotification);  // Label for the knob
        addAndMakeVisible(volumeLabel);

        addAndMakeVisible(cutoffSlider);
        cutoffSlider.setRange(1.0f, 20000.0f, 1.0f); // Cutoff frequency range in Hz
        cutoffSlider.setValue(20000.0f);    // Default cutoff value
        cutoffSlider.setSliderStyle(juce::Slider::LinearHorizontal);
        cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        cutoffSlider.addListener(this);     // Attach listener to handle changes
        cutoffLabel.setText("Cutoff", juce::dontSendNotification);
        addAndMakeVisible(cutoffLabel);
    }

    void configureUpdateIntervalSlider()
    {
        updateIntervalSlider.setRange(1, 10000, 1);
        updateIntervalSlider.setValue(100); // Default value 100ms
        updateIntervalSlider.onValueChange = [this]()
        {
            setUpdateInterval(static_cast<int>(updateIntervalSlider.getValue()));
        };
        setUpdateInterval(updateIntervalSlider.getValue());
    }

    void configureZoomSlider()
    {
        zoomSlider.setRange(0.1, 50.0, 0.1); // Adjust range as needed
        zoomSlider.setValue(1.0); // Default zoom level
        zoomSlider.onValueChange = [this]()
        {
            // Set the zoom level based on the slider value
            float sliderValue = zoomSlider.getValue();
            // Set zoom level based on slider value
            waveformDisplay.setZoomLevel(sliderValue);
        };
    }

    void initializeComboBox()
    {
        waveformSelector.addItem("Sine", 1);
        waveformSelector.addItem("Saw", 2);
        waveformSelector.addItem("Square", 3);
        waveformSelector.setSelectedId(1);
        waveformSelector.addListener(this);
        addAndMakeVisible(waveformSelector);

        addAndMakeVisible(currentWaveformLabel);
        currentWaveformLabel.setText("Current Waveform: Sine", juce::dontSendNotification);

        // Define the font options with size and bold style
        juce::FontOptions fontOptions;
        fontOptions = fontOptions.withHeight(16.0f);  // Set font size to 16.0
        fontOptions = fontOptions.withStyle("Bold");   // Set bold style

        // Apply the font options to create the Font object
        juce::Font customFont(fontOptions);
        currentWaveformLabel.setFont(customFont); // Set the font to the label

        currentWaveformLabel.setJustificationType(juce::Justification::centred);
    }

    void initializeWaveformDisplay()
    {
        addAndMakeVisible(waveformDisplay);
        waveformDisplay.setWaveform(processor.getCurrentWaveformData());
        waveformDisplay.setWaveformColor(juce::Colours::green);
    }

    void setUpdateInterval(int intervalMs)
    {
        stopTimer();  // Stop the current timer
        startTimer(intervalMs);  // Start a new timer with the new interval
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthEditor)
};

