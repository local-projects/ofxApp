#pragma once

#include "ofMain.h"
#include "ofxApp.h"
#include "CWRU_Object.h"
#include "CH_Object.h"

#define CWRU 	1
#define CH		2

/// change this between "CWRU" or "CH" for a different JSON / Parsing Example
#define JSON_SRC CH
/// you still need to change data/config/AppSettings.jon content:urls:jsonContentURL manually


#if (JSON_SRC == CWRU)
	#define USER_OBJECT CWRU_Object
#elif (JSON_SRC == CH)
	#define USER_OBJECT CH_Object
#endif



class ofApp : public ofBaseApp, public ofxAppDelegate{

public:
	
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	void setupChanged(ofxScreenSetup::ScreenSetupArg &arg);

	//ofxApp imposed - responder methods
	void startUserProcess(ofxApp::State);
	bool isUserProcessDone(ofxApp::State){return true;}
	void drawLoadingScreenForUserProcess(ofxApp::State, const ofRectangle & r){};
	float getProgressForUserProcess(ofxApp::State){return -1;}

	void contentIsReady(vector<ContentObject*>);

	vector<USER_OBJECT*> contentObjects;
};
