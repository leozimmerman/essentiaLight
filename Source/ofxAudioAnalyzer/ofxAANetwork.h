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

#pragma once

#include "ofxAudioAnalyzerAlgorithms.h"
#include "ofxAAValues.h"
#include <JuceHeader.h>


#define ACCUMULATED_SIGNAL_MULTIPLIER 20

namespace ofxaa {
    class Network {
    public:
        Network(int sampleRate, int bufferSize);
        ~Network();
        
        void computeAlgorithms(vector<Real>& signal);
        
        float getValue(ofxAAValue value, float smooth, bool normalized);
        float getValue(ofxAAValue value){ return getValue(value, 0.0, false); }
        
        vector<float>& getValues(ofxAABinsValue value, float smooth, bool normalized);
        vector<float>& getValues(ofxAABinsValue value){ return getValues(value, 0.0, false); }

        float getMinEstimatedValue(ofxAAValue valueType);
        float getMinEstimatedValue(ofxAABinsValue valueType);
        
        float getMaxEstimatedValue(ofxAAValue valueType);
        float getMaxEstimatedValue(ofxAABinsValue valueType);
        
        void setMaxEstimatedValue(ofxAAValue valueType, float value);
        void setMaxEstimatedValue(ofxAABinsValue valueType, float value);
        
        //ofxAAOnsetsAlgorithm* getOnsetsPtr(){ return onsets;}
        
        ofxAABaseAlgorithm* getAlgorithmWithType(ofxAAValue valueType);
        ofxAAOneVectorOutputAlgorithm* getAlgorithmWithType(ofxAABinsValue valueType);
        
    private:
        
        void createAlgorithms();
        
        void connectAlgorithms();
        void deleteAlgorithms();
        
        int _samplerate;
        int _framesize;
        
        vector<Real> _audioSignal;
        //vector<Real> _accumulatedAudioSignal;
        
        vector<ofxAABaseAlgorithm*> algorithms;
        
        ofxAAOneVectorOutputAlgorithm* dcRemoval;
        ofxAASingleOutputAlgorithm* rms;
        ofxAASingleOutputAlgorithm* power;
        ofxAASingleOutputAlgorithm* loudness;
        
    };
}
