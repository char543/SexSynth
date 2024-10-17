//
//  SynthEditor.h
//  SexSynth
//
//  Created by retrack on 16/10/2024.
//  Copyright © 2024 Bojo Corp. All rights reserved.
//

#pragma once

#include "MultiOutSynthTutorial.h"
#include <JuceHeader.h>
#include "WaveformDisplay.h"
//class MultiOutSynth;

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
        startTimer(100);

        // Set the size of the editor window
        setSize(900, 700);
    }

    ~SynthEditor() override {}
    
    void timerCallback() override
    {
        waveformDisplay.setWaveform(processor.getWaveformData());
    }

    void paint(juce::Graphics& g) override
    {
        if (backgroundImage.isValid())
        {
            // Draw the background image to fill the entire editor
//          g.drawImage(backgroundImage, getLocalBounds().toFloat());
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
        
        // ADSR sliders
        attackSlider.setBounds(10, 30, 70, 200);
        attackLabel.setBounds(10, 230, 70, 20);

        decaySlider.setBounds(100, 30, 70, 200);
        decayLabel.setBounds(100, 230, 70, 20);

        sustainSlider.setBounds(190, 30, 70, 200);
        sustainLabel.setBounds(190, 230, 70, 20);

        releaseSlider.setBounds(280, 30, 70, 200);
        releaseLabel.setBounds(280, 230, 70, 20);

        // Waveform selector and label
        waveformSelector.setBounds(10, 260, 100, 20);
        currentWaveformLabel.setBounds(120, 260, 200, 20);

        // Adjust the waveform display size
        waveformDisplay.setBounds(10, 290, 400, 100);  // Reduce height

        // Ensure the reese control is visible
        reeseSlider.setBounds(10, 450, 400, 40);      // Adjust y-position for visibility
        reeseLabel.setBounds(10, 400, 120, 20);       // Label for the reese slider
        
        reeseToggleButton.setBounds(10, 480, 100, 20); // Adjust the position as needed
        
        //volume knob
        volumeKnob.setBounds(450, 400, 80, 80);  // Adjust the position and size of the knob
        volumeLabel.setBounds(450, 480, 80, 20); // Position the label under the knob

        //cutoff slider
        cutoffSlider.setBounds(450, 50, 150, 50); // Position and size of the cutoff slider
        cutoffLabel.setBounds(450, 30, 100, 20);  // Label above the slider
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
            processor.setVolume(volumeKnob.getValue());  // Set the volume
        else if (slider == &cutoffSlider)
            processor.setCutoffFrequency(cutoffSlider.getValue());
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
        waveformDisplay.setWaveform(processor.getWaveformData()); // Assuming this method exists
        waveformDisplay.setWaveformColor(juce::Colours::green);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthEditor)
};
