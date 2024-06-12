/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VoicemorphAudioProcessorEditor::VoicemorphAudioProcessorEditor (VoicemorphAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 400);
    gainSlider.setRange(0.f, 1.f);
    gainSlider.setValue(audioProcessor.getCurrentGain(), juce::dontSendNotification);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);
    gainLabel.setText("gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, true);
//    addAndMakeVisible(gainLabel);
    
    pitchSlider.setRange(-12.f, 12.f);
    pitchSlider.setValue(audioProcessor.getPitchFactor(), juce::dontSendNotification);
    pitchSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    pitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    pitchSlider.addListener(this);
    addAndMakeVisible(pitchSlider);
    pitchLabel.setText("pitch", juce::dontSendNotification);
    pitchLabel.attachToComponent(&pitchSlider, true);
//    addAndMakeVisible(pitchLabel);
    
    lpcSlider.setRange(0.f, 1.f);
    lpcSlider.setValue(audioProcessor.getLpcMix(), juce::dontSendNotification);
    lpcSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    lpcSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    lpcSlider.addListener(this);
    addAndMakeVisible(lpcSlider);
    lpcLabel.setText("lpc", juce::dontSendNotification);
    lpcLabel.attachToComponent(&lpcSlider, true);
    addAndMakeVisible(lpcLabel);
}

VoicemorphAudioProcessorEditor::~VoicemorphAudioProcessorEditor()
{
}

//==============================================================================
void VoicemorphAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
}

void VoicemorphAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    gainSlider.setBounds(20, 20, getWidth()/3-30, getHeight()-150);
    pitchSlider.setBounds(getWidth()/3, 20, getWidth()/3-30, getHeight()-150);
    lpcSlider.setBounds(2*getWidth()/3, 20, getWidth()/3-30, getHeight()-150);
}

void VoicemorphAudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
    if (slider == &gainSlider) {
        audioProcessor.setTargetGain(gainSlider.getValue());
    }
    if (slider == &pitchSlider) {
        audioProcessor.setPitchFactor(pitchSlider.getValue());
    }
    if (slider == &lpcSlider) {
        audioProcessor.setLpcMix(lpcSlider.getValue());
    }
}
