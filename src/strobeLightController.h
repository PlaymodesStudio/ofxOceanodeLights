//
//  strobeLightController.h
//  Espills_controller
//
//  Created by Eduard Frigola on 14/08/2018.
//

#ifndef strobeLightController_h
#define strobeLightController_h

#include "ofxOceanodeNodeModel.h"
#include "fixture.h"

class strobeLightController : public ofxOceanodeNodeModel{
public:
    strobeLightController() : ofxOceanodeNodeModel("StrobeLight Controller"){};
    ~strobeLightController(){};
    
    void setup() override;
    void update(ofEventArgs &e) override;
    
    void rgbToRgbw(float &r, float &g, float &b, float &w, bool clamp);
    
    void presetHasLoaded() override {
//        red = {0};
//        green = {0};
//        blue = {0};
//        fader = {0};
    };
    
private:

	void loadDMXChannels();
	
	ofEventListener listener;
    
    ofParameter<int> numElements;
//    ofParameter<int> lightType;
    ofParameter<vector<float>> red;
    ofParameter<vector<float>> green;
    ofParameter<vector<float>> blue;
    ofParameter<vector<float>> saturate;
    ofParameter<vector<float>> fader;
    ofParameter<float> masterFader;
    ofParameter<vector<float>> strobeRate;
    ofParameter<vector<float>> strobeWidth;
    ofParameter<vector<float>> colorOutput;
	ofParameter<vector<fixture>> output;
	
	vector<fixture> fixtures;
	vector<int> channels;
};

#endif /* strobeLightController_h */
