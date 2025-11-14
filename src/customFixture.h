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
        for(int i = 0; i < splittedConfig.size(); i++){
            vector<string> ss = ofSplitString(splittedConfig[i], ":");
            float defVal = ofToFloat(ss[1]);
            if(ofStringTimesInString(ss[0], "//16") == 1){
                ofStringReplace(ss[0], "//16", "");
                if(ss.size() == 4){ //Parameter with custom min max
                    float minVal = ofToFloat(ss[2]);
                    float maxVal = ofToFloat(ss[3]);
                    if(abs(maxVal - minVal) == 65535){
                        std::shared_ptr<ofParameter<vector<int>>> temp(new ofParameter<vector<int>>(ss[0], vector<int>(1, defVal), vector<int>(1, minVal), vector<int>(1, maxVal)));
                        addParameter(temp);
                        channelsParams.emplace_back(true, temp);
                    }else{
                        std::shared_ptr<ofParameter<vector<float>>> temp(new ofParameter<vector<float>>(ss[0], vector<float>(1, defVal), vector<float>(1, minVal), vector<float>(1, maxVal)));
                        addParameter(temp);
                        channelsParams.emplace_back(true, temp);
                    }
                }else if(ss.size() == 2){ //Standard Parmaeter
                    std::shared_ptr<ofParameter<vector<int>>> temp(new ofParameter<vector<int>>(ss[0], vector<int>(1, defVal), {0}, {65535}));
                    addParameter(temp);
                    channelsParams.emplace_back(true, temp);
                }
                numChannels += 2;
            }else{
                if(ss.size() == 4){ //Parameter with custom min max
                    float minVal = ofToFloat(ss[2]);
                    float maxVal = ofToFloat(ss[3]);
                    if(abs(maxVal - minVal) == 255){
                        std::shared_ptr<ofParameter<vector<int>>> temp(new ofParameter<vector<int>>(ss[0], vector<int>(1, defVal), vector<int>(1, minVal), vector<int>(1, maxVal)));
                        addParameter(temp);
                        channelsParams.emplace_back(false, temp);
                    }else{
                        std::shared_ptr<ofParameter<vector<float>>> temp(new ofParameter<vector<float>>(ss[0], vector<float>(1, defVal), vector<float>(1, minVal), vector<float>(1, maxVal)));
                        addParameter(temp);
                        channelsParams.emplace_back(false, temp);
                    }
                }else if(ss.size() == 2){ //Standard Parameter
                    std::shared_ptr<ofParameter<vector<int>>> temp(new ofParameter<vector<int>>(ss[0], vector<int>(1, defVal), {0}, {255}));
                    addParameter(temp);
                    channelsParams.emplace_back(false, temp);
                }
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
                vector<float> values;
                float min = 0;
                float max = 0;
                bool isHiRes = channelsParams[j].first;
                if(typeid(*channelsParams[j].second.get()) == typeid(ofParameter<vector<float>>)){
                    auto param = channelsParams[j].second->cast<vector<float>>();
                    values = param;
                    min = param.getMin()[0];
                    max = param.getMax()[0];
                }else if(typeid(*channelsParams[j].second.get()) == typeid(ofParameter<vector<int>>)){
                    auto param = channelsParams[j].second->cast<vector<int>>();
                    values = vector<float>(param->begin(), param->end());
                    min = param.getMin()[0];
                    max = param.getMax()[0];
                }else{
                    ofLog() << "Parameter type error";
                }
                
                if(i < values.size()){
                    indexToGet = i;
                }
                
                if(isHiRes){ //True means a hires parameter
                    int mappedvalue = ofMap(values[indexToGet], min, max, 0, 65535);
                    fix.data[currentChannelToWrite] = (mappedvalue >> 8) & 0xFF;  // Shift right 8 bits and mask
                    fix.data[currentChannelToWrite + 1] = mappedvalue & 0xFF;  // Mask to get the fine 8-bit value
                    currentChannelToWrite += 2;
                }else{
                    int mappedvalue = ofMap(values[indexToGet], min, max, 0, 255);
                    fix.data[currentChannelToWrite] = mappedvalue;
                    currentChannelToWrite += 1;
                }
            }
        }
        output = fixtures;
    }
    
    void loadBeforeConnections(ofJson &json){
        deserializeParameter(json, numElements);
    }
    
private:
    string additionalName;
    string configuration;
    ofEventListener listener;
    
    ofParameter<int> numElements;
    ofParameter<int> universe;
    vector<ofParameter<int>> channels;
    vector<std::pair<bool, shared_ptr<ofAbstractParameter>>> channelsParams;
    ofParameter<vector<fixture>> output;
    
    int numChannels;
};

#endif /* customFixture_h */
