//
//  followIndexsToMovingHeads.h
//  Beams_Controlls
//
//  Created by Eduard Frigola Bagué on 21/12/2018.
//

#ifndef followIndexsToMovingHeads_h
#define followIndexsToMovingHeads_h

#include "ofxOceanodeNodeModel.h"

class followIndexsToMovingHeads : public ofxOceanodeNodeModel{
public:
    followIndexsToMovingHeads() : ofxOceanodeNodeModel("Follow Index to Moving Heads"){};
    ~followIndexsToMovingHeads(){};
    
    void setup() override{
        addParameter(input.set("Input", {0}, {0}, {1}));
        addParameter(distribution.set("Distribution", {0, 1, 2, 3, 4, 0, 1, 2, 3, 4}, {0}, {9}));
        addParameter(leftOutput.set("Left Output", {0}, {0}, {1}));
        addParameter(rightOutput.set("Right Output", {0}, {0}, {1}));
        
        listener = input.newListener([this](vector<float> &vf){
            int size = vf.size();
            vector<float> tempRightOutput(5, 0);
            vector<float> tempLeftOutput(5, 0);
            for(int i = 0; i < tempLeftOutput.size(); i++){
                if(distribution->at(i) < vf.size())
                    tempLeftOutput[i] = vf[distribution->at(i)];
                else
                    tempLeftOutput[i] = 0;
            }
            for(int i = 0; i < tempRightOutput.size(); i++){
                if(distribution->at(i+5) < vf.size())
                    tempRightOutput[i] = vf[distribution->at(i+5)];
                else
                    tempRightOutput[i] = 0;
            }
            leftOutput = tempLeftOutput;
            rightOutput = tempRightOutput;
        });
    };
    
private:
    ofEventListener listener;
    
    ofParameter<vector<float>> input;
    
    ofParameter<vector<int>> distribution;
    
    ofParameter<vector<float>> leftOutput;
    ofParameter<vector<float>> rightOutput;
};

#endif /* followIndexsToMovingHeads_h */
