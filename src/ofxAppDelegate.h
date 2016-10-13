//
//  HasRuiParams.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxAppStructs.h"
#include "ofxTuio.h"


//to use ofxApp, your app must follow this protocol, so make sure your app
//is a subclass of ofxAppDelegate

class ofxAppDelegate{

public:

	// your app will be given a chance to run 4 custom tasks during startup:
	// 1 - To run code just b4 content loading starts (ofxApp::UserAppSetupStage == SETUP_B4_CONTENT_LOAD)
	// 2 - To process the loaded content (ofxApp::State == RECEIVE_CONTENT_LOAD_RESULTS)
	// 3 - To set things up once content has been received (ofxApp::State == SETUP_AFTER_CONTENT_LOAD)
	// 4 - Once last callback just b4 the app starts Running (ofxApp::State == SETUP_JUST_B4_RUNNING)
	//
	// your ofxAppStartUserPhase(ofxApp::UserAppSetupStage) will get called when this happens;
	//
	// You can either run your code directly in the callback, or thread it and
	// make the loading process more friendly by using, ofxAppIsUserPhaseComplete()
	// ofxAppGetProgressForUserPhase() and ofxAppDrawPhaseProgressScreen()
	// letting the app update while the loading happens; ofxApp will show a loading screen
	// with progressbar & such, and you can drive what to draw on screen while that happens.
	// just return true in ofxAppIsUserPhaseComplete() when the stage is concluded for ofxApp to proceed
	// to the next state.

	virtual void ofxAppStartUserPhase(ofxApp::UserAppSetupStage){ ofLogError("ofxApp") << "ofxAppStartUserPhase() not implemented!"; }; //this will be your entry point to start loading stuff
	//after u are asked to start loading content, ofxApp will query every frame to check if you are done
	virtual bool ofxAppIsUserPhaseComplete(ofxApp::UserAppSetupStage){return true;} //your APP should override this method if you are loading custom content
	virtual void ofxAppDrawPhaseProgressScreen(ofxApp::UserAppSetupStage, const ofRectangle & r){}; //your APP can override the loading screen drawing
	virtual float ofxAppGetProgressForUserPhase(ofxApp::UserAppSetupStage){return -1;} //your APP should return [0..1] to report progressbar; -1 for indeterminate

	//this is how your app gets all the parsed objects - up to you how you store them
	virtual void ofxAppContentIsReady(const string & contentID, vector<ContentObject*>){};


	//tuio callbacks
	virtual void tuioAdded(ofxTuioCursor & tuioCursor){};
	virtual void tuioRemoved(ofxTuioCursor & tuioCursor){};
	virtual void tuioUpdated(ofxTuioCursor & tuioCursor){};

};