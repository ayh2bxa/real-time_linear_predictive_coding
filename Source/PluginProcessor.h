/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "pitchshift.h"
#include "lpc.h"
#include "ParameterHelper.h"
#include <cmath>

//==============================================================================
/**
*/

using namespace juce;
using namespace std;

class VoicemorphAudioProcessor  : public juce::AudioProcessor, public ValueTree::Listener
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    VoicemorphAudioProcessor();
    ~VoicemorphAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    float getTargetGain();
    float getCurrentGain();
    float getPitchFactor();
    float getLpcMix();
    void setTargetGain(float val);
    void setCurrrentGain(float val);
    void setPitchFactor(float val);
    void setLpcMix(float val);
    LPC lpc;
    void setInputGain(float val) {inputGain = val;};
    float getInputGain() {return inputGain;};
    void setOutputGain(float val) {outputGain = val;};
    float getOutputGain() {return outputGain;};
    
    ComboBox& getExcitationDropdown() {return excitationDropdown;};
    AudioProcessorValueTreeState apvts;
private:
    //==============================================================================
    float targetGain = 0.f;
    float currentGain = 0.f;
    float pitchFactor = 0.f;
    float lpcMix = 0.f;
    float max_amp = 1.f;
    float inputGain = 1.f;
    float outputGain = 1.f;
    ComboBox excitationDropdown;
    
    void loadFactoryExcitations();
    juce::File writeBinaryDataToTempFile(const void* data, int size, const juce::String& fileName);
    vector<vector<float>> factoryExcitations;
//    AudioProcessorValueTreeState::ParameterLayout makeParamLayout();
//    void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VoicemorphAudioProcessor)
};
