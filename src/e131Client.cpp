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


void artnodeFixtureSender::setup(){
	
	client.setup("2.0.0.1", "255.0.0.0");
    
    int numInputs = 3;
	inputs.resize(numInputs);
	
	for(int i = 0; i < numInputs; i++){
		addParameter(inputs[i].set("Input " + ofToString(i), {}));
	}
}

void artnodeFixtureSender::update(ofEventArgs &a){
	std::map<int, ArtDmx*> universes;
	for(auto &in : inputs){
		for(auto &fix : in.get()){
			auto data = fix.data;
			if(universes.count(fix.startUniverse) == 0){
				universes[fix.startUniverse] = client.createArtDmx(0, 0, 1);
			}
			for(int i = 0; i < data.size(); i++){
				universes[fix.startUniverse]->Data[fix.startChannel-1 + i] = data[i];
			}
		}
	}
	for(auto &uni : universes){
		client.sendUniCast("2.0.0.10", 6454, (char*)uni.second, uni.second->getSize());
	}
    client.update();
}
