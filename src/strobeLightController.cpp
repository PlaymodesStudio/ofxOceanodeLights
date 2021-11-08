//
//  strobeLightController.cpp
//  Espills_controller
//
//  Created by Eduard Frigola on 14/08/2018.
//

#include "strobeLightController.h"
#define NUM_OUTPUTS 6

void strobeLightController::setup(){
//    addParameter(lightType.set("Light Type", 0, 0, 1));
    addInspectorParameter(numElements.set("Num Elements", 32, 1, INT_MAX));
    addParameter(red.set("Red", {1}, {0}, {1}));
    addParameter(green.set("Green", {1}, {0}, {1}));
    addParameter(blue.set("Blue", {1}, {0}, {1}));
    addParameter(saturate.set("Saturate", {0}, {0}, {1}));
    addParameter(fader.set("Fader", {1}, {0}, {1}));
    addParameter(strobeRate.set("CTC", {0}, {0}, {255}));
    addParameter(strobeWidth.set("Shutter", {9}, {0}, {255}));
    addParameter(masterFader.set("Master Fader", 1, 0, 1));
    addParameter(colorOutput.set("Color Output", {0}, {0}, {1}));
	addParameter(output.set("Output", {}));
	
	listener = numElements.newListener([this](int &i){
		fixtures.resize(i);
		loadDMXChannels();
	});
	
	fixtures.resize(numElements);
	loadDMXChannels();
}

void strobeLightController::loadDMXChannels() {
	bool loaded = false;
	ofJson json = ofLoadJson("StrobeLightDMXChannels_" + ofToString(getNumIdentifier()) + ".json");
	if (!json.empty()) {
		if (json["size"].get<int>() == numElements) {
			channels = json["Channels"].get<vector<int>>();
			loaded = true;
		}
	}if (!loaded) {
		channels = { 1,1,1,1,1,1 };
	}
}

void strobeLightController::update(ofEventArgs &e){
    auto getValueForPosition([&](ofParameter<vector<float>> &p, int pos) -> float{
        if(pos < p->size()){
            return p->at(pos);
        }
        return p->at(0);
    });
    
    auto getValueForPositionInt([&](ofParameter<vector<int>> &p, int pos) -> int{
        if(pos < p->size()){
            return p->at(pos);
        }
        return p->at(0);
    });
    
    int elementSize = 12;
    int elementsPerUniverse = 6;
    int fixtureSize = 79;
    
    vector<float> tempColors;
    
//    for(int i = 0; i < NUM_OUTPUTS; i++){
//        tempOutputs[i].reserve(elementsPerUniverse*fixtureSize);
//    }
    
    tempColors.resize(numElements*3);
    vector<float> black = {0,0};
    vector<float> fixturePrefs = {masterFader.get(), 0, 0, 0, 0, 0, 0};
//    if(fader->size() == numElements){
//        for(int i = 0; i < numElements; i++){
//			tempColors[(i*3)] = getValueForPosition(red, i) * fader->at(i);
//			tempColors[(i*3)+1] = getValueForPosition(green, i) * fader->at(i);
//			tempColors[(i*3)+2] = getValueForPosition(blue, i) * fader->at(i);
//		}
//    }
//	else if(fader->size() == numElements*3){
//		for(int i = 0; i < numElements; i++){
//			tempColors[(i*3)] = getValueForPosition(red, i) * fader->at((i*3));
//			tempColors[(i*3)+1] = getValueForPosition(green, i) * fader->at((i*3)+1);
//			tempColors[(i*3)+2] = getValueForPosition(blue, i) * fader->at((i*3)+2);
//		}
////        for(int i = 0; i < numElements; i++){
////            int univers = i/6;
////            for(int j = 0; j < elementSize; j++){
////                int index = (i*elementSize*3)+(j*3);
////                vector<float> rgbw(fader->begin()+index, fader->begin()+index+4);
////                rgbToRgbw(rgbw[0], rgbw[1], rgbw[2], rgbw[3], false);
////                tempOutputs[univers].insert(tempOutputs[univers].end(), rgbw.begin(), rgbw.end());
////                tempOutputs[univers].insert(tempOutputs[univers].end(), black.begin(), black.end());
////            }
////
////            tempOutputs[univers].insert(tempOutputs[univers].end(), fixturePrefs.begin(), fixturePrefs.end());
////        }
////        for(int i = 0; i < NUM_OUTPUTS; i++){
////            dmxOutputs[i] = tempOutputs[i];
////        }
//    }
//	else{
//		for(int i = 0; i < numElements; i++){
//			tempColors[(i*3)] = getValueForPosition(red, i) * getValueForPosition(fader, i);
//			tempColors[(i*3)+1] = getValueForPosition(green, i) * getValueForPosition(fader, i);
//			tempColors[(i*3)+2] = getValueForPosition(blue, i) * getValueForPosition(fader, i);
//		}
//	}
    
	vector<fixture> fixtures(numElements);
    for(int i = 0; i < numElements; i++){
		auto &fix = fixtures[i];
		fix.startUniverse = getNumIdentifier();
		fix.startChannel = channels[i];
		fix.data.resize(10);
        
        //Shutter
        fix.data[0] = getValueForPositionInt(strobeWidth, i);
        
        //From ofxFixture::setDmxDimmer16bit()
//        int dim = dimmer * 65535;
//        int dim0 = (dim >> 8) & 0x00ff;
//        int dim1 =  dim       & 0x00ff;
        //Dimmer
        int dimmerAtIndex = getValueForPosition(fader, i) * masterFader * 65535;
        fix.data[1] = (dimmerAtIndex >> 8) & 0x00ff;
        fix.data[2] =  dimmerAtIndex       & 0x00ff;
        
        //CTC
        fix.data[3] = getValueForPositionInt(strobeRate, i);
        
        
		
        float posSaturate = getValueForPosition(saturate, i);

        int red_ = ((getValueForPosition(red, i) * (1-posSaturate)) + (1 * posSaturate)) * 65535;
        int green_ = ((getValueForPosition(green, i) * (1-posSaturate)) + (1 * posSaturate)) * 65535;
        int blue_ = ((getValueForPosition(blue, i) * (1-posSaturate)) + (1 * posSaturate)) * 65535;
        
        //Colors
        fix.data[4] = (red_ >> 8) & 0x00ff;
        fix.data[5] =  red_       & 0x00ff;
        fix.data[6] = (green_ >> 8) & 0x00ff;
        fix.data[7] =  green_       & 0x00ff;
        fix.data[8] = (blue_ >> 8) & 0x00ff;
        fix.data[9] =  blue_       & 0x00ff;
//
//        tempColors[(i*3)] = red_;
//        tempColors[(i*3)+1] = green_;
//        tempColors[(i*3)+2] = blue_;
		
		//Shutter
		//fix.data[0] = ofMap(getValueForPosition(strobeRate, i), 0, 1, 15, 151);
		
		//Dimmer
		//fix.data[1] = 255;
		
		//Color temp
		//fix.data[2] = 1;
//		float white = 0;
//
//		rgbToRgbw(red_, green_, blue_, white, false);

		//Rgbw
//		fix.data[0] = red_ * 255;
//		fix.data[1] = green_ * 255;
//		fix.data[2] = blue_ * 255;
//		fix.data[3] = white * 255;
        
	}
//
////        switch (lightType) {
////            case 0:
//                tempOutput[(i*elementSize)] = red_;
//                tempOutput[(i*elementSize)+1] = green_;
//                tempOutput[(i*elementSize)+2] = blue_;
//                tempOutput[(i*elementSize)+3] = 0;
////                break;
////            case 1:
////                tempOutput[(i*elementSize)] = red_;
////                tempOutput[(i*elementSize)+1] = green_;
////                tempOutput[(i*elementSize)+2] = blue_;
////                tempOutput[(i*elementSize)+3] = 0;
////                break;
////
////            default:
////                break;
////        }
//    }
//    dmxOutput = tempOutput;
    colorOutput = tempColors;
	output = fixtures;
}


void strobeLightController::rgbToRgbw(float &r, float &g, float &b, float &w, bool clamp){
    float tM = max(r, max(g, b));
    if(tM == 0){
        return;
    }
    
    //This section serves to figure out what the color with 100% hue is
    float multiplier = 1.0f/tM;
    float hR = r * multiplier;
    float hG = g * multiplier;
    float hB = b * multiplier;
    
    //This calculates the Whiteness (not strictly speaking Luminance) of the color
    float M = max(hR, max(hG, hB));
    float m = min(hR, min(hG, hB));
    float Luminance = ((M + m) / 2.0f - 0.5f) * (2.0f) / multiplier;
    
    if(clamp){
        if(r == g && r == b && g == b){
            Luminance = r;
        }else{
            Luminance = 0;
        }
    }
    
    //Calculate the output values
    w = Luminance;
    r -= Luminance;
    g -= Luminance;
    b -= Luminance;
    return;
}
