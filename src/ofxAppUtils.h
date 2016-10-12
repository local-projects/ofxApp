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
	
	static string secondsToHumanReadable(float secs, int decimalPrecision){
		string ret;
		if (secs < 60.0f ){ //if in seconds
			ret = ofToString(secs, decimalPrecision) + " seconds";
		}else{
			if (secs < 3600.0f){ //if in min range
				ret = ofToString(secs / 60.0f, decimalPrecision) + " minutes";
			}else{ //hours or days
				if (secs < 86400.0f){ // hours
					ret = ofToString(secs / 3600.0f, decimalPrecision) + " hours";
				}else{ //days
					if (secs < 86400.0f * 7.0f){ // days
						ret = ofToString(secs / (86400.0f), decimalPrecision) + " days";
					}else{ //days
						ret = ofToString(secs / (86400.0f * 7.0f) , decimalPrecision) + " weeks";
					}
				}
			}
		}
		return ret;
	}

};
