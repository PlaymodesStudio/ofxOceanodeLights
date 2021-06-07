//
//  e131Client.cpp
//  ofxOceanodeLights
//
//  Created by Eduard Frigola Bagué on 01/06/2021.
//

#include "e131Client.h"

void e131Client::setup(){
    addParameter(ip.set("IP", "192.168.1.1"), ofxOceanodeParameterFlags_DisableSavePreset | ofxOceanodeParameterFlags_DisableSaveProject);
	
	client.setup(ip, "255.255.255.0");
	client.setUseFramerate(false);
	client.setSourceName("Oceanode");
    
    int numInputs = 2;
	inputs.resize(numInputs);
	
	for(int i = 0; i < numInputs; i++){
		addParameter(inputs[i].set("Input " + ofToString(i), {}));
	}
}

void e131Client::update(ofEventArgs &a){
	for(auto &in : inputs){
		for(auto &fix : in.get()){
			auto data = fix.data;
			client.setChannels(fix.startChannel, data.data(), data.size(), fix.startUniverse);
		}
	}
    client.update();
}
