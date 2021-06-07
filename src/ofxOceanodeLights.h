//
//  ofxOceanodeLights.h
//  
//
//  Created by Eduard Frigola Bagué on 17/06/2020.
//

#ifndef ofxOceanodeLights_h
#define ofxOceanodeLights_h


#include "artnetSender.h"
#include "colorGradient.h"
#include "followIndexsToMovingHeads.h"
#include "followPoint.h"
#include "movingheadController.h"
#include "oscSenders.h"
#include "pantiltEditor.h"
#include "strobeLightController.h"
#include "e131Client.h"
#include "fixture.h"

#include "ofxOceanode.h"

namespace ofxOceanodeLights{
static void registerModels(ofxOceanode &o){
    o.registerModel<artnetSender>("Lights"); 
    o.registerModel<colorGradient>("Lights"); 
    o.registerModel<followIndexsToMovingHeads>("Lights");
    o.registerModel<followPoint>("Lights");
    o.registerModel<movingheadController>("Lights");
    o.registerModel<oscSenders>("Lights");
    o.registerModel<panTiltEditor>("Lights");
    o.registerModel<strobeLightController>("Lights");
	o.registerModel<e131Client>("Lights");
	
	o.registerType<vector<fixture>>("Fixtures");
}
}

#endif /* ofxOceanodeLights_h */
