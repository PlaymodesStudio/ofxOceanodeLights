//
//  ofxOceanodeLights.h
//  
//
//  Created by Eduard Frigola Bagué on 17/06/2020.
//

#ifndef ofxOceanodeLights_h
#define ofxOceanodeLights_h

// Declarations only: the node headers and the bodies live in ofxOceanodeLights.cpp, so
// including this header does not compile every node of the addon again.

class ofxOceanode;

namespace ofxOceanodeLights{

void registerModels(ofxOceanode &o);
void registerCollection(ofxOceanode &o);

}

#endif /* ofxOceanodeLights_h */
