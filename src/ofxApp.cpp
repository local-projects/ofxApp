//
//  ofxApp.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 3/8/16.
//
//

#include "ofxApp.h"

//ofxApp app; //app global in your subclass!

ofxApp::ofxApp(){

}

void ofxApp::setup(){

	if(isSetup){
		ofLogError("ofxApp") << "Trying to setup() ofxApp a second time!";
	}else{
		fonts().setup();

		setupLogging();
		setupRemoteUI();
		globals().setupRemoteUIParams();
		setupTimeMeasurements();
		colors().setupRemoteUIParams();
		textures().setup();
		setupTuio();
		ofSetFrameRate(getInt("App/frameRate"));
		ofBackground(colors().bgColor);

		ofAddListener(ofEvents().update, this, &ofxApp::update);
	}
}

void ofxApp::loadStaticAssets(){

	string texturesPath = getString("Assets/textures");
	assertFileExists(texturesPath);
	textures().loadTexturesInDir(texturesPath);
}

void ofxApp::postSetup(){

	setupRuiWatches();
	setupApp();
}


void ofxApp::loadSettings(){

	assertFileExists(settingsFile);

	ofLogNotice("ofxApp") << "loadSettings() from " << settingsFile;
	bool ok = settings().load(ofToDataPath(settingsFile, true));
	if(!ok){
		ofLogError("ofxApp") << "Could not load settings from " << ofToDataPath(settingsFile, true);
		ofLogError("ofxApp") << "Exiting!";
		exit(-1);
	}
	string settingsString = settings().getAsJsonString();
	ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////";
	ofLogNotice("ofxApp") << "Loaded Settings: " << endl << settingsString << endl;
	ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////";
	hasLoadedSettings = true;
}


void ofxApp::saveSettings(){
	ofLogNotice("ofxApp") << "saveSettings() to " << settingsFile;
	settings().save(ofToDataPath(settingsFile, true));
	string settingsString = settings().getAsJsonString();
	ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////";
	ofLogNotice("ofxApp") << "Saved Settings: " << endl << settingsString << endl;
	ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////";
}


void ofxApp::update(ofEventArgs & args){

}


void ofxApp::setupApp(){

	RUI_NEW_GROUP("APP");
	bool & showMouse = getBool("App/showMouse");
	RUI_SHARE_PARAM(showMouse);
	bool & enableMouse = getBool("App/enableMouse");;
	RUI_SHARE_PARAM(enableMouse);

	ofxScreenSetup::ScreenMode mode = ofxScreenSetup::ScreenMode((int)getInt("App/window/windowMode"));
	screenSetup.setScreenMode(mode);

}

void ofxApp::setupLogging(){

	#if defined(__has_feature) /*this triggers asan for some reason - dont clean longs when asan is ON*/
		#if !__has_feature(address_sanitizer)
			if(getBool("logging/deleteOldLogs")){
				ofxSuperLog::clearOldLogs(LogsDir, getInt("logging/logExpirationInDays"));
			}
		#endif
	#endif
	bool logToConsole = true; //getSBool("logging/deleteOldLogs");
	bool logToScreen = getBool("logging/toScreen");
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetLoggerChannel(ofxSuperLog::getLogger(logToConsole, logToScreen, LogsDir));
	ofxSuperLog::getLogger()->setScreenLoggingEnabled(false);
	ofxSuperLog::getLogger()->setMaximized(true);
	ofxSuperLog::getLogger()->setMaxNumLogLines(getInt("logging/maxScreenLines"));
	ofxSuperLog::getLogger()->setUseScreenColors(true);
	//set a nice font for the on screen logger
	ofxSuperLog::getLogger()->setFont(&(fonts().veraMono), getFloat("logging/uiScale", 1.0) * getFloat("logging/fontSize"));
	ofxSuperLog::getLogger()->setDisplayWidth(getFloat("logging/logPanelWidth"));
}


void ofxApp::setupRemoteUI(){
	RUI_SET_CONFIGS_DIR(configsDir);
	RUI_GET_INSTANCE()->setUiColumnWidth(getInt("RemoteUI/columnWidth", 280));
	RUI_GET_INSTANCE()->setBuiltInUiScale(getFloat("RemoteUI/uiScale", 1.0));
	bool useFontStash = getBool("RemoteUI/useFontStash");
	if(useFontStash){
		string fontFile = getString("RemoteUI/fontFile");
		assertFileExists(fontFile);
		RUI_GET_INSTANCE()->drawUiWithFontStash(fontFile, getInt("RemoteUI/fontSize", 15));
	}
	bool ruiSaveOnQuit = getBool("RemoteUI/saveSettingsOnQuit");
	RUI_GET_INSTANCE()->setSaveToXMLOnExit(ruiSaveOnQuit);
	ofLogNotice("ofxApp") << "RemoteUI will save settings on quit: " << ruiSaveOnQuit;
	RUI_GET_INSTANCE()->setShowUIDuringEdits(getBool("RemoteUI/showUiDuringEdits"));

	ofAddListener(RUI_GET_OF_EVENT(), this, &ofxApp::remoteUIClientDidSomething);
	RUI_SETUP();
}


void ofxApp::setupRuiWatches(){

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


void ofxApp::setupTimeMeasurements(){
	TIME_SAMPLE_SET_CONFIG_DIR(configsDir);
	TIME_SAMPLE_SET_FRAMERATE(getInt("App/frameRate", 60));
	TIME_SAMPLE_SET_ENABLED(getBool("TimeMeasurements/enabled", true));
	TIME_SAMPLE_DISABLE_AVERAGE();
	TIME_SAMPLE_SET_DRAW_LOCATION((ofxTMDrawLocation)(getInt("TimeMeasurements/widgetLocation", 3)));
	TIME_SAMPLE_GET_INSTANCE()->setDeadThreadTimeDecay(getFloat("TimeMeasurements/threadTimeDecay"));
	TIME_SAMPLE_GET_INSTANCE()->setUiScale(getFloat("TimeMeasurements/uiScale", 1.0));
	bool useFontStash = getBool("TimeMeasurements/useFontStash");
	if(useFontStash){
		string fontFile = getString("TimeMeasurements/fontFile");
		assertFileExists(fontFile);
		TIME_SAMPLE_GET_INSTANCE()->drawUiWithFontStash(fontFile, getInt("TimeMeasurements/fontSize", 13));
	}
	TIME_SAMPLE_GET_INSTANCE()->setMsPrecision(getInt("TimeMeasurements/msPrecision", 2));
	TIME_SAMPLE_GET_INSTANCE()->setPlotResolution(getFloat("TimeMeasurements/plotResolution", 1.0));
	TIME_SAMPLE_SET_REMOVE_EXPIRED_THREADS(getBool("TimeMeasurements/removeExpiredThreads", true));
	TIME_SAMPLE_GET_INSTANCE()->setRemoveExpiredTimings(getBool("TimeMeasurements/removeExpiredTimings", false));
	TIME_SAMPLE_GET_INSTANCE()->setDrawPercentageAsGraph(getBool("TimeMeasurements/percentageAsGraph", true));
	TIME_SAMPLE_GET_INSTANCE()->setPlotHeight(getFloat("TimeMeasurements/plotH", 60));
}


void ofxApp::setupTuio(int port){
	if(getBool("tuio/enabled")){
		int port = getInt("tuio/port");
		ofLogNotice("ofxApp") << "Listening for TUIO events at port " << port;
		tuioClient.start(port); //TODO - make sure we do it only once!
		ofAddListener(tuioClient.cursorAdded, this, &ofxApp::tuioAdded);
		ofAddListener(tuioClient.cursorRemoved, this, &ofxApp::tuioRemoved);
		ofAddListener(tuioClient.cursorUpdated, this, &ofxApp::tuioUpdated);
	}
}


ofxTuioCursor ofxApp::getTuioAtMouse(int x, int y){
	float r = 1;
	return ofxTuioCursor( 0,0, x / (float)ofGetWidth(),  r * y / (float)ofGetHeight());
}


bool& ofxApp::getBool(const string & key, bool defaultVal){
	if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << "getting Bool Value for " << key;
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(ofxApp::one().settings().exists(key)){
		return ofxApp::one().settings().getBool(key);
	}else{
		ofLogError("ofxApp") << "Requesting setting that does not exist! " << key;
		if(QUIT_ON_MISSING_SETTING) exit(-1);
		return defaultVal;
	}
}

int& ofxApp::getInt(const string & key, int defaultVal){
	if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << "getting Int Value for " << key;
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(ofxApp::one().settings().exists(key)){
		return ofxApp::one().settings().getInt(key);
	}else{
		ofLogError("ofxApp") << "Requesting setting that does not exist! " << key;
		if(QUIT_ON_MISSING_SETTING) exit(-1);
		return defaultVal;
	}
}

float& ofxApp::getFloat(const string & key, float defaultVal){
	if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << "getting Float Value for " << key;
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(ofxApp::one().settings().exists(key)){
		return ofxApp::one().settings().getFloat(key);
	}else{
		ofLogError("ofxApp") << "Requesting setting that does not exist! " << key;
		if(QUIT_ON_MISSING_SETTING) exit(-1);
		return defaultVal;
	}
}

string& ofxApp::getString(const string & key, string defaultVal){
	if(VERBOSE_SETTINGS_ACCESS) ofLogNotice("ofxApp") << "getting String Value for " << key;
	if(!hasLoadedSettings) ofLogError("ofxApp") << "Trying to get a setting but Settings have not been loaded!";
	if(ofxApp::one().settings().exists(key)){
		return ofxApp::one().settings().getString(key);
	}else{
		ofLogError("ofxApp") << "Requesting setting that does not exist! " << key;
		if(QUIT_ON_MISSING_SETTING) exit(-1);
		return defaultVal;
	}
}

void ofxApp::remoteUIClientDidSomething(RemoteUIServerCallBackArg &arg){
	switch (arg.action) {
		case CLIENT_CONNECTED: cout << "CLIENT_CONNECTED" << endl; break;
		case CLIENT_DISCONNECTED: cout << "CLIENT_DISCONNECTED" << endl; break;
		case CLIENT_UPDATED_PARAM:
			if(arg.paramName == "showMouse"){
				if(arg.param.boolVal) ofShowCursor();
				else ofHideCursor();
			}
			break;
		case CLIENT_DID_SET_PRESET: cout << "CLIENT_DID_SET_PRESET" << endl; break;
		case CLIENT_SAVED_PRESET: cout << "CLIENT_SAVED_PRESET" << endl; break;
		case CLIENT_DELETED_PRESET: cout << "CLIENT_DELETED_PRESET" << endl; break;
		case CLIENT_SAVED_STATE: cout << "CLIENT_SAVED_STATE" << endl; break;
		case CLIENT_DID_RESET_TO_XML: cout << "CLIENT_DID_RESET_TO_XML" << endl; break;
		case CLIENT_DID_RESET_TO_DEFAULTS: cout << "CLIENT_DID_RESET_TO_DEFAULTS" << endl; break;
		default:
			break;
	}
}
