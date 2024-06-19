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
    
    pitchSlider.setRange(-12.f, 12.f);
    pitchSlider.setValue(audioProcessor.getPitchFactor(), juce::dontSendNotification);
    pitchSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    pitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    pitchSlider.addListener(this);
    addAndMakeVisible(pitchSlider);
    pitchLabel.setText("pitch", juce::dontSendNotification);
    pitchLabel.attachToComponent(&pitchSlider, true);
    
    lpcSlider.setRange(0.f, 1.f);
    lpcSlider.setValue(audioProcessor.getLpcMix(), juce::dontSendNotification);
    lpcSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    lpcSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    lpcSlider.addListener(this);
    addAndMakeVisible(lpcSlider);
    lpcLabel.setText("lpc mix", juce::dontSendNotification);
    lpcLabel.attachToComponent(&lpcSlider, true);
    addAndMakeVisible(lpcLabel);
    
    exLenSlider.setRange(0.f, 1.f);
    exLenSlider.setValue((float)(audioProcessor.lpc.get_exlen()/audioProcessor.lpc.get_max_exlen()), juce::dontSendNotification);
    exLenSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    exLenSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    exLenSlider.addListener(this);
    addAndMakeVisible(exLenSlider);
    exLenLabel.setText("ex len", juce::dontSendNotification);
    exLenLabel.attachToComponent(&exLenSlider, true);
    addAndMakeVisible(exLenLabel);
    
    inputGainSlider.setRange(-40, 10);
    inputGainSlider.setValue(20*log10(audioProcessor.getInputGain()), juce::dontSendNotification);
    inputGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    inputGainSlider.addListener(this);
    addAndMakeVisible(inputGainSlider);
    inputGainLabel.setText("input gain", juce::dontSendNotification);
    inputGainLabel.attachToComponent(&inputGainSlider, true);
    addAndMakeVisible(inputGainLabel);

    outputGainSlider.setRange(-40, 10);
    outputGainSlider.setValue(20*log10(audioProcessor.getOutputGain()), juce::dontSendNotification);
    outputGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    outputGainSlider.addListener(this);
    addAndMakeVisible(outputGainSlider);
    outputGainLabel.setText("output gain", juce::dontSendNotification);
    outputGainLabel.attachToComponent(&outputGainSlider, true);
    addAndMakeVisible(outputGainLabel);
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
    inputGainSlider.setBounds(20, 20, getWidth()/3-30, getHeight()/3);
    outputGainSlider.setBounds(20, 50+getHeight()/3, getWidth()/3-30, getHeight()/3);
    lpcSlider.setBounds(2*getWidth()/3, 20, getWidth()/3-30, getHeight()-150);
    exLenSlider.setBounds(getWidth()/3, 20, getWidth()/3-30, getHeight()-150);
}

void VoicemorphAudioProcessorEditor::sliderValueChanged(juce::Slider *slider) {
    if (slider == &gainSlider) {
        audioProcessor.setTargetGain(gainSlider.getValue());
    }
    else if (slider == &pitchSlider) {
        audioProcessor.setPitchFactor(pitchSlider.getValue());
    }
    else if (slider == &lpcSlider) {
        audioProcessor.setLpcMix(lpcSlider.getValue());
    }
    else if (slider == &exLenSlider) {
        audioProcessor.lpc.set_exlen((int)(exLenSlider.getValue()*audioProcessor.lpc.get_max_exlen()));
    }
    else if (slider == &inputGainSlider) {
        audioProcessor.setInputGain(pow(10.0, inputGainSlider.getValue()/20));
    }
    else if (slider == &outputGainSlider) {
        audioProcessor.setOutputGain(pow(10.0, outputGainSlider.getValue()/20));
    }
}
