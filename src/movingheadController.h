//
//  movingheadController.hpp
//  Espills_controller
//
//  Created by Eduard Frigola Bagué on 02/07/2018.
//

#ifndef movingheadController_h
#define movingheadController_h

#include "ofxOceanodeNodeModelExternalWindow.h"
#include "fixture.h"

class movingheadController : public ofxOceanodeNodeModelExternalWindow{
public:
    movingheadController();
    ~movingheadController(){
         if(getNumIdentifier() != -1){
             saveCalibration();
         }
    };
    
	void setup() override;
    
    void update(ofEventArgs &a) override;
    
private:
    
    void saveCalibration();
    void loadCalibration();

	void loadDMXChannels();
    
    void drawInExternalWindow(ofEventArgs &e) override;
    void windowResized(ofResizeEventArgs &a) override;
    void keyPressed(ofKeyEventArgs &a) override;
    void keyReleased(ofKeyEventArgs &a) override;
    
    void mousePressed(ofMouseEventArgs &a) override;
    void mouseReleased(ofMouseEventArgs &a) override;
    void mouseDragged(ofMouseEventArgs &a) override;
    void showExternalWindow(bool &b);
    
    void recalculateSliders();
    
    template<typename T>
    float getValueAtIndex(const vector<T> &v, int index){
        if(v.size() == 1 || v.size() <= index){
            return v[0];
        }
        else{
            return v[index];
        }
    }
    
    ofEventListeners dropdownListeners;
    ofEventListeners sizeListeners;
    
    int numGroups;
    vector<ofParameter<int>>    size;
    vector<ofParameter<vector<float>>> pan;
    vector<ofParameter<vector<float>>> tilt;
    vector<ofParameter<vector<float>>> intensity;
    vector<ofParameter<vector<int>>> colorwheel;
//    vector<ofParameter<vector<float>>> red;
//    vector<ofParameter<vector<float>>> green;
//    vector<ofParameter<vector<float>>> blue;
    vector<ofParameter<int>> colorDropdown;
    vector<ofParameter<float>> strobe;
    vector<ofParameter<vector<float>>> gobo;
    vector<ofParameter<vector<float>>> frost;
    ofParameter<float> masterFader;
    vector<ofParameter<vector<float>>> dmxOutputs;
    ofParameter<vector<float>> panOutput;
    ofParameter<vector<float>> tiltOutput;
    ofParameter<vector<float>> colorOutput;
    ofParameter<bool> lampOn;
    ofParameter<bool> lampOff;
    ofParameter<bool> reset;
	
	ofParameter<vector<fixture>> output;
    
    int totalSize;
    
    vector<float> panOffset;
    vector<float> tiltOffset;
	vector<float> focusOffset;
    
    float originalValue;
    vector<ofRectangle> points;
    int indexClicked;
    bool isHorizontal;
    glm::vec2 initialClicPos;
    
    int panRange;
    int tiltRange;
	
	
	std::vector<std::vector<int>> channels;
};

#endif /* movingheadController_h */
