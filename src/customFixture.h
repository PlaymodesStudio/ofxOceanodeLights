//
//  customFixture.hpp
//  ofxOceanodeLights
//
//  Created by Eduard Frigola on 22/9/23.
//

#ifndef customFixture_h
#define customFixture_h

#include "ofxOceanodeNodeModel.h"
#include "fixture.h"

class customFixture: public ofxOceanodeNodeModel{
public:
    customFixture(string name, string config) : additionalName(name), configuration(config), ofxOceanodeNodeModel("Fixture " + name){
        numChannels = 0;
    };
    
    void setup(){
        vector<string> splittedConfig = ofSplitString(configuration, ", ");
        channelsParams.resize(splittedConfig.size());
        for(int i = 0; i < splittedConfig.size(); i++){
            vector<string> ss = ofSplitString(splittedConfig[i], ":");
            if(ofStringTimesInString(ss[0], "//16") == 1){
                ofStringReplace(ss[0], "//16", "");
                addParameter(channelsParams[i].set(ss[0], vector<int>(1, ofToInt(ss[1])), {0}, {65535}));
                numChannels += 2;
            }else{
                addParameter(channelsParams[i].set(ss[0], vector<int>(1, ofToInt(ss[1])), {0}, {255}));
                numChannels += 1;
            }
        }
        addParameter(output.set("Output", {}));
        
        addInspectorParameter(numElements.set("Num Elements", 1, 1, INT_MAX));
        addInspectorParameter(universe.set("Universe", 1, 1, 65535));
        
        listener = numElements.newListener([this](int &i){
            if(channels.size() != i){
                int oldSize = channels.size();
                channels.resize(i);

                for(int j = 0; j < oldSize; j++){
                    channels[j] = 1 + (numChannels * j);
                }
                if(oldSize > i){
                    for(int j = oldSize-1; j >= i; j--){
                        removeInspectorParameter("Chan Fix " + ofToString(j));
                    }
                }else{
                    for(int j = oldSize; j < i; j++){
                        addInspectorParameter(channels[j].set("Chan Fix " + ofToString(j), 1 + (numChannels * j), 1, 512));
                    }
                }
            }
        });
        
        numElements = numElements;
    }
    
    void update(ofEventArgs &a){
        vector<fixture> fixtures(numElements);
        for(int i = 0; i < numElements; i++){
            auto &fix = fixtures[i];
            fix.startUniverse = universe;
            fix.startChannel = channels[i];
            fix.data.resize(numChannels);
            
            int currentChannelToWrite = 0;
            for(int j = 0; j < channelsParams.size(); j++){
                int indexToGet = 0;
                if(i < channelsParams[j]->size()){
                    indexToGet = i;
                }
                if(channelsParams[j].getMax()[0] == 65535){ //Is a coarse - fine parameter;
                    fix.data[currentChannelToWrite] = (channelsParams[j]->at(indexToGet) >> 8) & 0xFF;  // Shift right 8 bits and mask
                    fix.data[currentChannelToWrite + 1] = channelsParams[j]->at(indexToGet) & 0xFF;  // Mask to get the fine 8-bit value
                    currentChannelToWrite += 2;
                }else{
                    fix.data[currentChannelToWrite] = channelsParams[j]->at(indexToGet);
                    currentChannelToWrite += 1;
                }
            }
        }
        output = fixtures;
    }
    
private:
    string additionalName;
    string configuration;
    ofEventListener listener;
    
    ofParameter<int> numElements;
    ofParameter<int> universe;
    vector<ofParameter<int>> channels;
    vector<ofParameter<vector<int>>> channelsParams;
    ofParameter<vector<fixture>> output;
    
    int numChannels;
};

#endif /* customFixture_h */
