//
//  ofxAppContent.cpp
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#include "ofxAppContent.h"
#include "ofxAppUtils.h"
#include "ofxApp.h"

void ofxAppContent::setup(	string ID,
							string jsonSrc,
							string jsonDestinationDir_,
							int numThreads_,
							int numConcurrentDownloads,
							int speedLimitKBs,
							int timeout,
							bool shouldSkipObjectTests,
							float idleTimeAfterEachDownload,
							const std::pair<string,string> & credentials,
							const ofxSimpleHttp::ProxyConfig & proxyConfig,
							const ofxApp::ParseFunctions & contentCfg,
							const ofxAssets::ObjectUsagePolicy & objectUsagePolicy,
							const string & assetsLocationPath){

	state = ContentState::IDLE;
	parsedObjects.clear();
	this->ID = ID;
	this->jsonURL = jsonSrc;
	this->contentCfg = contentCfg;
	this->objectUsagePolicy = objectUsagePolicy;
	this->jsonDestinationDir = jsonDestinationDir_;
	this->numThreads = numThreads_;
	this->shouldSkipObjectTests = shouldSkipObjectTests;
	this->assetsLocationPath = assetsLocationPath;

	//config the http downloader if you need to (proxy, etc)
	dlc.setMaxConcurrentDownloads(numConcurrentDownloads);
	dlc.setSpeedLimit(speedLimitKBs);
	dlc.setTimeOut(timeout);
	dlc.setIdleTimeAfterEachDownload(idleTimeAfterEachDownload);
	dlc.setCredentials(credentials.first, credentials.second);
	dlc.setProxyConfiguration(proxyConfig);

	jsonParser.getHttp().setTimeOut(timeout);
	jsonParser.getHttp().setSpeedLimit(speedLimitKBs);
	jsonParser.getHttp().setProxyConfiguration(proxyConfig);
	jsonParser.getHttp().setCredentials(credentials.first, credentials.second);

	//subscribe to parsing events
	ofAddListener(jsonParser.eventJsonDownloaded, this, 	&ofxAppContent::jsonDownloaded);
	ofAddListener(jsonParser.eventJsonDownloadFailed, this, &ofxAppContent::jsonDownloadFailed);
	ofAddListener(jsonParser.eventJsonInitialCheckOK, this, &ofxAppContent::jsonInitialCheckOK);
	ofAddListener(jsonParser.eventJsonParseFailed, this, 	&ofxAppContent::jsonParseFailed);
	ofAddListener(jsonParser.eventAllObjectsParsed, this, 	&ofxAppContent::jsonContentReady);
}


ofxAppContent::~ofxAppContent(){
	ofLogNotice("ofxAppContent") << "~ofxAppContent \"" << ID << "\"";
}


void ofxAppContent::setJsonDownloadURL(string jsonURL){
	ofLogNotice("ofxAppContent") << "updating the JSON Content URL of " << ID << " to '" << jsonURL << "'";
	this->jsonURL = jsonURL;
};


void ofxAppContent::fetchContent(){
	if(state == ContentState::IDLE ||
	   state == ContentState::JSON_PARSE_FAILED ||
	   state == ContentState::JSON_DOWNLOAD_FAILED){
		setState(ContentState::DOWNLOADING_JSON);
	}else{
		ofLogError("ofxAppContent") << "Can't fetch content now!";
	}
}


void ofxAppContent::stopAllDownloads(){
	dlc.cancelAllDownloads();
}


void ofxAppContent::update(float dt){

	timeInState += ofGetLastFrameTime();
	jsonParser.update();
	dlc.update();
	assetChecker.update();

	switch(state){

		case ContentState::DOWNLOADING_ASSETS:
			if(!dlc.isBusy()){ //downloader finished!
				ofLogNotice("ofxAppContent") << "Finished Asset downloads for \"" << ID << "\"!";
				setState(ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS);
			}break;

		case ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS:
			if(timeInState > 0.1){ //show this on screen for a sec
				setState(ContentState::SETUP_TEXTURED_OBJECTS);
			}
			break;

		default: break;
	}
}


void ofxAppContent::setState(ContentState s){

	state = s;
	timeInState = 0;

	switch (s) {

		case ContentState::DOWNLOADING_JSON:{

			map<string,string> userData; //the end user needs some basics to parse, like where are the assets downloaded at
			userData["assetsLocation"] = assetsLocationPath;

			//start the download and parse process
			jsonParser.downloadAndParse(jsonURL,
										jsonDestinationDir,	//directory where to save
										numThreads,			//num threads
										contentCfg.pointToObjects,
										contentCfg.parseOneObject,
										userData
										);
			}
			break;

		case ContentState::CHECKING_ASSET_STATUS:{
			//sadly we need to cast our objects to AssetHolder* objects to check them
			if (parsedObjects.size()) {
				vector<AssetHolder*> assetObjs;
				for (int i = 0; i < parsedObjects.size(); i++) {
					assetObjs.push_back(dynamic_cast<AssetHolder*>(parsedObjects[i]));
				}
				ofAddListener(assetChecker.eventFinishedCheckingAllAssets, this, &ofxAppContent::assetCheckFinished);
				assetChecker.checkAssets(assetObjs, numThreads);
			} else {
				setState(ContentState::DOWNLOADING_ASSETS);
			}

		}break;

		case ContentState::DOWNLOADING_ASSETS:
			//fill in the list
			for(int i = 0; i < parsedObjects.size(); i++){
				parsedObjects[i]->downloadMissingAssets(dlc);
			}
			totalAssetsToDownload = dlc.getNumPendingDownloads();
			dlc.setNeedsChecksumMatchToSkipDownload(true);
			dlc.startDownloading();
			break;

		case ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS:{

			if(!shouldSkipObjectTests){
				objectsWithBadAssets.clear();

				vector<int> badObjects;
				vector<string> badObjectsIds;

				for(int i = 0; i < parsedObjects.size(); i++){

					//do some asset integrity tests...
					bool allAssetsOK = parsedObjects[i]->areAllAssetsOK();
					bool needsAllAssetsToBeOk = objectUsagePolicy.allObjectAssetsAreOK;
					int numImgAssets = parsedObjects[i]->getAssetDescriptorsForType(ofxAssets::IMAGE).size();
					int numVideoAssets = parsedObjects[i]->getAssetDescriptorsForType(ofxAssets::VIDEO).size();
					int numAudioAssets = parsedObjects[i]->getAssetDescriptorsForType(ofxAssets::AUDIO).size();

					bool rejectObject = false;
					string rejectionReason;

					//apply all policy rules to decide if object is rejected or not
					if(needsAllAssetsToBeOk){
						if(!allAssetsOK){
							rejectObject = true;
							auto brokenAssets = parsedObjects[i]->getBrokenAssets();
							rejectionReason = ofToString(brokenAssets.size()) + " Broken Asset(s)";
						}
					}

					if(numImgAssets < objectUsagePolicy.minNumberOfImageAssets){
						rejectObject = true;
						if(rejectionReason.size()) rejectionReason += " | ";
						rejectionReason += "Not Enough Images";
						ofLogError("ofxAppContent") << "Rejecting Object '" << parsedObjects[i]->getObjectUUID()
							<< "' because doesnt have the min # of images! (" << numImgAssets << "/"
							<< objectUsagePolicy.minNumberOfImageAssets << ")" ;
					}

					if(numVideoAssets > objectUsagePolicy.minNumberOfVideoAssets){
						rejectObject = true;
						if(rejectionReason.size()) rejectionReason += " | ";
						rejectionReason += "Not Enough Videos";
						ofLogError("ofxAppContent") << "Rejecting Object '" << parsedObjects[i]->getObjectUUID()
						<< "' because doesnt have the min # of Videos! (" << numVideoAssets << "/"
						<< objectUsagePolicy.minNumberOfVideoAssets << ")" ;
					}

					if(numAudioAssets > objectUsagePolicy.minNumberOfAudioAssets){
						rejectObject = true;
						if(rejectionReason.size()) rejectionReason += " | ";
						rejectionReason += "Not Enough AudioFiles";
						ofLogError("ofxAppContent") << "Rejecting Object '" << parsedObjects[i]->getObjectUUID()
						<< "' because doesnt have the min # of Audio Files! (" << numAudioAssets << "/"
						<< objectUsagePolicy.minNumberOfAudioAssets << ")" ;
					}

					if (rejectObject){
						badObjects.push_back(i);
						badObjectsIds.push_back(parsedObjects[i]->getObjectUUID());
						objectsWithBadAssets += "Object '" + badObjectsIds.back() + "' : " + rejectionReason + "\n";
					}
				}

				for(int i = badObjects.size() - 1; i >= 0; i--){
					ofLogError("ofxAppContent") << "Dropping object " << parsedObjects[i]->getObjectUUID();
					delete parsedObjects[badObjects[i]];
					parsedObjects.erase(parsedObjects.begin() + badObjects[i]);
				}

				objectsWithBadAssets = "\nRemoved " + ofToString(badObjects.size()) + " \"" + ID + "\" objects:\n\n" + objectsWithBadAssets;
			}else{
				ofLogWarning("ofxAppContent") << "skipping Object Drop Policy Tests!! \"" << ID << "\"";
			}

		}break;

		case ContentState::SETUP_TEXTURED_OBJECTS:
			for(int i = 0; i < parsedObjects.size(); i++){
				auto setupTexObjUserLambda = contentCfg.setupTexturedObject;
				//call the User Supplied Lambda to setup the user's TexturedObject
				setupTexObjUserLambda( parsedObjects[i] );
			}
			setState(ContentState::JSON_CONTENT_READY);
			break;

		case ContentState::JSON_CONTENT_READY:{
			//keep the json as a good one
			ofFile jsonFile;
			jsonFile.open(jsonParser.getJsonLocalPath());
			string jsonPath = jsonParser.getJsonLocalPath();
			string dir = ofFilePath::getEnclosingDirectory(jsonPath);
			ofFilePath::createEnclosingDirectory(dir + "knownGood");
			jsonFile.moveTo(dir + "/knownGood/" + ID + ".json", false, true);
		}break;

		default: break;
	}

	string info = "\"" + ID + "\" > " + getNameForState(state);
	ofNotifyEvent(eventStateChanged, info);
}


string ofxAppContent::getLastKnownGoodJsonPath(){
	string dir = ofFilePath::getEnclosingDirectory(jsonParser.getJsonLocalPath());
	return dir + "knownGood/" + ID + ".json";
}


string ofxAppContent::getStatus(bool formatted){

	string r;
	string plainFormat = " %0.8 #0x888888 \n"; //text format for logging on screen - see ofxFontStash.h drawMultiLineColumn()
	string errorFormat = " %0.8 #0xBB0000 \n"; //text format for logging on screen - see ofxFontStash.h drawMultiLineColumn()

	switch (state) {
		case ContentState::DOWNLOADING_JSON: r = string(formatted ? plainFormat : "") + jsonParser.getHttp().drawableString(); break;
		case ContentState::JSON_DOWNLOAD_FAILED: r = string(formatted ? errorFormat : "") + errorMessage; break;
		case ContentState::CHECKING_JSON: r = string(formatted ? plainFormat : "") + jsonParser.getDrawableState(); break;
		case ContentState::PARSING_JSON: r = string(formatted ? plainFormat : "") + jsonParser.getDrawableState(); break;
		case ContentState::CHECKING_ASSET_STATUS: r = string(formatted ? plainFormat : "") + assetChecker.getDrawableState(); break;
		case ContentState::JSON_PARSE_FAILED: r = string(formatted ? errorFormat : "") +  errorMessage; break;
		case ContentState::DOWNLOADING_ASSETS: r =  string(formatted ? plainFormat : "") + dlc.getDrawableInfo(true, false); break;
		case ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS: r = string(formatted ? plainFormat : "") + objectsWithBadAssets; break;
		case ContentState::SETUP_TEXTURED_OBJECTS: r = string(formatted ? plainFormat : ""); break;
		case ContentState::JSON_CONTENT_READY: r = string(formatted ? plainFormat : "") + "READY"; break;
		default: break;
	}
	return r;
}


float ofxAppContent::getPercentDone(){
	float p = -1.0f;
	switch (state) {
		case ContentState::DOWNLOADING_JSON: p = jsonParser.getHttp().getCurrentDownloadProgress(); break;
		case ContentState::CHECKING_JSON: p = -1.0; break;
		case ContentState::PARSING_JSON: p = jsonParser.getTotalProgress(); break;
		case ContentState::CHECKING_ASSET_STATUS: p = assetChecker.getProgress(); break;
		case ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS: p = -1; break;
		case ContentState::DOWNLOADING_ASSETS:
			p = 1.0 - float(dlc.getNumPendingDownloads()) / totalAssetsToDownload;
			break;
		default: break;
	}
	return p;
}

bool ofxAppContent::isReadyToFetchContent(){
	return	state == ContentState::IDLE ||
			state == ContentState::JSON_PARSE_FAILED ||
			state == ContentState::JSON_DOWNLOAD_FAILED ||
			state == ContentState::JSON_CONTENT_READY;
}


bool ofxAppContent::foundError(){
	return	state == ContentState::JSON_DOWNLOAD_FAILED ||
			state == ContentState::JSON_PARSE_FAILED;
};


bool ofxAppContent::isContentReady(){
	return state == ContentState::JSON_CONTENT_READY;
};


// CALBACKS ////////////////////////////////////////////////////////////////////////////////////
#pragma mark Callbacks

void ofxAppContent::jsonDownloaded(ofxSimpleHttpResponse & arg){
	ofLogNotice("ofxAppContent") << "JSON download OK!";
	setState(ContentState::CHECKING_JSON);
	OFXAPP_REPORT("ofxAppJsonDownloadFailed", "JSON Download OK for '" + ID + "'! \"" + jsonURL + "\"", 0);
}


void ofxAppContent::jsonDownloadFailed(ofxSimpleHttpResponse & arg){
	ofLogError("ofxAppContent") << "JSON download failed!";
	errorMessage = arg.reasonForStatus + " (" + arg.url + ")";
	OFXAPP_REPORT("ofxAppJsonDownloadFailed", "JSON Download Failed for '" + ID + "'! \"" + jsonURL + "\"\nreason: " + arg.reasonForStatus , 2);
	setState(ContentState::JSON_DOWNLOAD_FAILED);
}


void ofxAppContent::jsonInitialCheckOK(){
	ofLogNotice("ofxAppContent") << "JSON Initial Check OK!";
	OFXAPP_REPORT("ofxAppJsonParseFailed", "JSON Parse OK '" + ID + "'! \"" + jsonURL + "\"", 0);
	setState(ContentState::PARSING_JSON);
}


void ofxAppContent::jsonParseFailed(){
	ofLogError("ofxAppContent") << "JSON Parse Failed!";
	OFXAPP_REPORT("ofxAppJsonParseFailed", "JSON Parse Failed for '" + ID + "'! \"" + jsonURL + "\"" , 2);
	setState(ContentState::JSON_PARSE_FAILED);
}


void ofxAppContent::jsonContentReady(vector<ParsedObject*> &parsedObjects_){
	ofLogNotice("ofxAppContent") << "JSON Content Ready! " << parsedObjects_.size() << " Objects received.";
	parsedObjects.reserve(parsedObjects_.size());
	for(auto o : parsedObjects_){
		//parsedObjects.push_back((ContentObject*)o);		
		//ContentObject * co = static_cast<ContentObject*>(o);
		ContentObject * co = (ContentObject*)(o);

		parsedObjects.push_back(co);
	}
	setState(ContentState::CHECKING_ASSET_STATUS);
}


void ofxAppContent::assetCheckFinished(){
	ofLogNotice("ofxAppContent") << "Asset Check Finished!";
	setState(ContentState::DOWNLOADING_ASSETS);
}


string ofxAppContent::getNameForState(ofxAppContent::ContentState state){

	switch (state) {
		case ContentState::IDLE: return "IDLE";
		case ContentState::DOWNLOADING_JSON: return "DOWNLOADING_JSON";
		case ContentState::JSON_DOWNLOAD_FAILED: return "JSON_DOWNLOAD_FAILED";
		case ContentState::CHECKING_JSON: return "CHECKING_JSON";
		case ContentState::JSON_PARSE_FAILED: return "JSON_PARSE_FAILED";
		case ContentState::PARSING_JSON: return "PARSING_JSON";
		case ContentState::CHECKING_ASSET_STATUS: return "CHECKING_ASSET_STATUS";
		case ContentState::DOWNLOADING_ASSETS: return "DOWNLOADING_ASSETS";
		case ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS: return "FILTER_OBJECTS_WITH_BAD_ASSETS";
		case ContentState::SETUP_TEXTURED_OBJECTS: return "SETUP_TEXTURED_OBJECTS";
		case ContentState::JSON_CONTENT_READY: return "JSON_CONTENT_READY";
		default: break;
	}
	return "UNKNOWN STATE";
}
