#pragma once

#include "ofMain.h"
#include "ofxApp.h"
#include "CWRU_Object.h"
#include "CH_Object.h"
#include "ofxInterface.h"
#include "TexturedObjectScrollView.h"

#define FAKE_LOAD_SCREEN_DURATION 2.5

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

	// ofxAppDelegate Callbacks /////////////////////////////////////////////////

	void	ofxAppPhaseWillBegin(ofxApp::Phase);
	bool	ofxAppIsPhaseComplete(ofxApp::Phase);

	void	ofxAppDrawPhaseProgress(ofxApp::Phase, const ofRectangle & r);
	string 	ofxAppGetStatusString(ofxApp::Phase);
	float	ofxAppGetProgressForPhase(ofxApp::Phase);

	void	ofxAppContentIsReady(const string & contentID, vector<ContentObject*>);

	void	screenSetupChanged(ofxScreenSetup::ScreenSetupArg &arg){}

	// Content delivered by ofxApp ////////////////////////////////////////////

	vector<CH_Object*>		chObjects;
	vector<CWRU_Object*>	cwruObjects;

	// Scrollview /////////////////////////////////////////////////////////////

	void setupScrollViews();
	ofxInterface::Node * scene;

	TexturedObjectScrollView * scrollView;
	TexturedObject * selectedObject = nullptr;

	void onSrollImageClicked(TexturedObjectScrollView::TouchedImage &);
	void onDrawTile(TexturedObjectScrollView::DrawTileInfo &);

	float phaseStartTime;

};
