//
//  movingheadController.cpp
//  Espills_controller
//
//  Created by Eduard Frigola Bagué on 02/07/2018.
//

#include "movingheadController.h"

movingheadController::movingheadController() : ofxOceanodeNodeModelExternalWindow("Movinghead Controller"){
    numGroups = 3;
    size.resize(numGroups);
    intensity.resize(numGroups);
    pan.resize(numGroups);
    tilt.resize(numGroups);
//    colorDropdown.resize(numGroups);
//    colorwheel.resize(numGroups);
    strobe.resize(numGroups);
    gobo.resize(numGroups);
    frost.resize(numGroups);
    red.resize(numGroups);
    green.resize(numGroups);
    blue.resize(numGroups);
    dmxOutputs.resize(2);
    for(int i = 0 ; i < numGroups; i++){
        ofParameter<char> label;
        addParameter(label.set("Group " + ofToString(i) + "      ----------------------------------", ' '));
        addParameter(size[i].set("Size " + ofToString(i), {1}, {1}, {500}));
        addParameter(intensity[i].set("Intensity " + ofToString(i), {0}, {0}, {1}));
        addParameter(pan[i].set("Pan " + ofToString(i), {0}, {-180}, {180}));
        addParameter(tilt[i].set("Tilt " + ofToString(i), {0}, {-180}, {180}));
//        addParam(createDropdownAbstractParameter("Color " + ofToString(i), {"White", "Red", "Orange", "Yellow", "Green-Blue", "Amber", "Blue Sky", "Green", "UV", "Light Red", "Magenta", "Dark Green", "Dark Yellow", "Blue"}, colorDropdown[i]));
//        addParam(colorwheel[i].set("Color " + ofToString(i), {0}, {0}, {colorDropdown[i].getMax()}));
        addParameter(red[i].set("Red " + ofToString(i), {1}, {0}, {1}));
        addParameter(green[i].set("Green " + ofToString(i), {1}, {0}, {1}));
        addParameter(blue[i].set("Blue " + ofToString(i), {1}, {0}, {1}));
        addParameter(strobe[i].set("Zoom " + ofToString(i), 1, 0, 1));
        addParameter(gobo[i].set("Gobo " + ofToString(i), 0, 0, 1));
        addParameter(frost[i].set("Focus " + ofToString(i), 0, 0, 1));
        
//        dropdownListeners.push(colorDropdown[i].newListener([this, i](int &ind){
//            colorwheel[i] = vector<int>(1, ind);
//        }));
        sizeListeners.push(size[i].newListener([this](int &s){
            totalSize = 0;
            for(auto s : size) totalSize += s;
            loadCalibration();
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
    
    
    indexClicked = -1;
    isHorizontal = true;
    
    totalSize = 4;
    panRange = 540;
    tiltRange = 271;
}

void movingheadController::loadCalibration(){
    bool loaded = false;
    ofJson json = ofLoadJson("MovingHeadCalibration_" + ofToString(getNumIdentifier()) + ".json");
    if(!json.empty()){
        if(json["size"].get<int>() == totalSize){
            panOffset = json["panOffset"].get<vector<float>>();
            tiltOffset = json["tiltOffset"].get<vector<float>>();
            loaded = true;
        }
    }if(!loaded){
        panOffset = vector<float>(totalSize, 0);
        tiltOffset = vector<float>(totalSize, 0);
    }
}

void movingheadController::saveCalibration(){
    ofJson json;
    json["size"] = totalSize;
    json["panOffset"] = panOffset;
    json["tiltOffset"] = tiltOffset;
    ofSavePrettyJson("MovingHeadCalibration_" + ofToString(getNumIdentifier()) + ".json", json);
}

void movingheadController::update(ofEventArgs &a){
    int nChannels = 23;
    
    vector<vector<float>> dmxInfo;
    vector<float> panInfo;
    vector<float> tiltInfo;
    vector<float> colorInfo;
    
    dmxInfo.resize(totalSize, vector<float>(nChannels, 0));
    panInfo.resize(totalSize, .5);
    tiltInfo.resize(totalSize, .5);
    colorInfo.resize(totalSize*3, .5); //3 color components;
    
    for(int i = 0; i < numGroups; i++){
        int accumulateSizes = 0;
        for(int k = 0; k < i; k++){
            accumulateSizes += size[k];
        }
        for(int j = 0; j < size[i]; j++){
            int index = j + accumulateSizes;
            
            dmxInfo[index][0] = 30.0/255.0;
            
            
            
            //dimmer
            float dimmerAtIndex = getValueAtIndex(intensity[i].get(), j) * masterFader;
            dmxInfo[index][1] = dimmerAtIndex;
            
            dmxInfo[index][2] = 1 - getValueAtIndex(red[i].get(), j);
            dmxInfo[index][3] = 1 - getValueAtIndex(green[i].get(), j);
            dmxInfo[index][4] = 1 - getValueAtIndex(blue[i].get(), j);
            
            dmxInfo[index][16] = frost[i];
            dmxInfo[index][17] = frost[i]*255 - int(frost[i]*255);
            
            //pan
            float panAtIndex = getValueAtIndex(pan[i].get(), j);
            panInfo[index] = ofClamp(panAtIndex, -180, 180);
            panAtIndex = ofMap(panAtIndex, -panRange/2 + panOffset[index], panRange/2 + panOffset[index], 0, 1, true);
            dmxInfo[index][18] = panAtIndex;
            dmxInfo[index][19] = panAtIndex*255 - int(panAtIndex*255);
            
            //tilt
            float tiltAtIndex = getValueAtIndex(tilt[i].get(), j);
            tiltInfo[index] = ofClamp(tiltAtIndex, -180, 180);
            tiltAtIndex = ofMap(-tiltAtIndex, -tiltRange/2 + tiltOffset[index], tiltRange/2 + tiltOffset[index], 0, 1, true);
            dmxInfo[index][20] = tiltAtIndex;
            dmxInfo[index][21] = tiltAtIndex*255 - int(tiltAtIndex*255);
            
            
            //zoom
            dmxInfo[index][15] = ofMap(strobe[i], 0, 1, 0, 1);
            
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
            
            if(lampOn)
                dmxInfo[index][15] = 84.0f/255.0f;
            else if(lampOff){
                dmxInfo[index][15] = 75.0f/255.0f;
            }else if(reset){
                dmxInfo[index][15] = 10.0f/255.0f;
            }else{
                dmxInfo[index][15] = 0;
            }
            
            
            
            
            ofColor colorValue;
            switch (int(getValueAtIndex(colorwheel[i].get(), j))) {
                case 0:
                    colorValue = ofColor::white;
                    break;
                case 1:
                    colorValue = ofColor::red;
                    break;
                case 2:
                    colorValue = ofColor::orange;
                    break;
                case 3:
                    colorValue = ofColor::yellow;
                    break;
                case 4:
                    colorValue = ofColor::blueViolet;
                    break;
                case 5:
                    colorValue = ofColor(231, 194, 81);
                    break;
                case 6:
                    colorValue = ofColor::skyBlue;
                    break;
                case 7:
                    colorValue = ofColor::green;
                    break;
                case 8:
                    colorValue = ofColor(46, 48, 89);
                    break;
                case 9:
                    colorValue = ofColor::paleVioletRed;
                    break;
                case 10:
                    colorValue = ofColor::magenta;
                    break;
                case 11:
                    colorValue = ofColor::darkGreen;
                    break;
                case 12:
                    colorValue = ofColor::greenYellow;
                    break;
                case 13:
                    colorValue = ofColor::blue;
                    break;
                    
                    
                default:
                    colorValue = ofColor::white;
                    break;
            }
            */
            colorInfo[index*3] = getValueAtIndex(red[i].get(), j) * dimmerAtIndex;
            colorInfo[(index*3)+1] = getValueAtIndex(green[i].get(), j) * dimmerAtIndex;
            colorInfo[(index*3)+2] = getValueAtIndex(blue[i].get(), j) * dimmerAtIndex;
             
        }
    }
    
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
            }
            ofLog() <<"Index: " << indexClicked << " Amount: "<< ofClamp(-amountMoved.y/(externalWindowRect.height/2), -1, 1) ;
        }else{
            float modVal = -amountMoved.x/(externalWindowRect.width/2);
            if(ofGetKeyPressed(OF_KEY_SHIFT)) modVal /= 2;
            if(ofGetKeyPressed('p')){
                panOffset[indexClicked] = originalValue + modVal;
            }else if(ofGetKeyPressed('t')){
                tiltOffset[indexClicked] = originalValue + modVal;
            }
            ofLog() <<"Index: " << indexClicked << " Amount: "<< ofClamp(amountMoved.x /(externalWindowRect.width/2), -1, 1);
        }
    }
}

void movingheadController::showExternalWindow(bool &b){
    ofxOceanodeNodeModelExternalWindow::showExternalWindow(b);
    recalculateSliders();
}
