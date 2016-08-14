//
//  AppContent.h
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#pragma once

#include "ofMain.h"
#include "AppBaseClasses.h"
#include "ofxAssets.h"
#include "ofxSimpleHttp.h"
#include "ofxDownloadCentral.h"
#include "ofxMtJsonParser.h"
#include "AssetChecker.h"
#include "ofxAppStructs.h"
#include "TexturedObject.h"

//all your content objects will have to subclass this class
class ContentObject : public ParsedObject, public AssetHolder, public TexturedObject{

	public:

		// Imposed by TexturedObject //
		ofVec2f getTextureDimensions(TexturedObjectSize, int){ return ofVec2f(0,0);}
		string getLocalTexturePath(TexturedObjectSize, int){ return "";}
		void deleteWithGC(){} //this is effectively the destructor of the object
};



class AppContent{

public:

	enum ContentState{
		IDLE = 0,
		DOWNLOADING_JSON,
		JSON_DOWNLOAD_FAILED,
		CHECKING_JSON,
		JSON_PARSE_FAILED,
		PARSING_JSON,
		CHECKING_ASSET_STATUS,
		DOWNLOADING_ASSETS,
		FILTER_OBJECTS_WITH_BAD_ASSETS,
		SETUP_TEXTURED_OBJECTS,
		JSON_CONTENT_READY,
		NUM_CONTENT_MANAGER_STATES
	};

	void setup(string jsonSrc,
			   string jsonDestinationDir_,
			   int numThreads,
			   int numConcurrentDownloads,
			   int speedLimitKBs,
			   int timeout,
			   float idleTimeAfterEachDownload,
			   const std::pair<string,string> & credentials,
			   const ofxSimpleHttp::ProxyConfig & proxyConfig,
			   const ofxApp::UserLambdas & contentCfg
			   );

	void fetchContent(); //start the process here

	void update(float dt);

	bool foundError(){ return state == JSON_DOWNLOAD_FAILED || state == JSON_PARSE_FAILED; };
	bool isContentReady(){ return state == JSON_CONTENT_READY; };

	string getStatus();
	float getPercentDone();
	bool isBusy();

	//call this only if isContentReady() == true
	vector<ContentObject*> getParsedObjects(){return parsedObjects;};

	void onDrawStateMachineStatus(ofRectangle & drawableArea);

	// CALLBACKS ///////////////////////////////////////////////////////////////////////////////////

	void jsonDownloaded(ofxSimpleHttpResponse & arg);
	void jsonDownloadFailed(ofxSimpleHttpResponse & arg);
	void jsonInitialCheckOK();
	void jsonParseFailed();
	void jsonContentReady(vector<ParsedObject*> &parsedObjects);
	void assetCheckFinished();

	int getNumStates(){return NUM_CONTENT_MANAGER_STATES;}
	string getNameForState(AppContent::ContentState state);

	// EVENTS //////////////////////////////////////////////////////////////////////////////////////

	ofEvent<string> eventStateChanged; //its your responsability to trigger this on every state change

protected:

	void setState(ContentState s);

	ContentState state;
	float timeInState = 0;
	int totalAssetsToDownload = 0;

	ofxMtJsonParser jsonParser;
	ofxApp::UserLambdas contentCfg;

	vector<ContentObject*> parsedObjects;
	AssetChecker assetChecker;
	ofxDownloadCentral dlc;

	string jsonURL;
	string jsonDestinationDir; //where the json will be downloaded to
	string errorMessage;
	int numThreads = 4;

};

