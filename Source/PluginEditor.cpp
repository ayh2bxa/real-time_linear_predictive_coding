/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VoicemorphAudioProcessorEditor::VoicemorphAudioProcessorEditor (VoicemorphAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), excitationDropdown (p.getExcitationDropdown())
{
    lpcSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    lpcSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    addAndMakeVisible(lpcSlider);
    lpcLabel.setText("LPC Mix", juce::dontSendNotification);
    lpcLabel.attachToComponent(&lpcSlider, false);
    addAndMakeVisible(lpcLabel);
    lpcMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "lpc mix", lpcSlider);
    
    exLenSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    exLenSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    addAndMakeVisible(exLenSlider);
    exLenLabel.setText("Length", juce::dontSendNotification);
    exLenLabel.attachToComponent(&exLenSlider, false);
    addAndMakeVisible(exLenLabel);
    exLenAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "ex len", exLenSlider);

    exStartSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    exStartSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    addAndMakeVisible(exStartSlider);
    exStartLabel.setText("Start", juce::dontSendNotification);
    exStartLabel.attachToComponent(&exStartSlider, false);
    addAndMakeVisible(exStartLabel);
    exStartAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "ex start pos", exStartSlider);
    
    orderSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    orderSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    addAndMakeVisible(orderSlider);
    orderLabel.setText("Order", juce::dontSendNotification);
    orderLabel.attachToComponent(&orderSlider, false);
    addAndMakeVisible(orderLabel);
    orderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "lpc order", orderSlider);
    
    inputGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    addAndMakeVisible(inputGainSlider);
    inputGainLabel.setText("Input Gain", juce::dontSendNotification);
    inputGainLabel.attachToComponent(&inputGainSlider, false);
    addAndMakeVisible(inputGainLabel);
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "input gain", inputGainSlider);

    outputGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    addAndMakeVisible(outputGainSlider);
    outputGainLabel.setText("Output Gain", juce::dontSendNotification);
    outputGainLabel.attachToComponent(&outputGainSlider, false);
    addAndMakeVisible(outputGainLabel);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(vts, "output gain", outputGainSlider);
    
    addAndMakeVisible(excitationDropdown);
    excitationDropdown.addListener(this);
    setSize (600, 600);
}

VoicemorphAudioProcessorEditor::~VoicemorphAudioProcessorEditor()
{
    outputGainAttachment.reset();
    inputGainAttachment.reset();
    exLenAttachment.reset();
    lpcMixAttachment.reset();
    exStartAttachment.reset();
    orderAttachment.reset();
    excitationDropdown.removeListener(this);
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
    lpcSlider.setBoundsRelative(0.05, 0.2, 0.2, 0.2);
    exLenSlider.setBoundsRelative(0.3, 0.2, 0.2, 0.2);
    exStartSlider.setBoundsRelative(0.55, 0.2, 0.2, 0.2);
    orderSlider.setBoundsRelative(0.05, 0.45, 0.2, 0.2);
    inputGainSlider.setBoundsRelative(0.3, 0.45, 0.2, 0.2);
    outputGainSlider.setBoundsRelative(0.55, 0.45, 0.2, 0.2);
    excitationDropdown.setBoundsRelative(0.05, 0.8, 0.25, 0.05);
}

void VoicemorphAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // Handle slider change events if needed
    // For now, you can leave it empty
}

void VoicemorphAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    // Handle combo box change events if needed
    // For now, you can leave it empty
    if (comboBoxThatHasChanged == &excitationDropdown) {
        audioProcessor.apvts.getParameterAsValue("ex type").setValue(excitationDropdown.getSelectedId()-1);
    }
}
