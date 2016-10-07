//
//  HasRuiParams.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxAppStructs.h"
#include "ofxThreadSafeLog.h"
#include "ofxSuperLog.h"
#include "ofxSimpleHttp.h"
#include "ofxTuio.h"

class HasRuiParams{
public:
	virtual void setupRemoteUIParams() = 0;
};


namespace ofxApp{
	
	static void terminateApp(const string & module, const string & reason, float secondsOnScreen = 15){
		ofLogFatalError("ofxApp") << "terminateApp()!";
		ofxSimpleHttp::destroySslContext();
		ofLogFatalError("ofxApp") << "";
		ofLogFatalError("ofxApp") << "-----------------------------------------------------------------------------------------------------------";
		ofLogFatalError("ofxApp") << "";
		ofLogFatalError("ofxApp") << "ofxApp is terminating because the module \"" << module << "\" found an unrecoverable error.";
		ofLogFatalError("ofxApp") << "\"" << reason << "\"";
		ofLogFatalError("ofxApp") << "This message will be on screen for " << (int)secondsOnScreen << " seconds, then the app will quit.";
		ofLogFatalError("ofxApp") << "";
		ofLogFatalError("ofxApp") << "-----------------------------------------------------------------------------------------------------------";
		ofLogFatalError("ofxApp") << "";
		ofxThreadSafeLog::one()->close();
		ofxSuperLog::getLogger()->setScreenLoggingEnabled(true); //show log if json error
		ofxSuperLog::getLogger()->getDisplayLogger().setPanelWidth(1.0);
		int numFrames = secondsOnScreen * 1000 / 16; //stay up a bit so that you can read logs on screen
		
		//hijack OF and refresh screen & events by hand
		if(ofGetWindowPtr()){
			for(int i = 0; i < numFrames; i++ ){
				ofSetupScreen();
				ofClear(0,0,0,255);
				ofxSuperLog::getLogger()->getDisplayLogger().draw(ofGetWidth(), ofGetHeight());
				//ofGetMainLoop()->loopOnce();
				ofGetMainLoop()->pollEvents();
				if(ofGetWindowPtr()->getWindowShouldClose()){
					ofLogFatalError("ofxApp") << "Quitting by user action";
					exit(-1);
				}
				ofGetWindowPtr()->swapBuffers();
				ofSleepMillis(16);
			}
		}
		exit(-1);
	};

	static void assertFileExists(string path){
		if(!ofFile::doesFileExist(path)){
			string msg = "Required asset not present: '" + path + "'";
			ofLogFatalError("HasAssets") << msg;
			ofxApp::terminateApp("ofxApp", msg);
		}else{
			//ofLogNotice("HasAssets") << "Confirmed asset is present: '" << path << "'";
		}
	}
};


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

	virtual void ofxAppStartUserPhase(ofxApp::UserAppSetupStage){}; //this will be your entry point to start loading stuff
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