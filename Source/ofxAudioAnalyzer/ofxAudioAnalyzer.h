/*
 * Copyright (C) 2016 Leo Zimmerman [http://www.leozimmerman.com.ar]
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

//
#include "ofxAudioAnalyzerUnit.h"
#include <JuceHeader.h>

class ofxAudioAnalyzer{
 
 public:
    
    void setup(int sampleRate, int bufferSize, int channels);
    void reset(int sampleRate, int bufferSize, int channels);
    void analyze(const juce::AudioBuffer<float>& buffer);
    void exit();
    
    int getSampleRate() const {return _samplerate;}
    int getBufferSize() const {return _buffersize;}
    int getChannelsNum() const {return _channels;}
    
    ///Gets value of single output  Algorithms.
    ///\param algorithm
    ///\param channel: starting from 0 (for stereo setup, 0 and 1)
    ///\param smooth: smoothing amount. 0.0=non smoothing, 1.0=fixed value
    float getValue(ofxAAValue valueType, int channel, float smooth=0.0, bool normalized=false) const;
    
    ///Gets values of vector output Algorithms.
    ///\param algorithm
    ///\param channel: starting from 0 (for stereo setup, 0 and 1)
    ///\param smooth: smoothing amount. 0.0=non smoothing, 1.0=fixed value
    vector<float>& getValues(ofxAABinsValue valueType, int channel, float smooth, bool normalized);
    
    ///Returns if there is an onset in the speciefied channel.
    bool getOnsetValue(int channel) const;
    
    ///Pointers for the audio analyzing units.
    ///Use very carefully!
    vector<ofxAudioAnalyzerUnit*>& getChannelAnalyzersPtrs(){return channelAnalyzerUnits;}
    
    ///Resets onsetsr detections buffer
    void resetOnsets(int channel);
    
    ///Set max estimated values for algorithms that are not normalized
    void setMaxEstimatedValue(int channel, ofxAAValue valueType, float value);
    void setMaxEstimatedValue(int channel, ofxAABinsValue valueType, float value);
    
    ///Sets onsets detection parameters
    ///\param channel: starting from 0 (for stereo setup, 0 and 1)
    ///\param alpha: the proportion of the mean included to reject smaller peaks--filters very short onsets
    ///\param silenceThreshold: the thhreshold for silence
    ///\param timeThreshold: time threshold in ms.
    ///\param useTimeThreshold: use or note the time threshold.
    void setOnsetsParameters(int channel, float alpha, float silenceTresh, float timeTresh, bool useTimeTresh = true);
    

 private:
    
    int _samplerate;
    int _buffersize;
    int _channels;
    
    vector<ofxAudioAnalyzerUnit*> channelAnalyzerUnits;
    
    
};

