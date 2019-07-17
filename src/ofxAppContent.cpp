//
//  ofxAppContent.cpp
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#include "ofxAppContent.h"
#include "ofxAppUtils.h"
#include "ofxApp.h"
#include "ofxChecksum.h"
#include "ofxAppErrorReporter.h"
#include "ofxGoogleAnalytics.h"

void ofxAppContent::setup(	std::string ID,
							std::string jsonSrc,
							std::string jsonDestinationDir_,
							int numThreads_,
							int numConcurrentDownloads,
							int speedLimitKBs,
							int timeoutDownloads,
						  	int timeoutApiEndpoint,
							bool shouldSkipObjectTests,
							float idleTimeAfterEachDownload,
						    const std::pair<std::string,std::string> & downloaderCredentials,
						  	ofxChecksum::Type checksumType,
						  	const ofxSimpleHttp::ProxyConfig & downloaderProxyConfig,
							const std::pair<std::string,std::string> & apiEndPointCredentials,
							const ofxSimpleHttp::ProxyConfig & apiEndpointProxyConfig,
						  	const map<std::string, std::string> customHeaders,
							const ofxApp::ParseFunctions & contentCfg,
							const ofxAssets::DownloadPolicy assetDownloadPolicy,
						  	const ofxAssets::UsagePolicy assetUsagePolicy,
							const ofxAssets::ObjectUsagePolicy & objectUsagePolicy,
							const std::string & assetsLocationPath,
						  	bool skipChecksumTests,
						  	float assetErrorsScreenReportTimeSeconds){

	state = ContentState::IDLE;
	parsedObjects.clear();
	this->ID = ID;
	this->assetErrorsScreenReportTimeSeconds = assetErrorsScreenReportTimeSeconds;
	this->jsonURL = jsonSrc;
	this->contentCfg = contentCfg;
	this->assetDownloadPolicy = assetDownloadPolicy;
	this->assetUsagePolicy = assetUsagePolicy;
	this->objectUsagePolicy = objectUsagePolicy;
	this->jsonDestinationDir = jsonDestinationDir_;
	this->numThreads = numThreads_;
	this->shouldSkipObjectTests = shouldSkipObjectTests;
	this->assetsLocationPath = assetsLocationPath;
	this->shouldSkipSha1Tests = skipChecksumTests;
	if(skipChecksumTests){
		ofLogWarning("ofxAppContent-" + ID) << "Running with skipChecksumTests == TRUE! Never run in this mode in production!";
	}

	//config the http downloader if you need to (proxy, etc)
	dlc.setMaxConcurrentDownloads(numConcurrentDownloads);
	dlc.setSpeedLimit(speedLimitKBs);
	dlc.setCopyBufferSize(512);
	dlc.setTimeOut(timeoutDownloads);
	dlc.setIdleTimeAfterEachDownload(idleTimeAfterEachDownload);
	dlc.setCredentials(downloaderCredentials.first, downloaderCredentials.second);
	dlc.setProxyConfiguration(downloaderProxyConfig);
	dlc.setChecksumType(checksumType);

	jsonParser.getHttp().setTimeOut(timeoutApiEndpoint);
	jsonParser.getHttp().setSpeedLimit(speedLimitKBs);
	jsonParser.getHttp().setCopyBufferSize(512); //kb
	jsonParser.getHttp().setProxyConfiguration(apiEndpointProxyConfig);
	if(apiEndPointCredentials.first.size() || apiEndPointCredentials.second.size()){
		jsonParser.getHttp().setCredentials(apiEndPointCredentials.first, apiEndPointCredentials.second);
	}

	for(auto & h : customHeaders){
		jsonParser.getHttp().addCustomHttpHeader(h.first, h.second);
	}

	//subscribe to parsing events
	ofAddListener(jsonParser.eventJsonDownloaded, this, 	&ofxAppContent::onJsonDownloaded);
	ofAddListener(jsonParser.eventJsonDownloadFailed, this, &ofxAppContent::onJsonDownloadFailed);
	ofAddListener(jsonParser.eventJsonInitialCheckOK, this, &ofxAppContent::onJsonInitialCheckOK);
	ofAddListener(jsonParser.eventJsonParseFailed, this, 	&ofxAppContent::onJsonParseFailed);
	ofAddListener(jsonParser.eventAllObjectsParsed, this, 	&ofxAppContent::onJsonContentReady);
}


ofxAppContent::~ofxAppContent(){
	ofLogNotice("ofxAppContent-" + ID) << "~ofxAppContent";
}


void ofxAppContent::setNumThreads(int nThreads){
	numThreads = MAX(nThreads, 1);
}

void ofxAppContent::setMaxConcurrentDownloads(int nDownloads){
	dlc.setMaxConcurrentDownloads(nDownloads);
}

void ofxAppContent::setJsonDownloadURL(std::string jsonURL){
	ofLogNotice("ofxAppContent-" + ID) << "updating the JSON Content URL of " << ID << " to '" << jsonURL << "'";
	this->jsonURL = jsonURL;
};


void ofxAppContent::fetchContent(){
	if(state == ContentState::IDLE ||
	   state == ContentState::JSON_PARSE_FAILED ||
	   state == ContentState::JSON_DOWNLOAD_FAILED ||
	   state == ContentState::JSON_CONTENT_READY
	   ){
		parsedObjects.clear(); //FIXME: here we are potentially leaking!
		setState(ContentState::DOWNLOADING_JSON);
	}else{
		ofLogError("ofxAppContent-" + ID) << "Can't fetch content now!";
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

		case ContentState::CATALOG_ASSETS:
			if(!isThreadRunning()){
				if(shouldSkipSha1Tests){
					setState(ContentState::SETUP_TEXTURED_OBJECTS);
				}else{
					setState(ContentState::CHECKING_ASSET_STATUS);
				}
			}
			break;

		case ContentState::REMOVING_EXPIRED_ASSETS:{
			if(!isThreadRunning()){
				ofLogNotice("ofxAppContent-" + ID) << "Finished Removing Expired assets.";
				setState(ContentState::DOWNLOADING_ASSETS);
			}
			}break;

		case ContentState::DOWNLOADING_ASSETS:
			if(!dlc.isBusy()){ //downloader finished!
				ofLogNotice("ofxAppContent-" + ID) << "Finished Asset downloads for \"" << ID << "\"!";
				setState(ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS);
			}break;

		case ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS:
			if(timeInState > (numIgnoredObjects > 0 ? assetErrorsScreenReportTimeSeconds : 0.0)){ //show this on screen for a sec if we are dropping objects
				setState(ContentState::SETUP_TEXTURED_OBJECTS);
			}
			break;

		case ContentState::SETUP_TEXTURED_OBJECTS:{
			int maxObjectsToSetupInOneFrame = 50; //stagger across frames, to avoid hogging the app
			int start = numSetupTexuredObjects;
			int end = MIN(parsedObjects.size(), numSetupTexuredObjects + maxObjectsToSetupInOneFrame);
			for(int i = start; i < end; i++){
				//call the User Supplied Lambda to setup the user's TexturedObject
				contentCfg.setupTexturedObject( parsedObjects[i] );
				numSetupTexuredObjects++;
			}
			if(numSetupTexuredObjects == parsedObjects.size()){
				setState(ContentState::FILTER_REJECTED_TEXTURED_OBJECTS);
			}
			}break;

		case ContentState::FILTER_REJECTED_TEXTURED_OBJECTS:{
			if(timeInState > (numIgnoredObjects > 0 ? assetErrorsScreenReportTimeSeconds : 0.0)){ //show this on screen for a sec if we are dropping objects
				setState(ContentState::JSON_CONTENT_READY);
			}
		}

		default: break;
	}
}


void ofxAppContent::threadedFunction(){

	#ifdef TARGET_WIN32
	#elif defined(TARGET_LINUX)
	pthread_setname_np(pthread_self(), string("ofxAppContent " + ID).c_str());
	#else
	pthread_setname_np(string("ofxAppContent " + ID).c_str());
	#endif

	switch (state){
		case ContentState::CATALOG_ASSETS:{
			ofxApp::CatalogAssetsData d;
			d.userData = &contentCfg.userData;
			d.assetsLocation = assetsLocationPath;
			d.assetUsagePolicy = assetUsagePolicy;
			d.assetDownloadPolicy = assetDownloadPolicy;
			for(auto co : parsedObjects){
				d.object = co;
				contentCfg.defineObjectAssets(d);
			}
		}break;

		case ContentState::REMOVING_EXPIRED_ASSETS:{
			if(assetsLocationPath.size()){
				removeExpiredAssets();
			}
		}break;

		default: break;
	}
	ofSleepMillis(45); 	//this is a shameful workaround to overcome the bug where too-short-lived threads cause expcetions on windows.
						//https://github.com/openframeworks/openFrameworks/issues/5262
						//content with no asset will create this condition.
						//FIXME: transition out of ofThread into std::thread/async.
}


void ofxAppContent::removeExpiredAssets(){

	//accumulate all assets that should be in the filesystem
	//note all paths are turned to absolute
	vector<string> allExpectedAssets;
	for(auto co : parsedObjects){
		auto assets = co->getAllAssetsInDB();
		for(auto & ad : assets){
			allExpectedAssets.push_back(ofToDataPath(ad.relativePath, true));
		}
	}

	if (ofDirectory::doesDirectoryExist(assetsLocationPath)){

		//build a list of files that exist in
		ofDirectory assetsDir;
		vector<string> allFilesOnAssetFolder; //store all files existing on assets dir
		vector<string> allEmptyDirs; //store all dirs with 0 files inside for deletion

		assetsDir.listDir(assetsLocationPath);
		for(int i = 0; i < assetsDir.numFiles(); i++){
			if(assetsDir.getFile(i).isDirectory()){ //if each objectID has its assets in a dir
				ofDirectory objDir;
				objDir.listDir(assetsDir.getPath(i));
				if(objDir.numFiles() == 0){
					allEmptyDirs.push_back(ofToDataPath(assetsDir.getPath(i), true));
				}else{
					for(int j = 0; j < objDir.numFiles(); j++){
						allFilesOnAssetFolder.push_back(ofToDataPath(objDir.getPath(j), true));
					}
				}
			}else{ //all assets from all objects mixed in the assets dir
				allFilesOnAssetFolder.push_back(ofToDataPath(assetsDir.getPath(i), true));
			}
		}

		for(auto & dir : allEmptyDirs){
			ofLogWarning("ofxAppContent-" + ID) << "removing empty directory at \"" << ofToDataPath(dir) << "\"";
			ofDirectory::removeDirectory(dir, true, false);
		}
		for(auto & file : allFilesOnAssetFolder){
			auto it = std::find(allExpectedAssets.begin(), allExpectedAssets.end(), file);
			if (it == allExpectedAssets.end()){ //this file on disk is not in the expected asset file list, delete!
				ofLogWarning("ofxAppContent-" + ID) << "removing expired asset at \"" << ofToDataPath(file) << "\"";
				ofFile::removeFile(file, false);
			}
		}
	}
}


void ofxAppContent::setShouldRemoveExpiredAssets(bool set){
	shouldRemoveExpiredAssets = set;
}


void ofxAppContent::setState(ContentState s){

	state = s;
	timeInState = 0;

	switch (s) {

		case ContentState::DOWNLOADING_JSON:{

			//start the download and parse process
			contentCfg.userData["jsonURL"] = jsonURL;
			contentCfg.userData["jsonDestinationDir"] = jsonDestinationDir;
			jsonParser.downloadAndParse(jsonURL,
										jsonDestinationDir,	//directory where to save
										numThreads,			//num threads
										contentCfg.pointToObjects,
										contentCfg.parseOneObject,
										contentCfg.userData
										);
			}
			break;

		case ContentState::CATALOG_ASSETS:
			startThread();
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
				ofLogWarning("ofxAppContent-" + ID) << "There are ZERO parsed objects!";
				setState(ContentState::JSON_CONTENT_READY);
			}

			}break;

		case ContentState::REMOVING_EXPIRED_ASSETS:
			ofLogNotice("ofxAppContent-" + ID) << "Start expired asset removal phase.";
			startThread();
			break;

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

			int numObjectB4Filter = parsedObjects.size();

			if(!shouldSkipObjectTests){
				objectsWithBadAssets.clear();

				vector<int> badObjects;
				vector<std::string> badObjectsIds;

				for(int i = 0; i < parsedObjects.size(); i++){

					//do some asset integrity tests...
					bool allAssetsOK = parsedObjects[i]->areAllAssetsOK();
					bool needsAllAssetsToBeOk = objectUsagePolicy.allObjectAssetsAreOK;
					int numImgAssets = parsedObjects[i]->getAssetDescriptorsForType(ofxAssets::IMAGE).size();
					int numVideoAssets = parsedObjects[i]->getAssetDescriptorsForType(ofxAssets::VIDEO).size();
					int numAudioAssets = parsedObjects[i]->getAssetDescriptorsForType(ofxAssets::AUDIO).size();

					bool rejectObject = false;
					std::string rejectionReason;

					//apply all policy rules to decide if object is rejected or not
					if(needsAllAssetsToBeOk){
						if(!allAssetsOK){
							rejectObject = true;
							auto brokenAssets = parsedObjects[i]->getBrokenAssets();
							if(rejectionReason.size()) rejectionReason += " | ";
							rejectionReason += ofToString(brokenAssets.size()) + " Broken Asset(s)";
						}
					}

					if(numImgAssets < objectUsagePolicy.minNumberOfImageAssets){
						rejectObject = true;
						if(rejectionReason.size()) rejectionReason += " | ";
						rejectionReason += "Not Enough Images";
						ofLogError("ofxAppContent-" + ID) << "Rejecting Object '" << parsedObjects[i]->getObjectUUID()
							<< "' because doesnt have the min # of images! (" << numImgAssets << "/"
							<< objectUsagePolicy.minNumberOfImageAssets << ")" ;
					}

					if(numVideoAssets < objectUsagePolicy.minNumberOfVideoAssets){
						rejectObject = true;
						if(rejectionReason.size()) rejectionReason += " | ";
						rejectionReason += "Not Enough Videos";
						ofLogError("ofxAppContent-" + ID) << "Rejecting Object '" << parsedObjects[i]->getObjectUUID()
						<< "' because doesnt have the min # of Videos! (" << numVideoAssets << "/"
						<< objectUsagePolicy.minNumberOfVideoAssets << ")" ;
					}

					if(numAudioAssets < objectUsagePolicy.minNumberOfAudioAssets){
						rejectObject = true;
						if(rejectionReason.size()) rejectionReason += " | ";
						rejectionReason += "Not Enough AudioFiles";
						ofLogError("ofxAppContent-" + ID) << "Rejecting Object '" << parsedObjects[i]->getObjectUUID()
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
					ofLogError("ofxAppContent-" + ID) << "Dropping object \"" << parsedObjects[i]->getObjectUUID() << "\"";
					delete parsedObjects[badObjects[i]];
					parsedObjects.erase(parsedObjects.begin() + badObjects[i]);
				}

				numIgnoredObjects += badObjects.size();
				objectsWithBadAssets = "\nRemoved " + ofToString(badObjects.size()) + " \"" + ID + "\" objects:\n\n" + objectsWithBadAssets + "\n\n" ;

			}else{
				ofLogWarning("ofxAppContent-" + ID) << "skipping Object Drop Policy Tests!! \"" << ID << "\"";
			}

			}break;

			
		case ContentState::SETUP_TEXTURED_OBJECTS:{
			numSetupTexuredObjects = 0;
			}break;


		case ContentState::FILTER_REJECTED_TEXTURED_OBJECTS:{
			int numObjectB4Filter = parsedObjects.size();

			vector<int> badObjects;
			vector<std::string> badObjectsIds;
			string log;

			for(int i = 0; i < numObjectB4Filter; i++){

				bool userRejectedObject = !parsedObjects[i]->isValid;

				if (userRejectedObject){
					badObjects.push_back(i);
					badObjectsIds.push_back(parsedObjects[i]->getObjectUUID());
					log += "Object '" + badObjectsIds.back() + "' : Rejected at Setup Textured Object stage - probably cant load img\n";
				}
			}

			for(int i = badObjects.size() - 1; i >= 0; i--){
				ofLogError("ofxAppContent-" + ID) << "Dropping object at setup Textured Object Stage \"" << parsedObjects[i]->getObjectUUID() << "\"";
				delete parsedObjects[badObjects[i]];
				parsedObjects.erase(parsedObjects.begin() + badObjects[i]);
			}

			numIgnoredObjects += badObjects.size();

			objectsWithBadAssets += "Setup Textured Object Statge\n\nRemoved " + ofToString(badObjects.size()) + " \"" + ID + "\" objects:\n\n" + log;

			ofLogWarning("ofxAppContent-" + ID) << "Removed a total of " << numIgnoredObjects << " objects for content type \"" << ID << "\" due to various rasons. Check 'logs/assetStatus.log' for more info.";
			if(numIgnoredObjects > 0) ofLogWarning("ofxAppContent-" + ID) << objectsWithBadAssets;
			if(numIgnoredObjects > 0){
				auto a = ofxApp::get().analytics();
				if(a && a->isEnabled()){
					a->sendException("ofxApp - Content '" + ID + "' - rejected " + ofToString(numIgnoredObjects) + " objects.", false);
				}
			}
			float pct;
			if(numObjectB4Filter > 0){
				pct = 100.0f * numIgnoredObjects / float(numObjectB4Filter);
			}else{
				pct = 0.0f;
			}
			ofLogWarning("ofxAppContent-" + ID) << "Ignored " << ofToString(pct,2) << "% of the objects defined in the \"" << ID << "\" JSON.";

		}break;

		case ContentState::JSON_CONTENT_READY:{
			//keep the json as a good one
			ofFile jsonFile;
			jsonFile.open(jsonParser.getJsonLocalPath());
			std::string jsonPath = jsonParser.getJsonLocalPath();
			std::string dir = ofFilePath::getEnclosingDirectory(jsonPath);
			ofFilePath::createEnclosingDirectory(dir + "knownGood");
			std::string oldJsonPath = dir + "/knownGood/" + ID + ".json";

			//calc sha1 for the last konwn json, and the fresh one
			newJsonSha1 = ofxChecksum::calcSha1(jsonParser.getJsonLocalPath());
			if(ofFile::doesFileExist(oldJsonPath)){
				oldJsonSha1 = ofxChecksum::calcSha1(oldJsonPath);
			}

			//replace the old json with the fresh one
			jsonFile.moveTo(oldJsonPath, false, true);
			}break;

		default: break;
	}

	std::string info = "\"" + ID + "\" > " + getNameForState(state);
	if (shouldSkipSha1Tests) info += " - SKIPPING CHECKSUM TESTS!";
	ofNotifyEvent(eventStateChanged, info);
}


std::string ofxAppContent::getLastKnownGoodJsonPath(){
	std::string dir = ofFilePath::getEnclosingDirectory(jsonParser.getJsonLocalPath());
	return dir + "knownGood/" + ID + ".json";
}


std::string ofxAppContent::getStatus(){

	std::string r;
	switch (state) {
		case ContentState::DOWNLOADING_JSON: r = jsonParser.getHttp().drawableString(); break;
		case ContentState::JSON_DOWNLOAD_FAILED: r = errorMessage; break;
		case ContentState::CHECKING_JSON: r = jsonParser.getDrawableState(); break;
		case ContentState::PARSING_JSON: r = jsonParser.getDrawableState(); break;
		case ContentState::CATALOG_ASSETS: break;
		case ContentState::CHECKING_ASSET_STATUS: r = assetChecker.getDrawableState(); break;
		case ContentState::JSON_PARSE_FAILED: r = errorMessage; break;
		case ContentState::DOWNLOADING_ASSETS: r = dlc.getDrawableInfo(true, false); break;
		case ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS: r = objectsWithBadAssets; break;
		case ContentState::SETUP_TEXTURED_OBJECTS: break;
		case ContentState::FILTER_REJECTED_TEXTURED_OBJECTS: r = objectsWithBadAssets; break;
		case ContentState::JSON_CONTENT_READY: r = "READY"; break;
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
		case ContentState::CATALOG_ASSETS: p = -1; break;
		case ContentState::CHECKING_ASSET_STATUS: p = assetChecker.getProgress(); break;
		case ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS: p = -1; break;
		case ContentState::SETUP_TEXTURED_OBJECTS: p = numSetupTexuredObjects / float(parsedObjects.size()); break;
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

void ofxAppContent::onJsonDownloaded(ofxSimpleHttpResponse & arg){
	ofLogNotice("ofxAppContent-" + ID) << "JSON download OK! \"" << jsonURL << "\"";
	setState(ContentState::CHECKING_JSON);
	OFXAPP_REPORT("ofxAppJsonDownloadFailed", "JSON Download OK for '" + ID + "'! \"" + jsonURL + "\"", 0);
}


void ofxAppContent::onJsonDownloadFailed(ofxSimpleHttpResponse & arg){
	ofLogError("ofxAppContent-" + ID) << "JSON download failed! \"" << jsonURL << "\"";
	errorMessage = arg.reasonForStatus + " (" + arg.url + ")";
	OFXAPP_REPORT("ofxAppJsonDownloadFailed", "JSON Download Failed for '" + ID + "'! \"" + jsonURL + "\"\nreason: " + arg.reasonForStatus , 2);
	setState(ContentState::JSON_DOWNLOAD_FAILED);
}


void ofxAppContent::onJsonInitialCheckOK(){
	ofLogNotice("ofxAppContent-" + ID) << "JSON Initial Check OK! \"" << jsonURL << "\"";
	OFXAPP_REPORT("ofxAppJsonParseFailed", "JSON Parse OK '" + ID + "'! \"" + jsonURL + "\"", 0);
	setState(ContentState::PARSING_JSON);
}


void ofxAppContent::onJsonParseFailed(){
	ofLogError("ofxAppContent-" + ID) << "JSON Parse Failed! \"" << jsonURL << "\"";
	OFXAPP_REPORT("ofxAppJsonParseFailed", "JSON Parse Failed for '" + ID + "'! \"" + jsonURL + "\"" , 2);
	errorMessage = "Json parse of \"" + jsonURL + "\" failed!";
	setState(ContentState::JSON_PARSE_FAILED);
}


void ofxAppContent::onJsonContentReady(vector<ParsedObject*> &parsedObjects_){
	ofLogNotice("ofxAppContent-" + ID) << "JSON Content Ready! " << parsedObjects_.size() << " Objects received.";
	numIgnoredObjects += jsonParser.getNumEntriesInJson() - parsedObjects_.size();
	parsedObjects.reserve(parsedObjects_.size());
	for(auto o : parsedObjects_){
		//ContentObject * co = static_cast<ContentObject*>(o);
		ContentObject * co = (ContentObject*)(o);
		parsedObjects.push_back(co);
	}
	setState(ContentState::CATALOG_ASSETS);
}


void ofxAppContent::assetCheckFinished(){
	ofLogNotice("ofxAppContent-" + ID) << "Asset Check Finished!";
	if(shouldRemoveExpiredAssets){
		setState(ContentState::REMOVING_EXPIRED_ASSETS);
	}else{
		setState(ContentState::DOWNLOADING_ASSETS);
	}
}


std::string ofxAppContent::getNameForState(ofxAppContent::ContentState state){

	switch (state) {
		case ContentState::IDLE: return "IDLE";
		case ContentState::DOWNLOADING_JSON: return "DOWNLOADING_JSON";
		case ContentState::JSON_DOWNLOAD_FAILED: return "JSON_DOWNLOAD_FAILED";
		case ContentState::CHECKING_JSON: return "CHECKING_JSON";
		case ContentState::JSON_PARSE_FAILED: return "JSON_PARSE_FAILED";
		case ContentState::PARSING_JSON: return "PARSING_JSON";
		case ContentState::CATALOG_ASSETS: return "CATALOG_ASSETS";
		case ContentState::CHECKING_ASSET_STATUS: return "CHECKING_ASSET_STATUS";
		case ContentState::REMOVING_EXPIRED_ASSETS: return "REMOVING_EXPIRED_ASSETS";
		case ContentState::DOWNLOADING_ASSETS: return "DOWNLOADING_ASSETS";
		case ContentState::FILTER_OBJECTS_WITH_BAD_ASSETS: return "FILTER_OBJECTS_WITH_BAD_ASSETS";
		case ContentState::SETUP_TEXTURED_OBJECTS: return "SETUP_TEXTURED_OBJECTS";
		case ContentState::FILTER_REJECTED_TEXTURED_OBJECTS: return "FILTER_REJECTED_TEXTURED_OBJECTS";
		case ContentState::JSON_CONTENT_READY: return "JSON_CONTENT_READY";
		default: break;
	}
	return "UNKNOWN STATE";
}
