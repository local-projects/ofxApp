#pragma once

#include "ofMain.h"
#include "ofxApp.h"
#include "CWRU_Object.h"
#include "CH_Object.h"
#include "ofxInterface.h"
#include "TexturedObjectScrollView.h"

#define FAKE_LOAD_SCREEN_DURATION 0.2

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

	string	ofxAppWillFetchContentFromURL(const std::string & contentID, const std::string & jsonURL);

	void	ofxAppPhaseWillBegin(ofxApp::Phase);
	bool	ofxAppIsPhaseComplete(ofxApp::Phase);

	void	ofxAppDrawPhaseProgress(ofxApp::Phase, const ofRectangle & r);
	string 	ofxAppGetStatusString(ofxApp::Phase);
	float	ofxAppGetProgressForPhase(ofxApp::Phase);

	//at 1st launch, we get all the content b4 the app start
	void	ofxAppContentIsReady(const string & contentID, vector<ContentObject*>);

	//if live Updates are enabled, you will periodically also get an updated list of content objects
	void	ofxAppContentUpdate(const std::string & contentID, vector<ContentObject*>);
	void	ofxAppContentUpdateFailed(const std::string & contentID, const std::string & errorMsg);

	void	screenSetupChanged(ofxScreenSetup::ScreenSetupArg &arg){}

	//multitouch callbacks
	void tuioAdded(ofxTuioCursor & tuioCursor){};
	void tuioRemoved(ofxTuioCursor & tuioCursor){};
	void tuioUpdated(ofxTuioCursor & tuioCursor){};

	// Content delivered by ofxApp ////////////////////////////////////////////

	vector<CH_Object*>		chObjects;
	vector<CWRU_Object*>	cwruObjects;

	// Scrollview /////////////////////////////////////////////////////////////

	void setupScene();

	void setupScrollViews();
	void deleteScrollViews();
	ofxInterface::Node * scene;

	TexturedObjectScrollView * scrollView;
	TexturedObject * selectedObject = nullptr;

	void onSrollImageClicked(TexturedObjectScrollView::TouchedImage &);
	void onDrawTile(TexturedObjectScrollView::DrawTileInfo &);

	float phaseStartTime;

};
