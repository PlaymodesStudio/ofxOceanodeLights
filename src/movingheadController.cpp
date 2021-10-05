//
//  movingheadController.cpp
//  Espills_controller
//
//  Created by Eduard Frigola Bagué on 02/07/2018.
//

#include "movingheadController.h"
#include "imgui.h"

movingheadController::movingheadController() : ofxOceanodeNodeModelExternalWindow("Movinghead Controller"){
    numGroups = 3;
    size.resize(numGroups);
    intensity.resize(numGroups);
    pan.resize(numGroups);
    tilt.resize(numGroups);
    colorDropdown.resize(numGroups);
    colorwheel.resize(numGroups);
    strobe.resize(numGroups);
    gobo.resize(numGroups);
    frost.resize(numGroups);
//    red.resize(numGroups);
//    green.resize(numGroups);
//    blue.resize(numGroups);
    dmxOutputs.resize(3);
    for(int i = 0 ; i < numGroups; i++){
        ofParameter<char> label;
        addParameter(label.set("Group " + ofToString(i) + "      ----------------", ' '));
        addParameter(size[i].set("Size " + ofToString(i), {1}, {1}, {500}));
        addParameter(intensity[i].set("Intensity " + ofToString(i), {0}, {0}, {1}));
        addParameter(pan[i].set("Pan " + ofToString(i), {0}, {-180}, {180}));
        addParameter(tilt[i].set("Tilt " + ofToString(i), {0}, {-180}, {180}));
        addParameterDropdown(colorDropdown[i], "Color " + ofToString(i), 0, {"White", "Red", "Orange", "Aquamarine", "Green", "Light Green", "Lavander", "Pink", "Yellow", "Magenta", "Cyan", "CTO 260", "CTO 190", "CTB 8000", "Blue"}, ofxOceanodeParameterFlags_DisableSavePreset | ofxOceanodeParameterFlags_DisableInConnection);
        addParameter(colorwheel[i].set("Col i " + ofToString(i), {0}, {0}, {colorDropdown[i].getMax()}));
//        addParameter(red[i].set("Red " + ofToString(i), {1}, {0}, {1}));
//        addParameter(green[i].set("Green " + ofToString(i), {1}, {0}, {1}));
//        addParameter(blue[i].set("Blue " + ofToString(i), {1}, {0}, {1}));
        addParameter(strobe[i].set("Focus " + ofToString(i), 0, 0, 1));
        addParameter(gobo[i].set("Gobo " + ofToString(i), {0}, {0}, {1}));
        addParameter(frost[i].set("Frost " + ofToString(i), {0}, {0}, {1}));
        
        dropdownListeners.push(colorDropdown[i].newListener([this, i](int &ind){
            colorwheel[i] = vector<int>(1, ind);
        }));
        
        sizeListeners.push(size[i].newListener([this](int &s){
            totalSize = 0;
            for(auto s : size) totalSize += s;
            loadCalibration();
			loadDMXChannels();
            recalculateSliders();
        }));
    }
    ofParameter<char> label;
    addParameter(label.set("Shared", ' '));
    addParameter(masterFader.set("Master Fader", 1, 0, 1));
    addParameter(lampOn.set("Lamp On", false));
    addParameter(lampOff.set("Lamp Off", false));
    addParameter(reset.set("Reset", false));
    addOutputParameter(dmxOutputs[0].set("Dmx Output 0", {0}, {0}, {1}));
    addOutputParameter(dmxOutputs[1].set("Dmx Output 1", {0}, {0}, {1}));
    addOutputParameter(panOutput.set("Pan Output", {0}, {-180}, {180}));
    addOutputParameter(tiltOutput.set("Tilt Output", {0}, {-180}, {180}));
    addOutputParameter(colorOutput.set("Color Output", {0}, {0}, {1}));
	addOutputParameter(output.set("Output", {}));
    
    
    indexClicked = -1;
    isHorizontal = true;
    
    totalSize = 0;
    for(auto s : size) totalSize += s;
    panRange = 540;
    tiltRange = 247;

	channels.resize(3);
	channels[0] = { 1,1,1,1,1 };
	channels[1] = { 1,1,1,1,1 };
	channels[2] = { 1,1,1,1,1 };
}

void movingheadController::setup() {
	if (getNumIdentifier() != -1) {
		loadCalibration();
		loadDMXChannels();
	}
}

void movingheadController::loadDMXChannels() {
	bool loaded = false;
	ofJson json = ofLoadJson("MovingHeadDMXChannels_" + ofToString(getNumIdentifier()) + ".json");
	if (!json.empty()) {
		if (json["size"].get<int>() == numGroups) {
			for (int i = 0; i < numGroups; i++) {
				channels[i] = json["Channels_" + ofToString(i)].get<vector<int>>();
			}
			loaded = true;
		}
	}if (!loaded) {
		channels.resize(3);
		channels[0] = { 1,1,1,1,1 };
		channels[1] = { 1,1,1,1,1 };
		channels[2] = { 1,1,1,1,1 };
	}
}

void movingheadController::loadCalibration(){
    bool loaded = false;
    ofJson json = ofLoadJson("MovingHeadCalibration_" + ofToString(getNumIdentifier()) + ".json");
    if(!json.empty()){
        if(json["size"].get<int>() == totalSize){
            panOffset = json["panOffset"].get<vector<float>>();
            tiltOffset = json["tiltOffset"].get<vector<float>>();
			focusOffset = json["focusOffset"].get<vector<float>>();
			panRange = json["panRange"];
			tiltRange = json["tiltRange"];
            loaded = true;
        }
    }if(!loaded){
        panOffset = vector<float>(totalSize, 0);
        tiltOffset = vector<float>(totalSize, 0);
		focusOffset = vector<float>(totalSize, 0);
    }
}

void movingheadController::saveCalibration(){
    ofJson json = ofLoadJson("MovingHeadCalibration_" + ofToString(getNumIdentifier()) + ".json");
    json["size"] = totalSize;
    json["panOffset"] = panOffset;
    json["tiltOffset"] = tiltOffset;
	json["focusOffset"] = focusOffset;
    ofSavePrettyJson("MovingHeadCalibration_" + ofToString(getNumIdentifier()) + ".json", json);
}

void movingheadController::update(ofEventArgs &a){
    int nChannels = 16;
    
    vector<vector<float>> dmxInfo;
    vector<float> panInfo;
    vector<float> tiltInfo;
    vector<float> colorInfo;
    
    dmxInfo.resize(totalSize, vector<float>(nChannels, 0));
    panInfo.resize(totalSize, .5);
    tiltInfo.resize(totalSize, .5);
    colorInfo.resize(totalSize*3, .5); //3 color components;
	
	vector<fixture> fixtures(totalSize);
    
    for(int i = 0; i < numGroups; i++){
        int accumulateSizes = 0;
        for(int k = 0; k < i; k++){
            accumulateSizes += size[k];
        }
        for(int j = 0; j < size[i]; j++){
            int index = j + accumulateSizes;
            
            auto &fix = fixtures[index];
			fix.startUniverse = 1;
			fix.startChannel = channels[i][j];
			fix.data.resize(nChannels);
			
            //color wheel
            vector<int> dmxColowheelValuesFromIndex{0, 9, 18, 26, 35, 43, 52, 60, 69, 77, 86, 94, 103, 111, 120};
            dmxInfo[index][7] = dmxColowheelValuesFromIndex.at(getValueAtIndex(colorwheel[i].get(), j))/255.0f;
            //dmxInfo[index][0] = getValueAtIndex(colorwheel[i].get(), j);
			fix.data[0] = dmxColowheelValuesFromIndex.at(getValueAtIndex(colorwheel[i].get(), j));
            
            //Strobe
			fix.data[1] = 255;
            
            //dimmer
            float dimmerAtIndex = getValueAtIndex(intensity[i].get(), j) * masterFader;
//            dmxInfo[index][2] = dimmerAtIndex;
//
//            dmxInfo[index][3] = ofMap(gobo[i], 0, 1, 0, 71.0/255.0);
			fix.data[2] = dimmerAtIndex * 255;
            
            //Prism
//            dmxInfo[index][4] = 0;
//            dmxInfo[index][5] = 0;
			
			//Gobo
			fix.data[3] = ofMap(getValueAtIndex(gobo[i].get(), j), 0, 1, 0, 71);
            
            //Efects

			//Frost
			fix.data[7] = getValueAtIndex(frost[i].get(), j) * 255;
			
			//Focus
			fix.data[8] = (strobe[i] + focusOffset[index]) * 255;
            
            //pan
            float panAtIndex = getValueAtIndex(pan[i].get(), j);
            panInfo[index] = ofClamp(panAtIndex, -180, 180);
            panAtIndex = ofMap(-panAtIndex, -panRange/2 + panOffset[index] - 180, panRange/2 + panOffset[index] - 180, 0, 1, true);
            dmxInfo[index][9] = panAtIndex;
            dmxInfo[index][10] = panAtIndex*255 - int(panAtIndex*255);
			fix.data[9] = dmxInfo[index][9]*255;
			fix.data[10] = dmxInfo[index][10]*255;
            
            //tilt
            float tiltAtIndex = getValueAtIndex(tilt[i].get(), j);
            tiltInfo[index] = ofClamp(tiltAtIndex, -180, 180);
            tiltAtIndex = ofMap(-tiltAtIndex, -tiltRange/2 + tiltOffset[index], tiltRange/2 + tiltOffset[index], 0, 1, true);
            dmxInfo[index][11] = tiltAtIndex;
            dmxInfo[index][12] = tiltAtIndex*255 - int(tiltAtIndex*255);
            fix.data[11] = dmxInfo[index][11]*255;
			fix.data[12] = dmxInfo[index][12]*255;
            
            //Function
            fix.data[13] = 24;
            
            //Reset
            if(reset){
                fix.data[14] = 255;
            }else{
                fix.data[14] = 0;
            }
            
            //Lamp Control
            if(lampOn)
                fix.data[15] = 101;
            else if(lampOff){
                fix.data[15] = 100;
            }else{
                fix.data[15] = 0;
            }
            
            
            //zoom
            //dmxInfo[index][15] = ofMap(strobe[i], 0, 1, 0, 1);
            
            /*
            //pan/tilt speed
            dmxInfo[index][4] = 0;
            
            dmxInfo[index][5] = 1; //Shutter open
            
            
            //dmxInfo[index][23] = dimmerAtIndex*255 - int(dimmerAtIndex*255);
            
            //color wheel
            dmxInfo[index][7] = ofMap(getValueAtIndex(colorwheel[i].get(), j), colorwheel[i].getMin()[0], colorwheel[i].getMax()[0]-1, 0, 27.0/255.0, true);
            
            
            //gobo
            dmxInfo[index][10] = ofMap(gobo[i], 0, 1, 0, 48.0/255.0);
            
            //frost
            dmxInfo[index][13] = ofMap(frost[i], 0, 1, 0, 1);
            */
            
            
            ofFloatColor colorValue;
            switch (int(getValueAtIndex(colorwheel[i].get(), j))) {
                case 0:
                    colorValue = ofFloatColor::white;
                    break;
                case 1:
                    colorValue = ofFloatColor::red;
                    break;
                case 2:
                    colorValue = ofFloatColor::orange;
                    break;
                case 3:
                    colorValue = ofFloatColor::aquamarine;
                    break;
                case 4:
                    colorValue = ofFloatColor::green;
                    break;
                case 5:
                    colorValue = ofFloatColor::lightGreen;
                    break;
                case 6:
                    colorValue = ofFloatColor::lavender;
                    break;
                case 7:
                    colorValue = ofFloatColor::pink;
                    break;
                case 8:
                    colorValue = ofFloatColor::yellow;//(46, 48, 89);
                    break;
                case 9:
                    colorValue = ofFloatColor::magenta;
                    break;
                case 10:
                    colorValue = ofFloatColor::cyan;
                    break;
                case 11:
                    colorValue = ofFloatColor(0.971, 0.783, 0.277);
                    break;
                case 12:
                    colorValue = ofFloatColor(0.964, 0.856, 0,529);
                    break;
                case 13:
                    colorValue = ofFloatColor(0.590, 0.779, 0.729);
                    break;
                case 14:
                    colorValue = ofFloatColor::blue;
                    break;
                    
                    
                default:
                    colorValue = ofColor::white;
                    break;
            }
            
//            colorInfo[index*3] = getValueAtIndex(red[i].get(), j) * dimmerAtIndex;
//            colorInfo[(index*3)+1] = getValueAtIndex(green[i].get(), j) * dimmerAtIndex;
//            colorInfo[(index*3)+2] = getValueAtIndex(blue[i].get(), j) * dimmerAtIndex;
            
            colorInfo[index*3] = colorValue.r * dimmerAtIndex;
            colorInfo[(index*3)+1] = colorValue.g * dimmerAtIndex;
            colorInfo[(index*3)+2] = colorValue.b * dimmerAtIndex;
             
        }
    }
	
	output = fixtures;
    
    vector<float> tempOutput(dmxInfo.size()*nChannels/2);
    for(int i = 0; i < (dmxInfo.size()*nChannels / 2); i++){
        tempOutput[i] = dmxInfo[i/nChannels][i%nChannels];
    }
    dmxOutputs[0] = tempOutput;
    
    tempOutput.resize(dmxInfo.size()*nChannels / 2);
    for(int i = 0; i < (dmxInfo.size()*nChannels / 2); i++){
        int newI = i + dmxInfo.size()*nChannels / 2;
        tempOutput[i] = dmxInfo[newI/nChannels][newI%nChannels];
    }
    dmxOutputs[1] = tempOutput;
    
    panOutput = panInfo;
    tiltOutput = tiltInfo;
    colorOutput = colorInfo;
}

void movingheadController::drawInExternalWindow(ofEventArgs &e){
    ofBackground(0);
    ofSetColor(255);
    for(int i = 0; i < points.size(); i++){
        auto &point = points[i];
        ofSetColor(255);
        ofNoFill();
        ofDrawRectangle(point);
        glm::vec3 numOffset = isHorizontal ? glm::vec3(0, -point.getHeight()/1.9, 1) : glm::vec3(point.getWidth()/1.9, 0, 1);
        ofDrawBitmapString(ofToString(i), point.getCenter() + numOffset);
        if(ofGetKeyPressed('p')){
            ofDrawBitmapString(ofToString(panOffset[i]), point.getCenter());
        }else if(ofGetKeyPressed('t')){
            ofDrawBitmapString(ofToString(tiltOffset[i]), point.getCenter());
        }else if(ofGetKeyPressed('f')){
            ofDrawBitmapString(ofToString(focusOffset[i]), point.getCenter());
        }
        ofPushStyle();
        ofSetColor(255, 127);
        ofFill();
        ofRectangle sliderVal;
        float offset = 0;
        if(ofGetKeyPressed('p')){
            offset = -panOffset[i] * float(max(point.getWidth(), point.getHeight()))/2;
        }else if(ofGetKeyPressed('t')){
            offset = -tiltOffset[i] * float(max(point.getWidth(), point.getHeight()))/2;
        }else if(ofGetKeyPressed('f')){
            offset = -focusOffset[i] * float(max(point.getWidth(), point.getHeight()))/2;
        }
        if(isHorizontal)
            sliderVal.setFromCenter(point.getCenter().x, point.getCenter().y + offset, point.getWidth(), 10);
        else{
            sliderVal.setFromCenter(point.getCenter().x + offset, point.getCenter().y, 10, point.getHeight());
        }
        ofDrawRectangle(sliderVal);
        ofPopStyle();
    }
}

void movingheadController::recalculateSliders(){
    points.resize(totalSize);
    if(externalWindowRect.width >= externalWindowRect.height){
        isHorizontal = true;
        int center = externalWindowRect.height/2;
        float step = float(externalWindowRect.width) / float(totalSize);
        float height = externalWindowRect.height / 1.2;
        float width = externalWindowRect.width / (totalSize*2);
        for(int i = 0; i < totalSize; i++){
            int pos = (step*i) + step/2;
            points[i].setFromCenter(pos, center, width, height);
        }
    }else{
        isHorizontal = false;
        int center = externalWindowRect.width/2;
        float step = externalWindowRect.height / float(totalSize);
        float height = externalWindowRect.height/(totalSize*2);
        float width = externalWindowRect.width / 1.2;
        for(int i = 0; i < totalSize; i++){
            int pos = (step*i) + step/2;
            points[i].setFromCenter(center, pos, width, height);
        }
    }
}

void movingheadController::windowResized(ofResizeEventArgs &a){
    recalculateSliders();
}

void movingheadController::keyPressed(ofKeyEventArgs &a){

}

void movingheadController::keyReleased(ofKeyEventArgs &a){

}

void movingheadController::mousePressed(ofMouseEventArgs &a){
    if(ofGetKeyPressed()){
        for(int i = 0; i < points.size(); i++){
            if(points[i].inside(a)){
            indexClicked = i;
            break;
            }
        }
        if(ofGetKeyPressed('p')){
            originalValue = panOffset[indexClicked];
        }else if(ofGetKeyPressed('t')){
            originalValue = tiltOffset[indexClicked];
        }else if(ofGetKeyPressed('f')){
            originalValue = focusOffset[indexClicked];
        }
        initialClicPos = a;
    }
}

void movingheadController::mouseReleased(ofMouseEventArgs &a){
//    indexClicked = -1;
}

void movingheadController::mouseDragged(ofMouseEventArgs &a){
    if(indexClicked != -1){
        glm::vec2 amountMoved = a - initialClicPos;
        if(isHorizontal){
            float modVal = -amountMoved.y/(externalWindowRect.height/2);
            if(ofGetKeyPressed(OF_KEY_SHIFT)) modVal /= 2;
            if(ofGetKeyPressed('p')){
                panOffset[indexClicked] = originalValue + modVal;
            }else if(ofGetKeyPressed('t')){
                tiltOffset[indexClicked] = originalValue + modVal;
            }else if(ofGetKeyPressed('f')){
                focusOffset[indexClicked] = originalValue + modVal;
            }
            ofLog() <<"Index: " << indexClicked << " Amount: "<< ofClamp(-amountMoved.y/(externalWindowRect.height/2), -1, 1) ;
        }else{
            float modVal = -amountMoved.x/(externalWindowRect.width/2);
            if(ofGetKeyPressed(OF_KEY_SHIFT)) modVal /= 2;
            if(ofGetKeyPressed('p')){
                panOffset[indexClicked] = originalValue + modVal;
            }else if(ofGetKeyPressed('t')){
                tiltOffset[indexClicked] = originalValue + modVal;
            }else if(ofGetKeyPressed('f')){
                focusOffset[indexClicked] = originalValue + modVal;
            }
            ofLog() <<"Index: " << indexClicked << " Amount: "<< ofClamp(amountMoved.x /(externalWindowRect.width/2), -1, 1);
        }
    }
}

void movingheadController::showExternalWindow(bool &b){
    ofxOceanodeNodeModelExternalWindow::showExternalWindow(b);
    recalculateSliders();
}
