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
#include "ofxChecksum.h"

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
		REMOVING_EXPIRED_ASSETS,
		DOWNLOADING_ASSETS,
		FILTER_OBJECTS_WITH_BAD_ASSETS,
		SETUP_TEXTURED_OBJECTS,
		FILTER_REJECTED_TEXTURED_OBJECTS,
		JSON_CONTENT_READY,
	};

	~ofxAppContent();

	void setup(	const std::string &  ID,
				const std::string &  jsonSrc, //the URL where the live content lives
				const std::string &  jsonSrc_offline, //the offline (local file) copy of the json
				bool useOfflineJson, //are we supposed to use offline?
			   	const std::string &  jsonDestinationDir_,
			   	int numThreads,
			   	int maxDlRetries,
			   	int copyBufferSizeKb,
			   	int numConcurrentDownloads,
			   	int speedLimitKBs,
			   	int timeoutDownloads,
			   	int timeoutApiEndpoint,
			   	bool shouldSkipObjectTests,
			   	float idleTimeAfterEachDownload,
			   	const std::pair<std::string,std::string> & downloaderCredentials, //dl configs for asset downloads (imgs, vids)
			   	ofxChecksum::Type checksumType,
			   	const ofxSimpleHttp::ProxyConfig & downloaderProxyConfig,
			   	const std::pair<std::string,std::string> & apiEndPointCredentials, //dl configs for endpoints (JSON)
			   	const ofxSimpleHttp::ProxyConfig & apiEndpointProxyConfig,
			   	const map<std::string, std::string> customHeaders,
			   	const ofxApp::ParseFunctions & contentCfg,
			   	const ofxAssets::DownloadPolicy assetDownloadPolicy,
			   	const ofxAssets::UsagePolicy assetUsagePolicy,
			   	const ofxAssets::ObjectUsagePolicy & objectUsagePolicy,
			   	const std::string & assetsLocationPath,
			   	bool skipChecksumTests,
			   	float assetErrorsScreenReportTimeSeconds
			   );
    void setupJsonOAuth(string tokenURL, string clientID, string clientSecret);
    
	void setNumThreads(int nThreads);
	void setMaxConcurrentDownloads(int nDownloads);

	bool isReadyToFetchContent();
	void fetchContent(); //start the process here

	void setShouldRemoveExpiredAssets(bool);

	void setJsonDownloadURL(std::string jsonURL, bool itsOfflineJson);
	std::string getJsonDownloadURL(){ return jsonURL;};

	void update(float dt);

	bool foundError();
	bool isContentReady();

	std::string getStatus();
	std::string getErrorMsg(){return errorMessage;}
	float getPercentDone();

	void stopAllDownloads();

	//call this only if isContentReady() == true
	vector<ContentObject*> getParsedObjects(){return parsedObjects;};
	int getNumParsedObjects(){ return parsedObjects.size();} //
	int getNumIgnoredObjects(){return numIgnoredObjects;} //total # of obj that are in json but are not used for one reason or another

	void onDrawStateMachineStatus(ofRectangle & drawableArea);
	
	std::string getLastKnownGoodJsonPath();

	// ofxMtJsonParser CALLBACKS ///////////////////////////////////////////////////////////////////

	void onJsonDownloaded(ofxSimpleHttpResponse & arg);
	void onJsonDownloadFailed(ofxSimpleHttpResponse & arg);
	void onJsonInitialCheckOK();
	void onJsonParseFailed();
	void onJsonContentReady(vector<ParsedObject*> &parsedObjects);
	void assetCheckFinished();

	std::string getNameForState(ofxAppContent::ContentState state);

	std::string getFreshJsonSha1(){return newJsonChecksum;}
	std::string getoldJsonChecksum(){return oldJsonChecksum;}
	float getRunDuration(){return totalDuration;};

	// EVENTS //////////////////////////////////////////////////////////////////////////////////////

	ofEvent<std::string> eventStateChanged; //its your responsability to trigger this on every state change

protected:

	void threadedFunction();

	void setState(ContentState s);

	void removeExpiredAssets();

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

	std::string jsonURL;
	std::string jsonURL_offline;
	bool useOfflineJson = false;

	std::string getAdaptativeJsonUrl();

	std::string jsonDestinationDir; //where the json will be downloaded to
	std::string errorMessage;
	int numThreads = 4;
	std::string objectsWithBadAssets;
	bool shouldSkipObjectTests;
	std::string assetsLocationPath;
	std::string ID;
	int numIgnoredObjects = 0; //total # of obj that are in json but are not used for one reason or another
	int numSetupTexuredObjects = 0;

	std::string oldJsonChecksum; //
	std::string newJsonChecksum;

	float startTimestamp = 0;
	float totalDuration = 0;
	bool shouldSkipSha1Tests = false;
	bool shouldRemoveExpiredAssets = true;
	float assetErrorsScreenReportTimeSeconds = 0.0; //if there are asset download / policy errors, show on screen for how many seconds?
};

