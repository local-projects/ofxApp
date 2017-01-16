#pragma once

#include "ofMain.h"
#include "ofxApp.h"

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
	bool	ofxAppIsPhaseComplete(ofxApp::Phase){return true;}
	void	ofxAppDrawPhaseProgress(ofxApp::Phase, const ofRectangle & r){};
	float	ofxAppGetProgressForPhase(ofxApp::Phase){return -1;}

	void	ofxAppContentIsReady(const string & contentID, vector<ContentObject*>){};

};
