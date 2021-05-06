//
//  MeterUnit.hpp
//  EssentiaTest
//
//  Created by Leo on 05/05/2021.
//

#pragma once

#include <JuceHeader.h>
#include "ofxAudioAnalyzer.h"
using namespace std;

class MeterUnit: private juce::AudioProcessorValueTreeState::Listener {
public:
    
    MeterUnit(int idx);
    ~MeterUnit();
    
    void setup(foleys::MagicProcessorState* magicState, juce::AudioProcessorValueTreeState* treeState, ofxAudioAnalyzer* audioAnalyzer);
    void parameterChanged (const juce::String& param, float value) override;
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void process();
    
    int getId() { return _idx; }
    unique_ptr<juce::AudioProcessorParameterGroup> getParameterGroup();
    
    juce::String meterId;
    juce::String algorithmTypeId;
    juce::String smoothingId;
    juce::String resetMaxId;
    juce::String maxEstimatedId;
    juce::String outputMeterId;
    juce::String historyPlotId;
    
private:
    void setOfxaaValue(ofxAAValue value);
    
    int _idx;
    ofxAudioAnalyzer* _audioAnalyzer;

    ofxAAValue currentOfxaaValue = NONE;
    foleys::MagicLevelSource* outputMeter  = nullptr;
    
    atomic<bool>* resetMax  = nullptr;
    atomic<float>* smoothing  = nullptr;
    atomic<float>* maxEstimated  = nullptr;
    
};
