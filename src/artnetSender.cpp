//
//  artnetSender.cpp
//  artnetOceanodeModule
//
//  Created by Eduard Frigola Bagué on 13/04/2018.
//

#include "artnetSender.h"

artnetSender::artnetSender() : ofxOceanodeNodeModel("Artnet Sender"){
    manualNodes = false;
}

artnetSender::~artnetSender(){
    
}

void artnetSender::setup(){
    artnet.setup(ofxArtNode::getInterfaceAddr(0), "255.255.255.0");
    eventListeners.push(artnet.nodeAdded.newListener(this, &artnetSender::nodeAdded));
    eventListeners.push(artnet.nodeErased.newListener(this, &artnetSender::nodeErased));
    
    nodeOptions.clear();
    nodeOptionStructs.clear();
    nodeOptions.push_back("None");
    
    loadManualNodes();
    
    addParameter(ip.set("IP", ofxArtNode::getInterfaceAddr(0)), ofxOceanodeParameterFlags_DisableSavePreset | ofxOceanodeParameterFlags_DisableSaveProject);
    addParameter(pollButton.set("Poll Devices"));
    
    int numInputs = 4;
    isPollPerIndex.resize(4, false);
    
    for(int i = 0; i < numInputs; i++){
        universeMap[i] = ofParameter<int>();
        addParameterDropdown(universeMap[i], "Output " + ofToString(i), 0, nodeOptions, ofxOceanodeParameterFlags_DisableSavePreset);
        inputMap[i] = ofParameter<vector<float>>();
        addParameter(inputMap[i].set("Input " + ofToString(i), {-1}, {0}, {1}), ofxOceanodeParameterFlags_DisableSavePreset);
        ifNewCreatedChecker[i] = true;
    }
    for(int i = 0; i < numInputs; i++){
        listeners[i] = inputMap[i].newListener([&, i](vector<float> &val){
            inputListener(i);
        });
    }
    
    sendPoll();
    
    eventListeners.push(pollButton.newListener(this, &artnetSender::sendPoll));
    eventListeners.push(ip.newListener([this](string &s){
        if(ofSplitString(s, ".").size() == 4){
            artnet.setup(s, "255.255.255.0");
        }
    }));
}

void artnetSender::update(ofEventArgs &a){
    artnet.update();
}

void artnetSender::inputListener(int index){
    if(isPollPerIndex[index]){
        if(getParameterGroup().contains("Output " + ofToString(index))){
            string optionsString;
            for(auto opt : nodeOptions){
                optionsString += opt + "-|-";
            }
            optionsString.erase(optionsString.end()-3, optionsString.end());
            string name = "Output " + ofToString(index);
            auto param = static_cast<ofxOceanodeAbstractParameter&>(getParameterGroup().get(name)).cast<int>();
            param.setDropdownOptions(nodeOptions);
            //.getString(0).set(optionsString);
            getParameter<int>(name).setMax(nodeOptions.size()-1);
        }
        isPollPerIndex[index] = false;
    }
    if(inputMap[index].get()[0] != -1){
        if(universeMap[index] != 0 && universeMap[index] < (nodeOptionStructs.size()+1)){
            nodeOptionStruct option = nodeOptionStructs[universeMap[index]-1];
            ArtDmx * dmx = artnet.createArtDmx(0, option.subnet, option.universe);
            for(int i = 0; i < inputMap[index].get().size(); i++){
                dmx->Data[i] = inputMap[index]->at(i)  * 255;
            }
            artnet.sendUniCast(option.ip, 6454, (char*)dmx, dmx->getSize());
        }
    }
}

void artnetSender::sendPoll(){
    artnet.sendPoll();
};

void artnetSender::nodeAdded(ofxArtNode::NodeEntry &node){
    if(manualNodes){
        nodeOptionStructs.clear();
        nodeOptions.clear();
        nodeOptions.push_back("None");
        manualNodes = false;
    }
    int subnet = int(node.pollReply.SubSwitch);
    cout<<"NODE FOUND! " << "IP: " <<node.address << " - Subnet: "<< subnet <<" Universes: -" ;
    for(int i = 0; i < node.pollReply.NumPortsLo; i++){
        int universe = int(node.pollReply.SwOut[i]);
        nodeOptionStructs.push_back(nodeOptionStruct(subnet, universe, node.address));
        nodeOptions.push_back("Sub:" + ofToString(subnet) + " Univ:" + ofToString(universe) + " IP:" + node.address);
        cout<<universe<<"-";
    }
    std::fill(isPollPerIndex.begin(), isPollPerIndex.end(), true);
    cout<<endl;
}

void artnetSender::nodeErased(ofxArtNode::NodeEntry &node){
    int subnet = int(node.pollReply.SubSwitch);
    cout<<"NODE Erase! " << "IP: " <<node.address << " - Subnet: "<< subnet <<" Universes: -" ;
    for(int i = 0; i < node.pollReply.NumPortsLo; i++){
        int universe = int(node.pollReply.SwOut[i]);
        for(int j = 0; j < nodeOptionStructs.size();){
            if(nodeOptionStructs[j].ip == node.address && nodeOptionStructs[j].subnet == subnet && nodeOptionStructs[j].universe == universe){
                nodeOptionStructs.erase(nodeOptionStructs.begin()+j);
                nodeOptions.erase(nodeOptions.begin()+j+1);
            }else{
                j++;
            }
        }
    }
    cout<<endl;
    if(nodeOptionStructs.size() == 0){
        loadManualNodes();
    }
    std::fill(isPollPerIndex.begin(), isPollPerIndex.end(), true);
}

void artnetSender::loadManualNodes(){
    manualNodes = true;
    ofJson json = ofLoadJson("ArtnetManualNodes.json");
    if(!json.empty()){
        for (ofJson::iterator node = json.begin(); node != json.end(); ++node) {
            string address = ofSplitString(node.key(), "-")[0];
            string subnet = ofSplitString(node.key(), "-")[1];
            cout<<"ManualNode! " << "IP: " << address <<  " - Subnet: "<< subnet << " Universes: -" ;
            for (auto universe : node.value()) {
                nodeOptionStructs.push_back(nodeOptionStruct(ofToInt(subnet), universe, address));
                nodeOptions.push_back("Sub:" + subnet + " Univ:" + ofToString(universe) + " IP:" + address);
                cout<<universe<<"-";
            }
            cout<< endl;
        }
    }
    
}
