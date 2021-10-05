//
//  followPoint.c
//  Beams_Controlls
//
//  Created by Eduard Frigola on 07/09/2018.
//

#include "followPoint.h"
#include <random>

void followPoint::setup(){
	addParameter(reloadPositions.set("Reload"));
    addParameter(pointsX.set("Points X", {0}, {-22.5}, {22.5}));
    addParameter(pointsY.set("Points Y", {0}, {-10}, {100}));
    addParameter(pointsZ.set("Points Z", {0}, {-50}, {50}));
    addParameter(sortPoints.set("Sort Points", false));
    addParameter(randomizePoints.set("Randomize Points", false));
    
    addParameter(panL.set("Pan L", {0}, {-180}, {180}));
    addParameter(tiltL.set("Tilt L", {0}, {-180}, {180}));
    addParameter(panC.set("Pan C", {0}, {-180}, {180}));
    addParameter(tiltC.set("Tilt C", {0}, {-180}, {180}));
    addParameter(panR.set("Pan R", {0}, {-180}, {180}));
    addParameter(tiltR.set("Tilt R", {0}, {-180}, {180}));
    addParameter(distribution.set("Distribution", {0}, {0}, {9}));

	ofJson json = ofLoadJson("FollowPointPositions_" + ofToString(getNumIdentifier()) + ".json");

	movingHeadsL.resize(5);
	movingHeadsR.resize(5);
	movingHeadsC.resize(5);
	if (!json.empty()) {
		movingHeadsL[0].setPosition(json["L"]["0"]["x"], json["L"]["0"]["y"], json["L"]["0"]["z"]);
		movingHeadsL[1].setPosition(json["L"]["1"]["x"], json["L"]["1"]["y"], json["L"]["1"]["z"]);
		movingHeadsL[2].setPosition(json["L"]["2"]["x"], json["L"]["2"]["y"], json["L"]["2"]["z"]);
		movingHeadsL[3].setPosition(json["L"]["3"]["x"], json["L"]["3"]["y"], json["L"]["3"]["z"]);
		movingHeadsL[4].setPosition(json["L"]["4"]["x"], json["L"]["4"]["y"], json["L"]["4"]["z"]);

		movingHeadsR[0].setPosition(json["R"]["0"]["x"], json["R"]["0"]["y"], json["R"]["0"]["z"]);
		movingHeadsR[1].setPosition(json["R"]["1"]["x"], json["R"]["1"]["y"], json["R"]["1"]["z"]);
		movingHeadsR[2].setPosition(json["R"]["2"]["x"], json["R"]["2"]["y"], json["R"]["2"]["z"]);
		movingHeadsR[3].setPosition(json["R"]["3"]["x"], json["R"]["3"]["y"], json["R"]["3"]["z"]);
		movingHeadsR[4].setPosition(json["R"]["4"]["x"], json["R"]["4"]["y"], json["R"]["4"]["z"]);

		movingHeadsC[0].setPosition(json["C"]["0"]["z"], json["C"]["0"]["y"], json["C"]["0"]["x"]);
		movingHeadsC[1].setPosition(json["C"]["1"]["z"], json["C"]["1"]["y"], json["C"]["1"]["x"]);
		movingHeadsC[2].setPosition(json["C"]["2"]["z"], json["C"]["2"]["y"], json["C"]["2"]["x"]);
		movingHeadsC[3].setPosition(json["C"]["3"]["z"], json["C"]["3"]["y"], json["C"]["3"]["x"]);
		movingHeadsC[4].setPosition(json["C"]["4"]["z"], json["C"]["4"]["y"], json["C"]["4"]["x"]);
	}
	else {
		float height = 6.5;
		float x = 6.75; //12 entre un i altre o mes
		movingHeadsL[0].setPosition(-x, height, -1.48 * 4);
		movingHeadsL[1].setPosition(-x, height, -1.48 * 3);
		movingHeadsL[2].setPosition(-x, height, -1.48 * 2);
		movingHeadsL[3].setPosition(-x, height, -1.48);
		movingHeadsL[4].setPosition(-x, height, 0);

		movingHeadsR[0].setPosition(x, height, 0);
		movingHeadsR[1].setPosition(x, height, -1.48 * 1);
		movingHeadsR[2].setPosition(x, height, -1.48 * 2);
		movingHeadsR[3].setPosition(x, height, -1.48 * 3);
		movingHeadsR[4].setPosition(x, height, -1.48 * 4);

		height = 6.7;
		float distanceZ = -12.3;
		float distanceBetween = 0.70;
		float offset = -0.04;

		//	movingHeadsC[0].setPosition(-0.74*2, height, distanceZ);
		//	movingHeadsC[1].setPosition(-0.74, height, distanceZ);
		//	movingHeadsC[2].setPosition(0, height, distanceZ);
		//	movingHeadsC[3].setPosition(0.74, height, distanceZ);
		//	movingHeadsC[4].setPosition(0.74*2, height, distanceZ);
		movingHeadsC[0].setPosition(distanceZ, height, offset - distanceBetween * 2);
		movingHeadsC[1].setPosition(distanceZ, height, offset - distanceBetween * 1);
		movingHeadsC[2].setPosition(distanceZ, height, offset + distanceBetween * 0);
		movingHeadsC[3].setPosition(distanceZ, height, offset + distanceBetween * 1);
		movingHeadsC[4].setPosition(distanceZ, height, offset + distanceBetween * 2);
	}
    
   
   
    for(int i = 0; i < movingHeadsL.size(); i++){
        //movingHeadsL[i].setPosition(movingHeadsL[i].getPosition() - glm::vec3(21.04, 0, 20.88));
//		movingHeadsL[i].rotateDeg(180, glm::vec3(1, 0, 0));
        movingHeadsL[i].setScale(.1);
    }
    
    listeners.push(randomizePoints.newListener([this](bool &b){
        if(b && !lastRandomizePoints){
            ofRandomize(randomDist);
        }
        lastRandomizePoints = b;
	}));
	
	listeners.push(reloadPositions.newListener([this]() {
		ofJson json = ofLoadJson("FollowPointPositions_" + ofToString(getNumIdentifier()) + ".json");

		if (!json.empty()) {
			movingHeadsL[0].setPosition(json["L"]["0"]["x"], json["L"]["0"]["y"], json["L"]["0"]["z"]);
			movingHeadsL[1].setPosition(json["L"]["1"]["x"], json["L"]["1"]["y"], json["L"]["1"]["z"]);
			movingHeadsL[2].setPosition(json["L"]["2"]["x"], json["L"]["2"]["y"], json["L"]["2"]["z"]);
			movingHeadsL[3].setPosition(json["L"]["3"]["x"], json["L"]["3"]["y"], json["L"]["3"]["z"]);
			movingHeadsL[4].setPosition(json["L"]["4"]["x"], json["L"]["4"]["y"], json["L"]["4"]["z"]);

			movingHeadsR[0].setPosition(json["R"]["0"]["x"], json["R"]["0"]["y"], json["R"]["0"]["z"]);
			movingHeadsR[1].setPosition(json["R"]["1"]["x"], json["R"]["1"]["y"], json["R"]["1"]["z"]);
			movingHeadsR[2].setPosition(json["R"]["2"]["x"], json["R"]["2"]["y"], json["R"]["2"]["z"]);
			movingHeadsR[3].setPosition(json["R"]["3"]["x"], json["R"]["3"]["y"], json["R"]["3"]["z"]);
			movingHeadsR[4].setPosition(json["R"]["4"]["x"], json["R"]["4"]["y"], json["R"]["4"]["z"]);

			movingHeadsC[0].setPosition(json["C"]["0"]["z"], json["C"]["0"]["y"], json["C"]["0"]["x"]);
			movingHeadsC[1].setPosition(json["C"]["1"]["z"], json["C"]["1"]["y"], json["C"]["1"]["x"]);
			movingHeadsC[2].setPosition(json["C"]["2"]["z"], json["C"]["2"]["y"], json["C"]["2"]["x"]);
			movingHeadsC[3].setPosition(json["C"]["3"]["z"], json["C"]["3"]["y"], json["C"]["3"]["x"]);
			movingHeadsC[4].setPosition(json["C"]["4"]["z"], json["C"]["4"]["y"], json["C"]["4"]["x"]);
		}
	}));
	
	
	for(int i = 0; i < movingHeadsL.size(); i++){
		//movingHeadsR[i].setPosition(movingHeadsR[i].getPosition() - glm::vec3(21.04, 0, 20.88));
		//		movingHeadsR[i].rotateDeg(180, glm::vec3(1, 0, 0));
		//		movingHeadsR[i].rotateDeg(180, glm::vec3(0, 1, 0));
		movingHeadsR[i].setScale(.1);
	}
	
	//height = 6.7;
	//float distanceZ = -12.4;
	//float distanceBetween = 0.87;
	
	
	for(int i = 0; i < movingHeadsC.size(); i++){
		//movingHeadsR[i].setPosition(movingHeadsR[i].getPosition() - glm::vec3(21.04, 0, 20.88));
		//		movingHeadsR[i].rotateDeg(180, glm::vec3(1, 0, 0));
		//		movingHeadsR[i].rotateDeg(180, glm::vec3(0, 1, 0));
		movingHeadsC[i].setScale(.1);
	}
	
	//center 74 entre ells
	
	lastSize = 0;
	randomDist.resize(10);
	iota(randomDist.begin(), randomDist.end(), 0);
}

void followPoint::update(ofEventArgs &a){
	int maxSize = max(max(pointsX->size(), pointsY->size()), pointsZ->size());
	bool manualDist = getOceanodeParameter(distribution).hasInConnection();
	
	if(maxSize != lastSize && !manualDist){
		if(randomizePoints) ofRandomize(randomDist);
	}
	lastSize = maxSize;
	
	auto getValForIndex = [this](const vector<float> &vf, int index)->float{
		if(vf.size() == 1 || vf.size() <= index){
			return vf[0];
		}
		else{
			return vf[index];
		}
	};
	
	points.resize(maxSize);
	for(int i = 0; i < points.size(); i++){
		auto &point =  points[i];
		point = glm::vec3(getValForIndex(pointsX.get(), i), getValForIndex(pointsY.get(), i), getValForIndex(pointsZ.get(), i));
		//        point.x = ofMap(point.x, 0, 1, -22.5, 22.5);
		//        point.y = ofMap(point.y, 0, 1, 0, 100);
		//        point.z = ofMap(point.z, 0, 1, -50, 50);
	}
	
	if(sortPoints){
		std::sort(points.begin(), points.end(), [](glm::vec3 p1, glm::vec3 p2){return p1.z > p2.z;});
	}
	
	vector<int> tempDistribution(15);
	
	vector<float> tempPans(5);
	vector<float> tempTilts(5);
	if(manualDist && distribution->size() == 15) tempDistribution = distribution;
	else{
		for(int i = 0; i < movingHeadsL.size(); i++){
			int pointPos = 0;
			if(points.size() < 5){
				pointPos = floor(i / (5.0 / points.size()));
			}else{
				pointPos = i;
			}
			tempDistribution[i] = pointPos;
		}
		for(int i = 0; i < movingHeadsR.size(); i++){
			int pointPos = 0;
			if(points.size() <= 5){
				pointPos = floor((i+0.5) / (5.0 / points.size()));
			}else{
				pointPos = (i+5) % points.size();
			}
			tempDistribution[i+5] = pointPos;
		}
		for(int i = 0; i < movingHeadsC.size(); i++){
			int pointPos = 0;
			if(points.size() <= 5){
				pointPos = floor((i+0.5) / (5.0 / points.size()));
			}else{
				pointPos = (i+10) % points.size();
			}
			tempDistribution[i+10] = pointPos;
		}
		
		if(randomizePoints){
			vector<int> distributionCopy = tempDistribution;
			for(int i = 0; i < distributionCopy.size(); i++){
				tempDistribution[i] = distributionCopy[randomDist[i]];
			}
		}
	}
    
    //---------
    for(int i = 0; i < movingHeadsL.size(); i++){
        //cout<<pointPos<<endl;
        glm::vec2 orientInfo = calculateAlfa(movingHeadsL[i].getPosition(), points[tempDistribution[i]], 0);
        float tilt = orientInfo.x;
        float pan = orientInfo.y;
        //            if(pan < 0){
        //                tilt = - tilt;
        //            }
        //pan = pan+90;
        //            if(i == 0) ofLog() << "Pan:" << pan << " Tilt:" << tilt;
        tempPans[i] = pan;
        tempTilts[i] = tilt+90;
    }
    panL = tempPans;
    tiltL = tempTilts;
    for(int i = 0; i < movingHeadsR.size(); i++){
        glm::vec2 orientInfo = calculateAlfa(movingHeadsR[i].getPosition(), points[ tempDistribution[i+5]], 0);
        float tilt = orientInfo.x;
        float pan = orientInfo.y;
        //            if(pan < 0){
        //                tilt = - tilt;
        //            }
        //pan = pan+90;
        tempPans[i] = -pan;
        tempTilts[i] = tilt+90;
    }
    //cout<<"---------------------"<<endl;
    panR = tempPans;
    tiltR = tempTilts;
	for(int i = 0; i < movingHeadsC.size(); i++){
		glm::vec3 rotatedPoint = points[tempDistribution[i+10]];
		rotatedPoint = glm::vec3(-rotatedPoint.z, rotatedPoint.y, rotatedPoint.x);
        glm::vec2 orientInfo = calculateAlfa(movingHeadsC[i].getPosition(), rotatedPoint, 0);
        float tilt = orientInfo.x;
        float pan = orientInfo.y;
        //            if(pan < 0){
        //                tilt = - tilt;
        //            }
        //pan = pan+90;
        tempPans[i] = pan;
        tempTilts[i] = tilt+90;
    }
	panC = tempPans;
    tiltC = tempTilts;
	
	if(!manualDist){
		distribution = tempDistribution;
	}
//        ofLog() << calculateAlfa(movingHeadsL[0].getPosition(), points[0], 0);

}

void followPoint::drawInExternalWindow(ofEventArgs &a){
    cam.begin();
    ofSetColor(255, 0, 0, 127);
    for(auto &head : movingHeadsL){
        head.draw();
    }
    for(auto &head : movingHeadsR){
        head.draw();
    }
//    auto polar = calculateAlfa(movingHeadsL[0].getPosition(), points[0], 0);
//    polar = glm::vec2(ofDegToRad(polar.x), ofDegToRad(polar.y));
////    ofLog() << polar.x << "---" << glm::distance(movingHeadsL[0].getPosition(), points[0]) * sin(polar.y);
//    glm::vec3 direction = glm::vec3(sin(polar.y) * cos(polar.x), sin(polar.y) * sin(polar.x), cos(polar.y));
////    glm::vec3 direction = glm::vec3(sin(polar.y), sin(polar.x), cos(90-polar.y));
//    ofDrawLine(movingHeadsL[0].getPosition(), movingHeadsL[0].getPosition() + 20 * direction);
//    movingHeadsL[0].draw();
    for(int i = 0; i < points.size(); i++){
        ofFill();
        ofDrawSphere(points[i], 1);
    }
    cam.end();
    
    ofRectangle vp = ofGetCurrentViewport();
    float r = std::min<float>(vp.width, vp.height) * 0.5f;
    float x = vp.width * 0.5f;
    float y = vp.height * 0.5f;
    
    ofPushStyle();
    ofSetLineWidth(3);
    ofSetColor(255, 255, 0);
    ofNoFill();
    glDepthMask(false);
    ofDrawCircle(x, y, r);
    glDepthMask(true);
    ofPopStyle();
}

void followPoint::showExternalWindow(bool &b){
    ofxOceanodeNodeModelExternalWindow::showExternalWindow(b);
    if(b){
        cam.setEvents(externalWindow->events());
        cam.enableMouseInput();
    }else{
        cam.disableMouseInput();
    }
}

void followPoint::keyPressed(ofKeyEventArgs &a){
    switch(a.key) {
        case ' ':
            cam.getOrtho() ? cam.disableOrtho() : cam.enableOrtho();
            break;
        case 'C':
        case 'c':
            cam.getMouseInputEnabled() ? cam.disableMouseInput() : cam.enableMouseInput();
            break;
        case 'F':
        case 'f':
            ofToggleFullscreen();
            break;
        case 'H':
        case 'h':
            //bHelpText ^=true;
            break;
        case 'I':
        case 'i':
            cam.getInertiaEnabled() ? cam.disableInertia() : cam.enableInertia();
            break;
        case 'Y':
        case 'y':
            cam.setRelativeYAxis(!cam.getRelativeYAxis());
            break;
    }
}

glm::vec2 followPoint::calculateAlfa(glm::vec3 position, glm::vec3 lookAT, bool orientMode){
    glm::vec2 result;
    glm::vec3 myVector;
    
    myVector = lookAT-position;
	
	float R1 = sqrt((myVector.x*myVector.x) + (myVector.z*myVector.z));
	float R2 = sqrt((R1*R1) + (myVector.y*myVector.y));
	
	if(orientMode==0)
    {
		result.y = -ofRadToDeg(asin((myVector.z/R1)));
		result.x = ofRadToDeg(asin(-myVector.y/R2));
    }
    else
    {
        result.y = ofRadToDeg(asin((myVector.z/R1)));
        result.x = ofRadToDeg(asin(-myVector.y/R2));
    }
    
    
//    result.x = ofRadToDeg(atan(myVector.y/myVector.x));
//    result.y = ofRadToDeg(acos(myVector.z/(sqrt(pow(myVector.x, 2)+pow(myVector.y, 2)+pow(myVector.z, 2)))));
    
    return result;
}
