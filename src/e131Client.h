//
//  e131Client.h
//  ofxOceanodeLights
//
//  Created by Eduard Frigola Bagué on 01/06/2021.
//

#ifndef e131Client_h
#define e131Client_h

#include "ofxOceanodeNodeModel.h"
#include "ofxE131Client.h"
#include "fixture.h"

class e131Client : public ofxOceanodeNodeModel{
public:
	e131Client() : ofxOceanodeNodeModel("E131 Sender"){};
	~e131Client(){};
    
    void setup() override;
    void update(ofEventArgs &a) override;
    
private:
    void inputListener(int index);
    
    ofEventListeners eventListeners;
    
    ofParameter<string> ip;
	vector<ofParameter<vector<fixture>>> inputs;
	
	ofxE131Client client;
	
};

#endif /* e131Client_h */

