//
//  AppContent.cpp
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#include "AppContent.h"

void AppContent::setup(string jsonSrc,
							string jsonDestinationDir_,
							int numThreads_,
							int numConcurrentDownloads,
							int speedLimitKBs,
							int timeout,
							float idleTimeAfterEachDownload,
							const std::pair<string,string> & credentials,
							const ofxSimpleHttp::ProxyConfig & proxyConfig,
							const ofxApp::UserLambdas & contentCfg){

	this->jsonURL = jsonSrc;
	this->contentCfg = contentCfg;
	this->jsonDestinationDir = jsonDestinationDir_;
	this->numThreads = numThreads_;

	//config the http downloader if you need to (proxy, etc)
	dlc.setMaxConcurrentDownloads(numConcurrentDownloads);
	dlc.setSpeedLimit(speedLimitKBs);
	dlc.setTimeOut(timeout);
	dlc.setIdleTimeAfterEachDownload(idleTimeAfterEachDownload);
	dlc.setCredentials(credentials.first, credentials.second);
	dlc.setProxyConfiguration(proxyConfig);

	//subscribe to parsing events
	ofAddListener(jsonParser.eventJsonDownloaded, this, 	&AppContent::jsonDownloaded);
	ofAddListener(jsonParser.eventJsonDownloadFailed, this, &AppContent::jsonDownloadFailed);
	ofAddListener(jsonParser.eventJsonInitialCheckOK, this, &AppContent::jsonInitialCheckOK);
	ofAddListener(jsonParser.eventJsonParseFailed, this, 	&AppContent::jsonParseFailed);
	ofAddListener(jsonParser.eventAllObjectsParsed, this, 	&AppContent::jsonContentReady);
}


void AppContent::fetchContent(){
	if(state == IDLE || state == JSON_PARSE_FAILED ){
		setState(DOWNLOADING_JSON);
	}else{
		ofLogError("AppContent") << "Can't fetch content now!";
	}
}


void AppContent::update(float dt){

	timeInState += ofGetLastFrameTime();
	jsonParser.update();
	dlc.update();
	assetChecker.update();

	switch(state){

		case DOWNLOADING_ASSETS:
			if(!dlc.isBusy()){ //downloader finished!
				ofLogNotice("AppContent") << "finished asset downloads!";
				setState(FILTER_OBJECTS_WITH_BAD_ASSETS);
			}break;
	}
}



void AppContent::setState(ContentState s){

	state = s;
	timeInState = 0;

	switch (s) {

		case DOWNLOADING_JSON:
			//start the download and parse process
			jsonParser.downloadAndParse(jsonURL,
										jsonDestinationDir,	//directory where to save
										numThreads,			//num threads
										contentCfg.describeJsonUserLambda,
										contentCfg.parseSingleObjectUserLambda
										);
			break;

		case CHECKING_ASSET_STATUS:{
			//sadly we need to cast our objects to AssetHolder* objects to check them
			vector<AssetHolder*> assetObjs;
			assetObjs.insert(assetObjs.begin(), parsedObjects.begin(), parsedObjects.end());
			ofAddListener(assetChecker.eventFinishedCheckingAllAssets, this, &AppContent::assetCheckFinished);
			assetChecker.checkAssets(assetObjs, numThreads);
		}break;

		case DOWNLOADING_ASSETS:
			//fill in the list
			for(int i = 0; i < parsedObjects.size(); i++){
				parsedObjects[i]->downloadMissingAssets(dlc);
			}
			totalAssetsToDownload = dlc.getNumPendingDownloads();
			dlc.setNeedsChecksumMatchToSkipDownload(true);
			dlc.startDownloading();
			break;

		case FILTER_OBJECTS_WITH_BAD_ASSETS:{
			vector<int> badObjects;
			vector<string> badObjectsIds;

			for(int i = 0; i < parsedObjects.size(); i++){

				bool assetsOK = parsedObjects[i]->areAllAssetsOK();
				bool hasEnoughAssets = parsedObjects[i]->getNumAssets() > 0;

				if(!assetsOK) ofLogError("AppContent") << "object " << parsedObjects[i]->getObjectUUID() << " assets NOT OK!";
				if(!hasEnoughAssets) ofLogError("AppContent") << "object '" << parsedObjects[i]->getObjectUUID() << "' has no assets!";

				if (!assetsOK || !hasEnoughAssets ){
					badObjects.push_back(i);
					badObjectsIds.push_back(parsedObjects[i]->getObjectUUID());
				}
			}

			for(int i = badObjects.size() - 1; i >= 0; i--){
				ofLogError("AppContent") << "Dropping object " << parsedObjects[i]->getObjectUUID();
				delete parsedObjects[badObjects[i]];
				parsedObjects.erase(parsedObjects.begin() + badObjects[i]);
			}
			setState(SETUP_TEXTURED_OBJECTS);
		}break;

		case SETUP_TEXTURED_OBJECTS:
			for(int i = 0; i < parsedObjects.size(); i++){
				auto setupTexObjuserLambda = contentCfg.setupTexturedObjectUserLambda;
				//call the User Supplied Lambda to setup the user's TexturedObject
				setupTexObjuserLambda( parsedObjects[i] );
			}
			setState(JSON_CONTENT_READY);
			break;

		case JSON_CONTENT_READY:
			break;

		default: break;
	}

	string name = getNameForState(state);
	ofNotifyEvent(eventStateChanged, name);
}


string AppContent::getStatus(){

	string r;
	string plainFormat = " %0.9 #0x888888 \n"; //text format for logging on screen - see ofxFontStash.h drawMultiLineColumn()
	string errorFormat = " %0.9 #0xBB0000 \n"; //text format for logging on screen - see ofxFontStash.h drawMultiLineColumn()

	switch (state) {
		case DOWNLOADING_JSON: r = plainFormat + jsonParser.getHttp().drawableString(); break;
		case JSON_DOWNLOAD_FAILED: r = errorFormat + errorMessage; break;
		case CHECKING_JSON: r = plainFormat + jsonParser.getDrawableState(); break;
		case PARSING_JSON: r = plainFormat + jsonParser.getDrawableState(); break;
		case CHECKING_ASSET_STATUS: r = plainFormat + assetChecker.getDrawableState(); break;
		case JSON_PARSE_FAILED: r = errorFormat +  errorMessage; break;
		case DOWNLOADING_ASSETS: r =  plainFormat + dlc.getDrawableInfo(true, false); break;
		case FILTER_OBJECTS_WITH_BAD_ASSETS: r = plainFormat; break;
		case SETUP_TEXTURED_OBJECTS: r = plainFormat; break;
		case JSON_CONTENT_READY: r = plainFormat + "READY"; break;
	}
	return r;
}


float AppContent::getPercentDone(){
	float p = -1.0f;
	switch (state) {
		case DOWNLOADING_JSON: p = jsonParser.getHttp().getCurrentDownloadProgress(); break;
		case CHECKING_JSON: p = -1.0; break;
		case PARSING_JSON: p = jsonParser.getTotalProgress(); break;
		case CHECKING_ASSET_STATUS: p = assetChecker.getProgress(); break;
		case FILTER_OBJECTS_WITH_BAD_ASSETS: p = -1; break;
		case DOWNLOADING_ASSETS:
			p = 1.0 - float(dlc.getNumPendingDownloads()) / totalAssetsToDownload;
			break;
		default: break;
	}
	return p;
}


bool AppContent::isBusy(){

}

// CALBACKS ////////////////////////////////////////////////////////////////////////////////////
#pragma mark Callbacks

void AppContent::jsonDownloaded(ofxSimpleHttpResponse & arg){
	ofLogNotice("AppContent") << "JSON download OK!";
	setState(CHECKING_JSON);
}


void AppContent::jsonDownloadFailed(ofxSimpleHttpResponse & arg){
	ofLogError("AppContent") << "JSON download failed!";
	errorMessage = arg.reasonForStatus + " (" + arg.url + ")";
	setState(JSON_DOWNLOAD_FAILED);
}


void AppContent::jsonInitialCheckOK(){
	ofLogNotice("AppContent") << "JSON Initial Check OK!";
	setState(PARSING_JSON);
}


void AppContent::jsonParseFailed(){
	ofLogError("AppContent") << "json Parse Failed!";
	setState(JSON_PARSE_FAILED);
}


void AppContent::jsonContentReady(vector<ParsedObject*> &parsedObjects_){
	ofLogNotice("PgContentManager") << "json Content Ready! " << parsedObjects.size() << " Objects received.";
	//parsedObjects = parsedObjects_;
	parsedObjects.reserve(parsedObjects_.size());
	for(auto o : parsedObjects_){
		parsedObjects.push_back((ContentObject*)o);
	}
	setState(CHECKING_ASSET_STATUS);
}


void AppContent::assetCheckFinished(){
	ofLogNotice("AppContent") << "Asset Check Finished!";
	setState(DOWNLOADING_ASSETS);
}


string AppContent::getNameForState(AppContent::ContentState state){

	switch (state) {
		case IDLE: return "IDLE";
		case DOWNLOADING_JSON: return "DOWNLOADING_JSON";
		case JSON_DOWNLOAD_FAILED: return "JSON_DOWNLOAD_FAILED";
		case CHECKING_JSON: return "CHECKING_JSON";
		case JSON_PARSE_FAILED: return "JSON_PARSE_FAILED";
		case PARSING_JSON: return "PARSING_JSON";
		case CHECKING_ASSET_STATUS: return "CHECKING_ASSET_STATUS";
		case DOWNLOADING_ASSETS: return "DOWNLOADING_ASSETS";
		case FILTER_OBJECTS_WITH_BAD_ASSETS: return "FILTER_OBJECTS_WITH_BAD_ASSETS";
		case SETUP_TEXTURED_OBJECTS: return "SETUP_TEXTURED_OBJECTS";
		case JSON_CONTENT_READY: return "JSON_CONTENT_READY";
		case NUM_CONTENT_MANAGER_STATES: return "NUM_CONTENT_MANAGER_STATES";
	}
	return "UNKNOWN STATE";
}
