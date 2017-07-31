//
//  ofxAppContent.h
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#pragma once

#include "ofMain.h"
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

		virtual ~ContentObject() {};

		// Imposed by TexturedObject //
		virtual ofVec2f getTextureDimensions(TexturedObjectSize, int){ return ofVec2f(0,0);}
		virtual string getLocalTexturePath(TexturedObjectSize, int){ return "";}

		//this is effectively the destructor of the object - bc of texturedObject loading assets
 		//in secondary threads, we can't use an in-place destructor as you could destruct the object
		//while a thread is loading it - thus we have this GarbageCollector-like behavior.
		virtual void deleteWithGC(){}
};


class ofxAppContent : public ofThread{

public:

	enum class ContentState{
		IDLE = 0,
		DOWNLOADING_JSON,
		JSON_DOWNLOAD_FAILED,
		CHECKING_JSON,
		JSON_PARSE_FAILED,
		PARSING_JSON,
		CATALOG_ASSETS,
		CHECKING_ASSET_STATUS,
		DOWNLOADING_ASSETS,
		FILTER_OBJECTS_WITH_BAD_ASSETS,
		SETUP_TEXTURED_OBJECTS,
		JSON_CONTENT_READY,
	};

	~ofxAppContent();

	void setup(string ID,
			   string jsonSrc,
			   string jsonDestinationDir_,
			   int numThreads,
			   int numConcurrentDownloads,
			   int speedLimitKBs,
			   int timeout,
			   bool shouldSkipObjectTests,
			   float idleTimeAfterEachDownload,
			   const std::pair<string,string> & credentials,
			   const ofxSimpleHttp::ProxyConfig & proxyConfig,
			   const ofxApp::ParseFunctions & contentCfg,
			   const ofxAssets::DownloadPolicy assetDownloadPolicy,
			   const ofxAssets::UsagePolicy assetUsagePolicy,
			   const ofxAssets::ObjectUsagePolicy & objectUsagePolicy,
			   const string & assetsLocationPath
			   );

	bool isReadyToFetchContent();
	void fetchContent(); //start the process here
	
	void setJsonDownloadURL(string jsonURL);
	string getJsonDownloadURL(){ return jsonURL;};

	void update(float dt);

	bool foundError();
	bool isContentReady();

	string getStatus(bool formatted = true);
	string getErrorMsg(){return errorMessage;}
	float getPercentDone();

	void stopAllDownloads();

	//call this only if isContentReady() == true
	vector<ContentObject*> getParsedObjects(){return parsedObjects;};
	int getNumParsedObjects(){ return parsedObjects.size();} //
	int getNumIgnoredObjects(){return numIgnoredObjects;} //total # of obj that are in json but are not used for one reason or another

	void onDrawStateMachineStatus(ofRectangle & drawableArea);
	
	string getLastKnownGoodJsonPath();

	// ofxMtJsonParser CALLBACKS ///////////////////////////////////////////////////////////////////

	void jsonDownloaded(ofxSimpleHttpResponse & arg);
	void jsonDownloadFailed(ofxSimpleHttpResponse & arg);
	void jsonInitialCheckOK();
	void jsonParseFailed();
	void jsonContentReady(vector<ParsedObject*> &parsedObjects);
	void assetCheckFinished();

	string getNameForState(ofxAppContent::ContentState state);

	string getFreshJsonSha1(){return newJsonSha1;}
	string getOldJsonSha1(){return oldJsonSha1;}

	// EVENTS //////////////////////////////////////////////////////////////////////////////////////

	ofEvent<string> eventStateChanged; //its your responsability to trigger this on every state change

protected:

	void threadedFunction();

	void setState(ContentState s);

	ContentState state = ContentState::IDLE;
	float timeInState = 0;
	int totalAssetsToDownload = 0;

	ofxMtJsonParser jsonParser;
	ofxApp::ParseFunctions contentCfg;
	ofxAssets::ObjectUsagePolicy objectUsagePolicy;
	ofxAssets::DownloadPolicy assetDownloadPolicy;
	ofxAssets::UsagePolicy assetUsagePolicy;


	vector<ContentObject*> parsedObjects;
	AssetChecker assetChecker;
	ofxDownloadCentral dlc;

	string jsonURL;
	string jsonDestinationDir; //where the json will be downloaded to
	string errorMessage;
	int numThreads = 4;
	string objectsWithBadAssets;
	bool shouldSkipObjectTests;
	string assetsLocationPath;
	string ID;
	int numIgnoredObjects = 0; //total # of obj that are in json but are not used for one reason or another
	int numSetupTexuredObjects = 0;

	string oldJsonSha1;
	string newJsonSha1;
};

