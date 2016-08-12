//
//  AppContentBasic.h
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#pragma once

#include "ofMain.h"
#include "AppBaseClasses.h"
#include "ofxAsset.h"
#include "ofxSimpleHttp.h"
#include "ofxDownloadCentral.h"
#include "ofxMtJsonParser.h"
#include "AssetChecker.h"
#include "ofxAppStructs.h"

//all your content objects will have to subclass this class
class ContentObject : public ParsedObject, public AssetHolder{

};



class AppContentBasic{

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
		CONTENT_READY,
		LOAD_CUSTOM_USER_CONTENT,
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
			   const ofxAssets::DownloadPolicy & downloadPolicy,
			   const ofxAssets::UsagePolicy & usagePolicy,
			   const ofxApp::ContentConfig & contentCfg
			   );

	virtual void fetchContent(); //start the process here
	virtual void update(float dt);

	virtual bool foundError();
	virtual bool isContentReady();
	virtual string getStatus();
	virtual float getPercentDone();
	virtual bool isBusy();
	virtual bool customUserContentIsReady() = 0;

	void onDrawStateMachineStatus(ofRectangle & drawableArea);

	// CALLBACKS ///////////////////////////////////////////////////////////////////////////////////

	void jsonDownloaded(ofxSimpleHttpResponse & arg);
	void jsonDownloadFailed(ofxSimpleHttpResponse & arg);
	void jsonInitialCheckOK();
	void jsonParseFailed();
	void jsonContentReady(vector<ParsedObject*> &parsedObjects);
	void assetCheckFinished();

	int getNumStates(){return NUM_CONTENT_MANAGER_STATES;}
	string getNameForState(AppContentBasic::ContentState state);

	// EVENTS //////////////////////////////////////////////////////////////////////////////////////

	ofEvent<string> eventStateChanged; //its your responsability to trigger this on every state change

protected:

	void setState(ContentState s);

	ContentState state;
	float timeInState = 0;
	int totalAssetsToDownload = 0;

	ofxMtJsonParser jsonParser;
	ofxApp::ContentConfig contentCfg;

	vector<ContentObject*> parsedObjects;
	AssetChecker assetChecker;
	ofxDownloadCentral dlc;

	string jsonURL;
	string jsonDestinationDir; //where the json will be downloaded to
	string errorMessage;
	int numThreads = 4;

};

