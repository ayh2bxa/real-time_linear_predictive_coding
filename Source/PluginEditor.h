/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <cmath>

//==============================================================================
/**
*/
using namespace juce;
using namespace std;

class VoicemorphAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::ComboBox::Listener
{
public:
    VoicemorphAudioProcessorEditor (VoicemorphAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~VoicemorphAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VoicemorphAudioProcessor& audioProcessor;
    ComboBox excitationDropdown;
    juce::Slider lpcSlider;
    juce::Label lpcLabel;
    juce::Slider exLenSlider;
    juce::Label exLenLabel;
    juce::Slider exStartSlider;
    juce::Label exStartLabel;
    Slider inputGainSlider;
    Label inputGainLabel;
    Slider outputGainSlider;
    Label outputGainLabel;
    Slider orderSlider;
    Label orderLabel;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lpcMixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> exLenAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> exStartAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> orderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoicemorphAudioProcessorEditor)
};
