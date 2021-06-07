//
//  oscSender.h
//  Beams_Controlls
//
//  Created by Eduard Frigola Bagué on 14/11/2018.
//

#ifndef oscSenders_h
#define oscSenders_h

#include "ofxOceanodeNodeModel.h"
#include "ofxOsc.h"

class oscSenders : public ofxOceanodeNodeModel{
public:
    oscSenders() : ofxOceanodeNodeModel("Osc Sender"){
        sender = nullptr;
    };
    ~oscSenders(){};
    
    void setup() override;
    void update(ofEventArgs &a) override;
private:
    void setupSender(){
        if(sender != nullptr){
            delete sender;
        }
        sender = new ofxOscSender();
        sender->setup(host, ofToInt(port));
    }
    
    ofEventListeners listeners;
    
    ofParameter<string> host;
    ofParameter<string> port;
    ofParameter<vector<float>> pan;
    ofParameter<vector<float>> tilt;
    vector<ofParameter<vector<float>>> colors;
    
    
    
    ofxOscSender* sender;
};

#endif /* oscSenders_h */
