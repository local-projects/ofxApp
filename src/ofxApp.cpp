//
//  ofxApp.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesi√† on 3/8/16.
//
//

#include "ofxApp.h"
#include "ofxThreadSafeLog.h"
#include "TexturedObjectStats.h"

ofxApp::App app; //app global in your project!

using namespace ofxApp;

App::App() {
	cout << "ofxApp::App()";
}

void App::setup(ofxAppDelegate * delegate){

	map<string,ofxApp::UserLambdas> emptyLambas;
	setup(emptyLambas, delegate);
}

void App::setup(const map<string,ofxApp::UserLambdas> & cfgs, ofxAppDelegate * delegate){

	ofLogNotice("ofxApp") << "setup()";
	if(!this->delegate){
		contentCfgs = cfgs;
		this->delegate = delegate;
		if(!hasLoadedSettings) loadSettings();
		fontStorage = new ofxAppFonts();
		setupContentData();
		setupLogging();
		printSettingsFile();
		fonts().setup();
		if(getBool("logging/useFontStash")){ //set a nice font for the on screen logger if using fontstash
			ofxSuperLog::getLogger()->setFont(&(fonts().getMonoBoldFont()), getInt("logging/fontSize"));
		}
		loadModulesSettings();
		if(timeSampleOfxApp) TS_START_NIF("ofxApp Setup");
		setupTimeMeasurements();
		setupTextureLoader();
		setupWindow();
		ofxSimpleHttp::createSslContext();
		setupStateMachine();
		appState.setState(SETTING_UP);
		setupListeners();
		setupRemoteUI();
		globals().setupRemoteUIParams();
		colors().setupRemoteUIParams();
		textures().setup();
		setupTuio();
		setupOF();
		if(timeSampleOfxApp) TS_START_NIF("ofxApp Load Static Textures");
		appState.setState(LOADING_STATIC_TEXTURES); //start loading content
	}else{
		ofxApp::terminateApp("ofxApp", "Trying to setup() ofxApp a second time!");
	}
}


App::~App(){
	//cout << (*loggerStorage).use_count() << endl;
	ofLogNotice("ofxApp")<< "~ofxApp()";
}


void App::setupContentData() {
	for (auto & cfg : contentCfgs) {
		contentStorage[cfg.first] = new ofxAppContent();
		requestedContent.push_back(cfg.first);
	}
	if(requestedContent.size()){
		currentContentID = requestedContent[0];
	}
}


void App::setupOF(){
	ofSetFrameRate(getInt("App/frameRate"));
	ofBackground(22);
	dt = 1.0f / ofGetTargetFrameRate();

	bool showMouse = getBool("App/showMouse");
	if(showMouse) ofShowCursor();
	else ofHideCursor();

	setMouseEvents(getBool("App/enableMouse"));
}

void App::setMouseEvents(bool enabled){
	if(enabled){
		if(!ofEvents().mousePressed.isEnabled()){
			ofEvents().mousePressed.enable();
			ofEvents().mouseReleased.enable();
			ofEvents().mouseDragged.enable();
			ofLogWarning("ofxApp") << "Enabled Mouse Events";
		}
	}else{
		if(ofEvents().mousePressed.isEnabled()){
			ofEvents().mousePressed.disable();
			ofEvents().mouseReleased.disable();
			ofEvents().mouseDragged.disable();
			ofLogWarning("ofxApp") << "Disabled Mouse Events";
		}
	}
}


void App::setupWindow(){
	ofxScreenSetup::ScreenMode mode = ofxScreenSetup::ScreenMode((int)getInt("App/window/windowMode"));
	screenSetup.setup(getInt("App/window/customWidth"), getInt("App/window/customHeight"), mode);

	bool customPosition = getBool("App/window/customWindowPosition");
	int customX = getInt("App/window/customPositionX");
	int customY = getInt("App/window/customPositionY");
	if(customPosition){
		ofLogNotice("ofxApp") << "Setting a custom window position [" << customX << " , " << customY << "]";
		ofSetWindowPosition(customX, customY);
	}

	//setup mullions user settings
	bool mullionsVisible = getBool("App/mullions/visibleAtStartup");
	mullions.setup(getInt("App/mullions/numX"), getInt("App/mullions/numY"));
	if(mullionsVisible) mullions.enable();
	else mullions.disable();
	
	//trying to get the window to "show up" in the 1st frame - to show terminateApp() in the 1st frame
	GLFWwindow* glfwWindow = (GLFWwindow*)ofGetWindowPtr()->getWindowContext();
	glfwShowWindow(glfwWindow);
	//ofGetWindowPtr()->setFullscreen(true);
	//ofSetWindowPosition(0,0);
	ofGetWindowPtr()->makeCurrent();
	ofGetGLRenderer()->startRender();
	ofGetGLRenderer()->setupScreen();
	ofGetGLRenderer()->finishRender();
//	ofGetWindowPtr()->update();
//	ofGetWindowPtr()->draw();
	ofGetMainLoop()->pollEvents();
}


void App::setupListeners(){

	ofAddListener(ofEvents().update, this, &App::update);
	ofAddListener(ofEvents().exit, this, &App::exit, OF_EVENT_ORDER_AFTER_APP);
	//listen to content manager state changes
	for(auto c : contentStorage){
		ofAddListener(c.second->eventStateChanged, this, &App::onContentManagerStateChanged);
	}

	ofAddListener(ofEvents().keyPressed, this, &App::onKeyPressed);
	ofAddListener(ofEvents().draw, this, &App::draw, OF_EVENT_ORDER_AFTER_APP);
	ofAddListener(textures().eventAllTexturesLoaded, this, &App::onStaticTexturesLoaded);
}


void App::setupStateMachine(){

	//listen to state machine changes
	ofAddListener(appState.eventStateChanged, this, &App::onStateChanged);
	ofAddListener(appState.eventStateError, this, &App::onStateError);
	ofAddListener(appState.eventDraw, this, &App::onDrawLoadingScreenStatus);

	string boldFontPath = getString("Fonts/ofxApp/monospacedBold/fontFile");
	ofxApp::assertFileExists(boldFontPath);
	appState.setup(boldFontPath, "", ofColor::black, ofColor::white);
	//this creates strings for each of the ENUM states
	float dark = 0.333;
	appState.SET_NAME_AND_COLOR_FOR_STATE(SETTING_UP, ofColor(0,0,255), ofColor(0,0,128));
	appState.SET_NAME_AND_COLOR_FOR_STATE(LOADING_STATIC_TEXTURES, ofColor::darkorange, ofColor::darkorange * dark);
	appState.SET_NAME_AND_COLOR_FOR_STATE(LOADING_JSON_CONTENT, ofColor::lawnGreen, ofColor::lawnGreen * dark);
	appState.SET_NAME_AND_COLOR_FOR_STATE(LOADING_JSON_CONTENT_FAILED, ofColor::crimson, ofColor::crimson * dark);
	appState.SET_NAME_AND_COLOR_FOR_STATE(LOAD_CUSTOM_USER_CONTENT, ofColor::blueViolet, ofColor::blueViolet * dark);
	appState.SET_NAME_AND_COLOR_FOR_STATE(SETUP_USER_APP, ofColor::royalBlue, ofColor::royalBlue * dark);
	appState.SET_NAME_AND_COLOR_FOR_STATE(POST_USER_SETUP, ofColor::mediumAquaMarine, ofColor::mediumAquaMarine * dark);
	appState.SET_NAME_AND_COLOR_FOR_STATE(RUNNING, ofColor::white, ofColor::grey);
}

void App::startLoadingStaticAssets(){
	string texturesPath = getString("StaticAssets/textures");
	if(texturesPath.size()){
		ofxApp::assertFileExists(texturesPath);
		textures().loadTexturesInDir(texturesPath, getInt("App/maxThreads"));
	}else{
		ofLogWarning("ofxApp") << "App doesnt want to load static Assets!";
		onStaticTexturesLoaded();
	}
}


void App::setupTextureLoader(){

	ProgressiveTextureLoadQueue * q = ProgressiveTextureLoadQueue::instance();
	q->setNumberSimultaneousLoads( getInt("textureLoader/maxNumberSimulataneousLoads") ); //N threads loading images in the bg
	q->setTexLodBias( getFloat("textureLoader/textureLodBias") ); //MipMap sharpness
	q->setTargetTimePerFrame( getFloat("textureLoader/maxTimeSpentLoadingPerFrameMs") );	//spend at most 'x' milis loading textures per frame
	q->setScanlinesPerLoop( getInt("textureLoader/scanlinesPerLoop") );
	q->setMaximumRequestsPerFrame( getInt("textureLoader/maxLoadRequestsPerFrame") );

}


void App::loadSettings(){

	ofxApp::assertFileExists(settingsFile);

	ofLogNotice("ofxApp") << "loadSettings() from \"" << settingsFile << "\"";
	bool ok = settings().load(ofToDataPath(settingsFile, true));
	if(!ok){
		ofxApp::terminateApp("ofxApp", "Could not load settings from \"" + ofToDataPath(settingsFile, true) + "\"");
	}
	hasLoadedSettings = true;
}


void App::printSettingsFile(){

	string settingsString = settings().getAsJsonString();
	logBanner("Loaded ofxApp Settings - JSON Contents follow :");
	vector<string> jsonLines = ofSplitString(settingsString, "\n");
	#ifdef TARGET_WIN32
	ofLogNotice("ofxApp") << " %%%%%% AppSettings.json %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";
	for (auto & l : jsonLines) {
		ofLogNotice("ofxApp") << " % " << l;
	}
	ofLogNotice("ofxApp") << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";
	#else
	ofLogNotice("ofxApp") << " ╔═════╣ AppSettings.json ╠═════════════════════════════════════════════════════════════════════════════════";
	for (auto & l : jsonLines) {
		ofLogNotice("ofxApp") << " ║ " << l;
	}
	ofLogNotice("ofxApp") << " ╚══════════════════════════════════════════════════════════════════════════════════════════════════════════";
	#endif	
}


void App::saveSettings(){
	ofLogNotice("ofxApp") << "saveSettings() to " << settingsFile;
	settings().save(ofToDataPath(settingsFile, true));
	string settingsString = settings().getAsJsonString();
	logBanner("Saved Settings: \n" + settingsString + "\n");
}


void App::setupApp(){

	RUI_NEW_GROUP("APP");
	showMouse = getBool("App/showMouse");
	RUI_SHARE_PARAM(showMouse);
	enableMouse = getBool("App/enableMouse");
	RUI_SHARE_PARAM(enableMouse);
	RUI_PUSH_TO_CLIENT();
	//RUI_LOAD_FROM_XML();
	setMouseEvents(enableMouse);
	ofBackground(colorsStorage.bgColor);
}


void App::setupLogging(){

	if(getBool("logging/deleteOldLogs")){
		ofxSuperLog::clearOldLogs(LogsDir, getInt("logging/logExpirationInDays"));
	}
	bool logToConsole = getBool("logging/toConsole");
	bool logToScreen = getBool("logging/toScreen");
	ofSetLogLevel(ofLogLevel(getInt("logging/logLevel")));
	//lets keep a ref to the logger counter around so that we can control when it gets deleted
	
	loggerStorage = new ofPtr<ofxSuperLog>(); //note this 2* madness is to avoid the logger being delete b4 the app is finished logging
	*loggerStorage = ofxSuperLog::getLogger(logToConsole, logToScreen, LogsDir);
	ofSetLoggerChannel(*loggerStorage);
	bool visible = getBool("logging/visibleAtStartup");
	(*loggerStorage)->setScreenLoggingEnabled(visible);
	(*loggerStorage)->setMaximized(true);
	(*loggerStorage)->setMaxNumLogLines(getInt("logging/maxScreenLines"));
	(*loggerStorage)->setUseScreenColors(true);
	(*loggerStorage)->setSyncronizedLogging(getBool("logging/syncronizedLogging"));
	

	float panelW = getFloat("logging/screenLogPanelWidth");
	ofxSuperLog::getLogger()->setDisplayWidth(panelW);

	//asset manager uses this separate logger to create an "asset report"  file after every launch
 	//stating status of every downloaded asset (ie missing sha1, sha1 missmatch, etc)
	ofxThreadSafeLog::one()->setPrintToConsole(getBool("logging/ThreadSafeLog/alsoPrintToConsole"));
}


void App::setupRemoteUI(){
	RUI_SET_CONFIGS_DIR(configsDir);
	RUI_GET_INSTANCE()->setUiColumnWidth(getInt("RemoteUI/columnWidth", 280));
	RUI_GET_INSTANCE()->setBuiltInUiScale(getFloat("RemoteUI/uiScale", 1.0));
	bool useFontStash = getBool("RemoteUI/useFontStash");
	if(useFontStash){
		string fontFile = getString("RemoteUI/fontFile");
		ofxApp::assertFileExists(fontFile);
		RUI_GET_INSTANCE()->drawUiWithFontStash(fontFile, getInt("RemoteUI/fontSize", 15));
	}
	bool ruiSaveOnQuit = getBool("RemoteUI/saveSettingsOnExit");
	RUI_GET_INSTANCE()->setSaveToXMLOnExit(ruiSaveOnQuit);

	bool autoBackupsWhenSaving = getBool("RemoteUI/automaticBackupsOnSave");
	RUI_GET_INSTANCE()->setAutomaticBackupsEnabled(autoBackupsWhenSaving);

	bool drawNotifications = getBool("RemoteUI/drawOnScreenNotifications");
	RUI_GET_INSTANCE()->setDrawsNotificationsAutomaticallly(drawNotifications);

	float notifScreenTime = getFloat("RemoteUI/notificationsScreenTime");
	RUI_GET_INSTANCE()->setNotificationScreenTime(notifScreenTime);

	float logNotifScreenTime = getFloat("RemoteUI/logNotificationsScreenTime");
	RUI_GET_INSTANCE()->setLogNotificationScreenTime(logNotifScreenTime);

	ofLogNotice("ofxApp") << "RemoteUI will save settings on quit: " << ruiSaveOnQuit;
	RUI_GET_INSTANCE()->setShowUIDuringEdits(getBool("RemoteUI/showUiDuringEdits"));

	ofAddListener(RUI_GET_OF_EVENT(), this, &App::onRemoteUINotification);
	RUI_SETUP();

	bool enabled = getBool("RemoteUI/enabled");
	if(!enabled){
		ofLogWarning("ofxApp") << "Disabling ofxRemoteUI as specified in json settings!";
		RUI_GET_INSTANCE()->setEnabled(enabled);
	}
}


void App::loadModulesSettings(){

	std::pair<string,string> credentials;
	credentials.first = getString("downloads/credentials/username");
	credentials.second = getString("downloads/credentials/password");

	ofxSimpleHttp::ProxyConfig proxyCfg;
	proxyCfg.useProxy = getBool("downloads/proxy/useProxy");
	proxyCfg.host = getString("downloads/proxy/proxyHost");
	proxyCfg.port = getInt("downloads/proxy/proxyPort");
	proxyCfg.login = getString("downloads/proxy/proxyUser");
	proxyCfg.password = getString("downloads/proxy/proxyPassword");

	assetDownloadPolicy.fileMissing = getBool("content/AssetDownloadPolicy/fileMissing");
	assetDownloadPolicy.fileTooSmall = getBool("content/AssetDownloadPolicy/fileTooSmall");
	assetDownloadPolicy.fileExistsAndNoSha1Provided = getBool("content/AssetDownloadPolicy/fileExistsAndNoSha1Provided");
	assetDownloadPolicy.fileExistsAndProvidedSha1Missmatch = getBool("content/AssetDownloadPolicy/fileExistsAndProvidedSha1Missmatch");
	assetDownloadPolicy.fileExistsAndProvidedSha1Match = getBool("content/AssetDownloadPolicy/fileExistsAndProvidedSha1Match");

	assetUsagePolicy.fileMissing = getBool("content/AssetUsagePolicy/fileMissing");
	assetUsagePolicy.fileTooSmall = getBool("content/AssetUsagePolicy/fileTooSmall");
	assetUsagePolicy.fileExistsAndNoSha1Provided = getBool("content/AssetUsagePolicy/fileExistsAndNoSha1Provided");
	assetUsagePolicy.fileExistsAndProvidedSha1Missmatch = getBool("content/AssetUsagePolicy/fileExistsAndProvidedSha1Missmatch");
	assetUsagePolicy.fileExistsAndProvidedSha1Match = getBool("content/AssetUsagePolicy/fileExistsAndProvidedSha1Match");

	objectUsagePolicy.allObjectAssetsAreOK = getBool("content/ObjectUsagePolicy/allAssetsAreOK");
	objectUsagePolicy.minNumberOfImageAssets = getBool("content/ObjectUsagePolicy/minNumberImgAssets");
	objectUsagePolicy.minNumberOfVideoAssets = getBool("content/ObjectUsagePolicy/minNumberVideoAssets");
	objectUsagePolicy.minNumberOfAudioAssets = getBool("content/ObjectUsagePolicy/minNumberAudioAssets");

	renderSize.x = getInt("App/renderSize/width");
	renderSize.y = getInt("App/renderSize/height");
	timeSampleOfxApp = getBool("App/TimeSampleOfxApp");
}


void App::setupRuiWatches(){

	ofxJSON paramWatches = settings().getJson("RemoteUI/paramWatches");
	if(paramWatches.size()){
		for( auto itr = paramWatches.begin(); itr!=paramWatches.end() ; itr++){
			string paramName = itr.key().asString();
			bool shouldWatch = (*itr).asBool();
			if (shouldWatch){
				ofLogNotice("ofxApp") << "Adding RemoteUI Param Watch for '" << paramName << "'";
				RUI_WATCH_PARAM_WCN(paramName);
			}
		}
	}
}


void App::setupTimeMeasurements(){
	TIME_SAMPLE_SET_CONFIG_DIR(configsDir);
	TIME_SAMPLE_SET_FRAMERATE(getInt("App/frameRate", 60));
	bool enabled = getBool("TimeMeasurements/enabled", true);
	if(timeSampleOfxApp) enabled = true; //if we are benchmarking ofxApp, enable regardless
	TIME_SAMPLE_SET_ENABLED(enabled);
	TIME_SAMPLE_DISABLE_AVERAGE();
	TIME_SAMPLE_SET_DRAW_LOCATION((ofxTMDrawLocation)(getInt("TimeMeasurements/widgetLocation", 3)));
	TIME_SAMPLE_GET_INSTANCE()->setDeadThreadTimeDecay(getFloat("TimeMeasurements/threadTimeDecay"));
	TIME_SAMPLE_GET_INSTANCE()->setUiScale(getFloat("TimeMeasurements/uiScale", 1.0));
	bool useFontStash = getBool("TimeMeasurements/useFontStash");
	if(useFontStash){
		string fontFile = getString("TimeMeasurements/fontFile");
		ofxApp::assertFileExists(fontFile);
		TIME_SAMPLE_GET_INSTANCE()->drawUiWithFontStash(fontFile, getInt("TimeMeasurements/fontSize", 13));
	}
	TIME_SAMPLE_GET_INSTANCE()->setMsPrecision(getInt("TimeMeasurements/msPrecision", 2));
	TIME_SAMPLE_GET_INSTANCE()->setPlotResolution(getFloat("TimeMeasurements/plotResolution", 1.0));
	TIME_SAMPLE_SET_REMOVE_EXPIRED_THREADS(getBool("TimeMeasurements/removeExpiredThreads", true));
	TIME_SAMPLE_GET_INSTANCE()->setRemoveExpiredTimings(getBool("TimeMeasurements/removeExpiredTimings", false));
	TIME_SAMPLE_GET_INSTANCE()->setDrawPercentageAsGraph(getBool("TimeMeasurements/percentageAsGraph", true));
	TIME_SAMPLE_GET_INSTANCE()->setPlotHeight(getFloat("TimeMeasurements/plotH", 60));
}


void App::setupTuio(){
	if(getBool("tuio/enabled")){
		int port = getInt("tuio/port");
		ofLogNotice("ofxApp") << "Listening for TUIO events at port " << port;
		tuioClient.start(port); //TODO - make sure we do it only once!
		ofAddListener(tuioClient.cursorAdded, delegate, &ofxAppDelegate::tuioAdded);
		ofAddListener(tuioClient.cursorRemoved, delegate, &ofxAppDelegate::tuioRemoved);
		ofAddListener(tuioClient.cursorUpdated, delegate, &ofxAppDelegate::tuioUpdated);
	}
}


void App::update(ofEventArgs &){
	tuioClient.getMessage();
	for(auto c : contentStorage){
		c.second->update(dt);
	}
	updateStateMachine(dt);
}


void App::exit(ofEventArgs &){
	ofLogWarning("ofxApp") << "OF is exitting!";
	ofLogWarning("ofxApp") << "Destroying ofxSimpleHttp SSL context...";
	ofxSimpleHttp::destroySslContext();
	ofLogWarning("ofxApp") << "Closing ThreadSafeLog(s)...";
	ofxThreadSafeLog::one()->close();
	ofLogWarning("ofxApp") << "Done exitting!";
}

//////////////////// LOADING SCREEN /////////////////////////////////////////////////////////////////
#pragma Draw Loading Screen

void App::draw(ofEventArgs &){
	if(appState.getState() != RUNNING){
		ofSetupScreen();
		appState.draw(ofGetCurrentViewport());
	}
	ofSetColor(0);
	mullions.draw();
	ofSetColor(255);

	//stack up stats
	int x = 20;
	int y = 27;
	int pad = -10;
	int fontSize = 15;
	if(globalsStorage.drawStaticTexturesMemStats){
		float mb = app.textures().getTotalMemUsed();
		ofRectangle r = drawMsgInBox("Static Assets Mem Used: " + ofToString(mb, 1) + "Mb", x, y, fontSize, ofColor::fuchsia);
		y += r.height + fabs(r.y - y) + pad;
	}

	if(globalsStorage.drawTextureLoaderStats){
		ofRectangle r = drawMsgInBox(TexturedObjectStats::one().getStatsAsText(), x, y, fontSize, ofColor::orange);
		y += r.height + fabs(r.y - y) + pad;
	}

	if(globalsStorage.drawTextureLoaderState){
		ofRectangle r = drawMsgInBox(ProgressiveTextureLoadQueue::instance()->getStatsAsText(), x, y, fontSize, ofColor::limeGreen);
		y += r.height + fabs(r.y - y) + pad;
	}
}


void App::onDrawLoadingScreenStatus(ofRectangle & area){

	switch (appState.getState()) {

		case LOADING_STATIC_TEXTURES:{
			textures().drawAll(area);
			float progress = textures().getNumLoadedTextures() / float(textures().getNumTextures());
			appState.updateState( progress, "");
			string msg = ofToString(textures().getTotalMemUsed(), 1) + "MBytes used";
			drawMsgInBox(msg, 20, 60, loadingScreenFontSize, ofColor::white);
			
		}break;

		case LOAD_CUSTOM_USER_CONTENT:
		case SETUP_USER_APP:
			delegate->drawLoadingScreenForUserProcess(appState.getState(), area);
			break;

		default: break;
	}
}


#pragma mark State Machine

void App::updateStateMachine(float dt){

	switch (appState.getState()) {

		case LOADING_JSON_CONTENT:

			appState.updateState( contentStorage[currentContentID]->getPercentDone(), contentStorage[currentContentID]->getStatus());

			if(appState.isReadyToProceed() ){ //slow down the state machine to handle error / retry

				if( appState.hasError() && appState.ranOutOfErrorRetries()){ //give up!
					ofLogError("ofxApp") << "json failed to load too many times! Giving Up!";
					appState.setState(LOADING_JSON_CONTENT_FAILED);
					break;
				}else{
					
					if(contentStorage[currentContentID]->isContentReady()){ //see if we are done
						logBanner("JSON content \"" + currentContentID + "\" loaded! " + ofToString(contentStorage[currentContentID]->getNumParsedObjects()) + " objects.");
						loadedContent.push_back(currentContentID);
						if(timeSampleOfxApp) TS_STOP_NIF("ofxApp LoadContent " + currentContentID);

						if(loadedContent.size() == contentStorage.size()){ //done loading ALL the JSON contents!
							appState.setState(LOAD_CUSTOM_USER_CONTENT);
						}else{ //load the next json
							currentContentID = requestedContent[loadedContent.size()];
							appState.setState(LOADING_JSON_CONTENT);
						}
						break;
					}
				}

				if(contentStorage[currentContentID]->foundError()){
					int numRetries = getInt("StateMachine/onErrorRetryCount", 5);
					int delaySeconds = getInt("StateMachine/onErrorWaitTimeSec", 5);
					appState.setError("failed to load content for \"" + currentContentID + "\"", delaySeconds /*sec*/, numRetries /*retry max*/); //report an error, retry!
					ofLogError("ofxApp") << "json failed to load! (" << appState.getNumTimesRetried() << ")";
					if(numRetries > 0){ //if no retry allowed, jump to fail state directly
						appState.setState(LOADING_JSON_CONTENT, false); //note "false" << do not clear errors (to keep track of # of retries)
					}else{
						appState.setState(LOADING_JSON_CONTENT_FAILED, false);  //note "false" << do not clear errors (to keep track of # of retries)
					}
				}
			}
			break;

		case LOADING_JSON_CONTENT_FAILED:{
			string knownGoodJSON = "file://" + contentStorage[currentContentID]->getLastKnownGoodJsonPath();
			contentStorage[currentContentID]->setJsonDownloadURL(knownGoodJSON); //lets try from a known good json
			appState.setState(LOADING_JSON_CONTENT, false);
			}break;

		case LOAD_CUSTOM_USER_CONTENT:
			if(delegate->isUserProcessDone(LOAD_CUSTOM_USER_CONTENT)){
				ofLogNotice("ofxApp") << "Done LOAD_CUSTOM_USER_CONTENT!";
				appState.setState(SETUP_USER_APP);
			}break;

		case SETUP_USER_APP:
			if(delegate->isUserProcessDone(SETUP_USER_APP)){
				ofLogNotice("ofxApp") << "Done SETUP_USER_APP!";
				appState.setState(POST_USER_SETUP);
			}break;

		case RUNNING:
			appState.updateState( -1, "");
			break;


		default: break;
	}
}


void App::onStateChanged(ofxStateMachine<ofxApp::State>::StateChangedEventArgs& change){

	ofLogNotice("ofxApp") 	<< "State Changed from \"" << appState.getNameForState(change.oldState)
							<< "\" to \"" << appState.getNameForState(change.newState) << "\"  State Duration: " << change.timeInPrevState << "sec.";

	switch(change.newState){

		case LOADING_STATIC_TEXTURES:
			startLoadingStaticAssets();
			break;

		case LOADING_JSON_CONTENT:{
			if(change.oldState != LOADING_JSON_CONTENT_FAILED){
				if(timeSampleOfxApp) TS_START_NIF("ofxApp LoadContent " + currentContentID);
				logBanner("Start Loading Content  \"" + currentContentID + "\"");
				
				bool keyExists = settings().exists("content/JsonSources/" + currentContentID);
				
				if(keyExists){
					string jsonURL = getString("content/JsonSources/" + currentContentID + "/url");
					string jsonDir = getString("content/JsonSources/" + currentContentID + "/jsonDownloadDir");
					bool skipPolicyTests = getBool("content/JsonSources/" + currentContentID + "/shouldSkipObjectPolicyTests");
					
					int numConcurrentDownloads = getInt("downloads/maxConcurrentDownloads");
					int numThreads = getInt("App/maxThreads");
					int timeOutSecs = getInt("downloads/timeOutSec");
					int speedLimitKBs = getInt("downloads/speedLimitKb");
					float idleTimeAfterDl = getFloat("downloads/idleTimeAfterEachDownloadSec");
					
					contentStorage[currentContentID]->setup(currentContentID,
															jsonURL,
															jsonDir,
															numThreads,
															numConcurrentDownloads,
															speedLimitKBs,
															timeOutSecs,
															skipPolicyTests,
															idleTimeAfterDl,
															credentials,
															proxyCfg,
															contentCfgs[currentContentID],
															objectUsagePolicy
													  );
					
					contentStorage[currentContentID]->fetchContent(); //this starts the ofxAppContent process!
					
				}else{
					ofxApp::terminateApp("ofxApp", "Requested content ID \"content/JsonSources/" + currentContentID + "\" not found in \"" + settingsFile + "\"");
				}

			}else{ //We are retrying to download with a known good json! we already swapped the JSON URL to a local older JSON
				contentStorage[currentContentID]->fetchContent(); //this starts the ofxAppContent process!
			}
			}break;

		case LOADING_JSON_CONTENT_FAILED:
			appState.setProgressBarExtraInfo(" - CONTENT LOAD FAILED");
			//ofxSuperLog::getLogger()->setScreenLoggingEnabled(true); //show log if json error
			break;

		case LOAD_CUSTOM_USER_CONTENT:
			for(auto c : contentStorage){
				delegate->contentIsReady(c.first, c.second->getParsedObjects());
			}
			ofLogNotice("ofxApp") << "Start Loading Custom User Content...";
			delegate->startUserProcess(LOAD_CUSTOM_USER_CONTENT);
			break;

		case SETUP_USER_APP:
			ofLogNotice("ofxApp") << "Start Setup User App...";
			delegate->startUserProcess(SETUP_USER_APP);
			break;

		case POST_USER_SETUP:
			setupRuiWatches();
			setupApp();
			ofLogNotice("ofxApp") << "Start Post User Setup...";
			delegate->startUserProcess(POST_USER_SETUP); //user custom code runs here
			appState.setState(RUNNING);
			break;

		case RUNNING:{
			float ts = -1.0f;
			if(timeSampleOfxApp){
				ts = TS_STOP_NIF("ofxApp Setup");
			}
			logBanner(" ofxApp Setup Complete! " + string((ts > 0.0f) ? ofToString(ts, 1) + "sec." : "") );
			}
			break;

		default: break;
	}
}


void App::onStateError(ofxStateMachine<ofxApp::State>::ErrorStateEventArgs& error){
	ofLogError("ofxApp") << "Error '" << error.errorMsg << "' during state '" << appState.getNameForState(error.state) << "'";
}


void App::onContentManagerStateChanged(string& s){
	appState.setProgressBarExtraInfo(": " + s); // add our sub-state name to the loading screen
}


void App::onStaticTexturesLoaded(){
	ofLogNotice("ofxApp")<< "All Static Textures Loaded!";
	if(timeSampleOfxApp) TS_STOP_NIF("ofxApp Load Static Textures");
	if(contentStorage.size()){
		appState.setState(LOADING_JSON_CONTENT);
	}else{
		ofLogWarning("ofxApp")<< "Skipping JsonLoadContent phase, as there's no content to load.";
		appState.setState(SETUP_USER_APP);
	}
}


void App::onRemoteUINotification(RemoteUIServerCallBackArg &arg){
	switch (arg.action) {
		case CLIENT_UPDATED_PARAM:
			if(arg.paramName == "showMouse"){
				if(arg.param.boolVal) ofShowCursor();
				else ofHideCursor();
			}
			if(arg.paramName == "enableMouse"){
				setMouseEvents(arg.param.boolVal);
			}
			if(arg.paramName == "bgColor"){
				ofBackground(colorsStorage.bgColor);
				RUI_PUSH_TO_CLIENT();
			}
			break;
		default:
			break;
	}
}


void App::onKeyPressed(ofKeyEventArgs & a){
	bool didPress = false;
	switch(a.key){
		case 'W': screenSetup.cycleToNextScreenMode(); didPress = true; break;
		case 'L': ofxSuperLog::getLogger()->setScreenLoggingEnabled(!ofxSuperLog::getLogger()->isScreenLoggingEnabled()); didPress = true; break;
		case 'M': mullions.toggle(); didPress = true; break;
		case 'D': globalsStorage.debug^= true; didPress = true; break;
	}
	if(didPress){
		RUI_PUSH_TO_CLIENT();
	}
}


ofRectangle App::getRenderAreaForCurrentWindowSize(){

	ofRectangle win = ofRectangle(0,0, ofGetWindowWidth(), ofGetWindowHeight());
	ofRectangle render = ofRectangle(0,0,app.renderSize.x, app.renderSize.y);
	render.scaleTo(win);
	return render;
}


ofRectangle App::getRenderRect() {
	return ofRectangle(0, 0, app.renderSize.x, app.renderSize.y);
}


void App::logBanner(const string & log){
	ofLogNotice("ofxApp") << "";
	#ifdef TARGET_WIN32
	ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////";
	ofLogNotice("ofxApp") << "// " << log;
	ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////";
	#else
	ofLogNotice("ofxApp") << "███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████";
	ofLogNotice("ofxApp") << "██ " << log;
	ofLogNotice("ofxApp") << "███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████";
	#endif
	ofLogNotice("ofxApp") << "";
}

ofRectangle App::drawMsgInBox(string msg, int x, int y, int fontSize, ofColor fontColor, ofColor bgColor, float edgeGrow) {

	if (msg.size() == 0) return ofRectangle();
	ofxFontStash & font = fonts().getMonoBoldFont();
	ofRectangle bbox = font.getBBox(msg, fontSize, x, y);
	ofSetColor(bgColor);
	bbox.x -= edgeGrow; bbox.y -= edgeGrow; bbox.width += 2 * edgeGrow; bbox.height += 2 * edgeGrow;
	ofDrawRectangle(bbox);
	ofSetColor(fontColor);
	font.drawMultiLine(msg, fontSize, x, y);
	ofSetColor(255);
	return bbox;
}

///////////////////// SETTINGS //////////////////////////////////////////////////////////////////////
#pragma mark Settings

bool& App::getBool(const string & key, bool defaultVal){
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(settings().exists(key)){
		if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << FILE_ACCES_ICON << " Getting Bool Value for \"" << key << "\" : " << settings().getBool(key);
		return settings().getBool(key);
	}else{
		string msg = "Requesting a BOOL setting that does not exist! \"" + key + "\" in '" + settingsFile + "'";
		ofLogFatalError("ofxApp") << msg;
		if(QUIT_ON_MISSING_SETTING) ofxApp::terminateApp("ofxApp", msg);
		static auto def = defaultVal;
		return def; //mmmm....
	}
}


int& App::getInt(const string & key, int defaultVal){
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(settings().exists(key)){
		if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << FILE_ACCES_ICON << " Getting Int Value for \"" << key << "\" : " << settings().getInt(key);
		return settings().getInt(key);
	}else{
		string msg = "Requesting an INT setting that does not exist! \"" + key + "\" in '" + settingsFile + "'";
		ofLogFatalError("ofxApp") << msg;
		if(QUIT_ON_MISSING_SETTING) ofxApp::terminateApp("ofxApp", msg);
		static auto def = defaultVal;
		return def; //mmmm....
	}
}

float& App::getFloat(const string & key, float defaultVal){
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(settings().exists(key)){
		if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << FILE_ACCES_ICON << " Getting Float Value for \"" << key << "\" : " << settings().getFloat(key);
		return settings().getFloat(key);
	}else{
		string msg = "Requesting a FLOAT setting that does not exist! \"" + key + "\" in '" + settingsFile + "'";
		ofLogFatalError("ofxApp") << msg;
		if(QUIT_ON_MISSING_SETTING) ofxApp::terminateApp("ofxApp", msg);
		static auto def = defaultVal;
		return def; //mmmm....
	}
}

string& App::getString(const string & key, string defaultVal){
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(settings().exists(key)){
		if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << FILE_ACCES_ICON << " Getting String Value for \"" << key << "\" : " << settings().getString(key);
		return settings().getString(key);
	}else{
		string msg = "Requesting a STRING setting that does not exist! \"" + key + "\" in '" + settingsFile + "'";
		ofLogFatalError("ofxApp") << msg;
		if(QUIT_ON_MISSING_SETTING) ofxApp::terminateApp("ofxApp", msg);
		static auto def = defaultVal;
		return def; //mmmm....
	}
}

ofColor& App::getColor(const string & key, ofColor defaultVal){
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(settings().exists(key)){
		if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << FILE_ACCES_ICON << " Getting Color Value for \"" << key << "\" : " << settings().getColor(key);
		return settings().getColor(key);
	}else{
		string msg = "Requesting a COLOR setting that does not exist! \"" + key + "\" in '" + settingsFile + "'";
		ofLogFatalError("ofxApp") << msg;
		if(QUIT_ON_MISSING_SETTING) ofxApp::terminateApp("ofxApp", msg);
		static auto def = defaultVal;
		return def; //mmmm....
	}
}
