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
#include "ofxSimpleHttp.h"
#include "ofxTuio.h"

class HasRuiParams{
public:
	virtual void setupRemoteUIParams() = 0;
};

class CanTerminate{
public:
	virtual void terminateApp(){
		ofxSimpleHttp::destroySslContext();
		ofLogFatalError("ofxApp") << "terminateApp()!";
		ofxThreadSafeLog::one()->close();
		exit(-1);
	};
};


class HasAssets{
public:
	static void assertFileExists(string path){
		if(!ofFile::doesFileExist(path)){
			ofLogFatalError("HasAssets") << "Required asset not present: '" << path << "'";
			ofLogFatalError("HasAssets") << "Terminating app";
			ofExit();
			exit(-1);
		}else{
			//ofLogNotice("HasAssets") << "Confirmed asset is present: '" << path << "'";
		}
	}
};

//to use ofxApp, your app must follow this protocol, so make sure your app
//is a subclass of ofxAppDelegate

class ofxAppDelegate{

public:


	//your app will be given a chance to run 2 custom tasks during startup:
	// 1 - Load Custom Content (ofxApp::State == LOAD_CUSTOM_USER_CONTENT)
	// 2 - Setup your App (ofxApp::State == SETUP_USER_APP)

	virtual void startUserProcess(ofxApp::State){}; //this will be your entry point to start loading stuff
	//after u are asked to start loading content, ofxApp will query every frame to check if you are done
	virtual bool isUserProcessDone(ofxApp::State){return true;} //your APP should override this method if you are loading custom content
	virtual void drawLoadingScreenForUserProcess(ofxApp::State, const ofRectangle & r){}; //your APP can override the loading screen drawing
	virtual float getProgressForUserProcess(ofxApp::State){return -1;} //your APP should return [0..1] to report progressbar; -1 for indeterminate

	virtual void contentIsReady(vector<ContentObject*>){};


	//tuio callbacks
	virtual void tuioAdded(ofxTuioCursor & tuioCursor){};
	virtual void tuioRemoved(ofxTuioCursor & tuioCursor){};
	virtual void tuioUpdated(ofxTuioCursor & tuioCursor){};

};