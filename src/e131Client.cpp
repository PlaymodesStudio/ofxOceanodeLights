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

void artnodeFixtureSender::setup() {

    ofJson json = ofLoadJson("ArtnetConfig.json");

    addParameter(ip.set("Dest IP", "2.0.0.10"));
    string interface = "10.0.0.1";
    string interfaceMask = "255.0.0.0";
    if (json.count("interface") == 1) interface = json["interface"].get<string>();
    if (json.count("interfaceMask") == 1) interfaceMask = json["interfaceMask"].get<string>();
    if (json.count("destIP") == 1) configip = json["destIP"];
    
    if(configip != "0.0.0.0"){
        ip = configip;
    }

    client.setup(interface, interfaceMask);

    int numInputs = 4;
    inputs.resize(numInputs);

    for (int i = 0; i < numInputs; i++) {
        addParameter(inputs[i].set("Input " + ofToString(i), {}));
    }
}

void artnodeFixtureSender::update(ofEventArgs &a) {
    ArtDmx* universe = client.createArtDmx(0, 0, 0);
    map<int, vector<char>> universesData;
    //map<int, ArtDmx*> universes;
    int currentUniv = 0;
    for (auto &in : inputs) {
        for (auto &fix : in.get()) {
            auto data = fix.data;
            int startChan = fix.startChannel;
            int startUniv = fix.startUniverse;
            while (data.size() != 0) {
                int dataCopySize = std::min(511 - startChan, (int)data.size());
                vector<u_char> datacopy(data.begin(), data.begin() + dataCopySize);
                //e131client.setChannels(startChan, datacopy.data(), datacopy.size(), startUniv);
                
                if (universesData.count(startUniv) == 0) {
                    //ofLog() << "createUniverse " << startUniv;
                    //universes[startUniv] = client.createArtDmx(0, startUniv >> 4, startUniv & 0x0f);
                    universesData[startUniv].resize(510);
                }
                
                /*if (currentUniv != startUniv) {
                    client.sendUniCast(ip, 6454, (char*)universe, universe->getSize());
                    universe = client.createArtDmx(0, startUniv >> 4, startUniv & 0x0f);
                }*/

                for (int i = 0; i < datacopy.size(); i++) {
                    //universes[startUniv]->Data[startChan - 1 + i] = datacopy[i];
                    universesData[startUniv][startChan - 1 + i] = datacopy[i];
                }
                //client.sendUniCast(ip, 6454, (char*)universes[startUniv], universes[startUniv]->getSize());
                

                /*for (int i = 0; i < datacopy.size(); i++) {
                    universe->Data[startChan - 1 + i] = datacopy[i];
                }*/

                data = vector<u_char>(data.begin() + dataCopySize, data.end());
                startChan = 1;
                startUniv++;
            }





            //auto data = fix.data;
            //if (universes.count(fix.startUniverse) == 0) {
            //    universes[fix.startUniverse] = client.createArtDmx(0, 0, 1);
            //}
            //for (int i = 0; i < data.size(); i++) {
            //    universes[fix.startUniverse]->Data[fix.startChannel - 1 + i] = data[i];
            //}
        }
    }
    
    for (auto &uni : universesData) {
        auto artdmx = client.createArtDmx(0, uni.first >> 4, uni.first & 0x0f);
        for (int i = 0; i < uni.second.size(); i++) {
            //universes[startUniv]->Data[startChan - 1 + i] = datacopy[i];
            artdmx->Data[i] = uni.second[i];
        }
        client.sendUniCast(ip, 6454, (char*)artdmx, artdmx->getSize());
    }
    //client.update();
}
