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
#include "oscSender.h"
#include "pantiltEditor.h"
#include "strobeLightController.h"

#include "ofxOceanode.h"

namespace ofxOceanodeLights{
static void registerModels(ofxOceanode &o){
    o.registerModel<artnetSender>("Lights"); 
    o.registerModel<colorGradient>("Lights"); 
    o.registerModel<followIndexsToMovingHeads>("Lights");
    o.registerModel<followPoint>("Lights");
    o.registerModel<movingheadController>("Lights");
    o.registerModel<oscSender>("Lights");
    o.registerModel<panTiltEditor>("Lights");
    o.registerModel<strobeLightController>("Lights");
}
}

#endif /* ofxOceanodeLights_h */
