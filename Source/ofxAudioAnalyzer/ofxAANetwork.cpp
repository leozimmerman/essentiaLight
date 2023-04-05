/*
 * Copyright (C) 2019 Leo Zimmerman [http://www.leozimmerman.com.ar]
 *
 * ofxAudioAnalyzer is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 *
 * ---------------------------------------------------------------
 *
 * This project uses Essentia, copyrighted by Music Technology Group - Universitat Pompeu Fabra
 * using GNU Affero General Public License.
 * See http://essentia.upf.edu for documentation.
 *
 */

// NETWORK DIAGRAM:
// https://app.diagrams.net/#G14q2cA6zyjloAdpmDFluz4zcrpQkGqIXO

#include "ofxAANetwork.h"
#include "ofxAAConfigurations.h"
#include "ofxAAFactory.h"

#define LOUDNESS_MAX_VALUE 100.0
#define DYN_COMP_MAX_VALUE 50.0
#define STRONG_DECAY_MAX_VALUE 120.0
#define FLATNESS_SFX_MAX_VALUE 60.0

#define CREST_MAX_VALUE 50.0
#define ENERGY_MAX_VALUE 1.50
#define ENTROPY_MAX_VALUE 10.0

#define KURTOSIS_MIN_VALUE -100
#define KURTOSIS_MAX_VALUE 1000

#define SPREAD_MIN_VALUE 0.0
#define SPREAD_MAX_VALUE 0.2

#define SKEWNESS_MIN_VALUE -2.0
#define SKEWNESS_MAX_VALUE 25.0

#define SPECTRAL_COMPLEXITY_MAX_VALUE 75.0

#define HFC_MAX_VALUE 8000.0
#define ODD_TO_EVEN_MAX_VALUE 10.0
#define STRONG_PEAK_MAX_VALUE 150
#define PITCH_YIN_FREQ_MAX_VALUE 4186.0 //C8

#define GFCC_MAX_VALUE 36000

#define HPCP_SIZE 12

//TODO: Remove deprecated mfcc ?

namespace ofxaa {
    
    Network::Network(int sr, int bufferSize){
        _framesize = bufferSize;
        _samplerate = sr;
        
        _audioSignal.resize(bufferSize);
        //_accumulatedAudioSignal.resize(bufferSize * ACCUMULATED_SIGNAL_MULTIPLIER, 0.0);
        
        createAlgorithms();
        connectAlgorithms();
    }
    
    Network::~Network(){
        deleteAlgorithms();
    }
    
    void Network::deleteAlgorithms(){
        for (auto a : algorithms){
            a->deleteAlgorithm();
            delete a;
        }
    }
    
    //MARK: - CREATE ALGORITHMS
    void Network::createAlgorithms(){
        int sr = _samplerate;
        int fs = _framesize;
        
        dcRemoval = new ofxAAOneVectorOutputAlgorithm(DCRemoval, sr, fs);
        algorithms.push_back(dcRemoval);
        
        //MARK: TEMPORAL
        rms = new ofxAASingleOutputAlgorithm(Rms, sr, fs);
        rms->hasLogarithmicValues = true;
        algorithms.push_back(rms);
        
        power = new ofxAASingleOutputAlgorithm(InstantPower, sr, fs);
        power->hasLogarithmicValues = true;
        algorithms.push_back(power);
        
        loudness = new ofxAASingleOutputAlgorithm(Loudness, sr, fs);
        loudness->maxEstimatedValue = LOUDNESS_MAX_VALUE;
        algorithms.push_back(loudness);
        
      
    }
    
    //MARK: - CONNECT ALGORITHMS
    void Network::connectAlgorithms(){
        
        dcRemoval->algorithm->input("signal").set(_audioSignal);
        dcRemoval->algorithm->output("signal").set(dcRemoval->outputValues);
        

        //MARK: TEMPORAL
        rms->algorithm->input("array").set(dcRemoval->outputValues);
        rms->algorithm->output("rms").set(rms->outputValue);
        
        power->algorithm->input("array").set(dcRemoval->outputValues);
        power->algorithm->output("power").set(power->outputValue);
        
        loudness->algorithm->input("signal").set(dcRemoval->outputValues);
        loudness->algorithm->output("loudness").set(loudness->outputValue);
        
    }
    //MARK: - COMPUTE
    

    void Network::computeAlgorithms(vector<Real>& signal){
        _audioSignal = signal;
        for (int i=0; i<algorithms.size(); i++){
            algorithms[i]->compute();
        }
    }
    
    //MARK: - GET VALUES
    float Network::getValue(ofxAAValue value, float smooth, bool normalized){
        switch (value) {
            case NONE:
                juce::Logger::outputDebugString("ofxAANetwork: getValue() for NONE value type");
                return 0.0;
                
            default:
                auto singleAlgorithm = dynamic_cast<ofxAASingleOutputAlgorithm*>(getAlgorithmWithType(value));
                return singleAlgorithm->getValue(smooth, normalized);
        }
    }
    
    vector<float>& Network::getValues(ofxAABinsValue value, float smooth, bool normalized){
        static vector<float> r(1, 0.0);
        switch (value){
            case NONE_BINS:
                juce::Logger::outputDebugString("ofxAANetwork: getValues() for NONE_BINS type");
                return r;
                
            default:
                return getAlgorithmWithType(value)->getValues(smooth, normalized);
        }
    }
    //MARK: - 
    ofxAABaseAlgorithm* Network::getAlgorithmWithType(ofxAAValue valueType){
        switch (valueType) {
                //MARK: TEMPORAL
            case RMS:
                return rms;
            case POWER:
                return power;
          
            case LOUDNESS:
                return loudness;
                
            case NONE:
                juce::Logger::outputDebugString("ofxAANetwork: getValue() for NONE value type");
                return NULL;
                
            default:
                return NULL;
                break;
        }
    }
    
    ofxAAOneVectorOutputAlgorithm* Network::getAlgorithmWithType(ofxAABinsValue valueType){
        switch (valueType){
                
            case NONE_BINS:
                juce::Logger::outputDebugString("ofxAANetwork: getValues() for NONE_BINS type.");
                return NULL;
            default:
                return NULL;
                break;
        }
    }
    //----------------------------------------------
    float Network::getMinEstimatedValue(ofxAAValue valueType){
        //TODO: Copy max
        return getAlgorithmWithType(valueType)->minEstimatedValue;
    }
    //----------------------------------------------
    float Network::getMaxEstimatedValue(ofxAAValue valueType){
        switch (valueType) {
            default:
                return getAlgorithmWithType(valueType)->maxEstimatedValue;
    
        }
    }
    //----------------------------------------------
    float Network::getMaxEstimatedValue(ofxAABinsValue valueType){
        return getAlgorithmWithType(valueType)->maxEstimatedValue;
    }
    
    //----------------------------------------------
    float Network::getMinEstimatedValue(ofxAABinsValue valueType){
        return getAlgorithmWithType(valueType)->minEstimatedValue;
    }
    //----------------------------------------------
    void Network::setMaxEstimatedValue(ofxAAValue valueType, float value){
        switch (valueType) {
            default:
                getAlgorithmWithType(valueType)->maxEstimatedValue = value;
                break;
        }
    }
    //----------------------------------------------
    void Network::setMaxEstimatedValue(ofxAABinsValue valueType, float value){
        getAlgorithmWithType(valueType)->maxEstimatedValue = value;
    }
}
