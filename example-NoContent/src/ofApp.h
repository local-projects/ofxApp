#pragma once

#include "ofMain.h"
#include "ofxApp.h"

#define FAKE_LOAD_SCREEN_DURATION 1.0

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

	//ofxApp imposed - responder methods
	void	ofxAppPhaseWillBegin(ofxApp::Phase);
	bool	ofxAppIsPhaseComplete(ofxApp::Phase);

	void	ofxAppDrawPhaseProgress(ofxApp::Phase, const ofRectangle & r);
	string 	ofxAppGetStatusString(ofxApp::Phase); //short status on the progress bar
	string 	ofxAppGetLogString(ofxApp::Phase); //long status (ie log / script output) above the progress bar
	float	ofxAppGetProgressForPhase(ofxApp::Phase);

	void	ofxAppContentIsReady(const string & contentID, const vector<ContentObject*> &){};

	float phaseStartTime;
};
