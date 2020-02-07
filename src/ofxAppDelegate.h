//
//  ofxAppDelegate.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxAppStructs.h"
#include "ofxTuio.h"
#include "ofxScreenSetup.h"
#include "ofxAppTuioManager.h"

//to use ofxApp, your app must follow this protocol, so make sure your app
//is a subclass of ofxAppDelegate

class ofxAppDelegate{

public:

	// your app will be given a chance to run 4 custom tasks during startup:
	// 1 - To run code just b4 content loading starts (ofxApp::Phase == WILL_LOAD_CONTENT)
	// 2 - To process the loaded content (ofxApp::State == DID_DELIVER_CONTENT)
	// 3 - Once last callback just b4 the app starts Running (ofxApp::State == WILL_BEGIN_RUNNING)
	//
	// your ofxAppPhaseWillBegin(ofxApp::Phase) will get called when this happens;
	//
	// You can either run your code directly in the callback, or thread it and
	// make the loading process more friendly by using, ofxAppIsPhaseComplete()
	// ofxAppGetProgressForPhase() and ofxAppDrawPhaseProgress()
	// letting the app update while the loading happens; ofxApp will show a loading screen
	// with progressbar & such, and you can drive what to draw on screen while that happens.
	// just return true in ofxAppIsPhaseComplete() when the stage is concluded for ofxApp to proceed
	// to the next state.

	//this will be your entry point to start loading stuff
	virtual void ofxAppPhaseWillBegin(ofxApp::Phase) = 0;
	//after u are asked to start loading content, ofxApp will query every frame to check if you are done
	virtual bool ofxAppIsPhaseComplete(ofxApp::Phase){return true;} //override this method if you are loading custom content
	virtual void ofxAppDrawPhaseProgress(ofxApp::Phase, const ofRectangle & r){}; //override the loading screen drawing
	virtual std::string ofxAppGetStatusString(ofxApp::Phase){return "";}; //override the progress bar status text
	virtual std::string ofxAppGetLogString(ofxApp::Phase){return "";}; //override the text message above the progress bar (ie for showing script logs)
	virtual float ofxAppGetProgressForPhase(ofxApp::Phase){return -1;} //return [0..1] to report progressbar; -1 for indeterminate

	virtual bool ofxAppShouldFetchContent(const std::string & contentID){return true;}

	//this is how your app gets all the parsed objects - up to you how you store them
	virtual void ofxAppContentIsReady(const std::string & contentID, const vector<ContentObject*> & objs) = 0;

	//this is your chance to override where the content will be downloaded from;
	//ofxApp will provide the JsonURL you defined in ofxAppSettings.json, and you can modify it
	//to your linking (ie add a timepstamp, a URL queryParam or whatever you want) and return that
	virtual string ofxAppWillFetchContentFromURL(const std::string & contentID, const std::string & jsonURL){return jsonURL;};

	//this is your chance to override the ofxAppSettings.json config for ofxGoogleAnalytics, just edit the
	//string references
	virtual void ofxAppWillSetupGoogleAnalytics(std::string & googleTrackingID_, std::string & appName,
												std::string & appVersion, std::string & appID, std::string & appInstallerID){};

	//if you enabled live updates for your content source, you will gett this callback every "interval" seconds
	//with the new content in that JSON src (with all the assets downloaded, etc)
	virtual void ofxAppContentUpdate(const std::string & contentID, const vector<ContentObject*> & objs ){};
	virtual void ofxAppContentUpdateFailed(const std::string & contentID, const std::string & errorMsg){};

	//tuio callbacks
	virtual void multiportTuioAdded(TouchAtPort& t){};
	virtual void multiportTuioRemoved(TouchAtPort& t){};
	virtual void multiportTuioUpdated(TouchAtPort& t){};
    
    virtual void tuioAdded(ofxTuioCursor & tuioCursor){};
    virtual void tuioRemoved(ofxTuioCursor & tuioCursor){};
    virtual void tuioUpdated(ofxTuioCursor & tuioCursor){};

	//screen setup changed callback
	virtual void screenSetupChanged(ofxScreenSetup::ScreenSetupArg &arg){};

	//error report
	virtual void ofxAppErrorReported(ofxApp::ErrorReportData & data){};

};
