//
//  ofxApp.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 3/8/16.
//
//

#pragma once

#define VERBOSE_SETTINGS_ACCESS 	true
#define QUIT_ON_MISSING_SETTING		true


//Check if the user created the required macro to include his custom sub-classes for Colors, Globals and Fonts.
#ifndef OFX_APP_NAME
	#error You Must define an app Name for your app in the preprocessor macros; ie OFX_APP_NAME=MyApp
	//#define OFX_APP_NAME MyApp /*you define your App's name in your PREPROCESSOR MACROS*/
#endif

#include "ofMain.h"
#include "AppBaseClasses.h"
#include "AppContent.h"
#include "AppFonts.h"
#include "AppStaticTextures.h"
#include "ofxAppMacros.h"

#include "ofxJsonSettings.h"
#include "ofxMullion.h"
#include "ofxTuio.h"
#include "ofxSuperLog.h"
#include "ofxScreenSetup.h"
#include "ofxRemoteUIServer.h"
#include "ofxTimeMeasurements.h"
#include "ofxDrawableStateMachine.h"

//some macro magic to include the user defined subclasses of AppColorsBasic, AppFonts, AppGlobalsBasic
//it takes the user defined macro (ie OFX_APP_NAME=MyApp) and creates an #include "MyAppColors.h"
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_COLORS_FILENAME) 	//include MyAppColors.h
//#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_FONTS_FILENAME)	//include MyAppFonts.h
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_GLOBALS_FILENAME) //include MyAppGlobals.h
//#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_STATES_FILENAME)	//include MyAppStates.h

namespace ofxApp{

class App : public HasAssets, public CanTerminate{

public:

	const string settingsFile = "configs/AppSettings.json";
	const string LogsDir = "logs";
	const string configsDir = "configs";
	const string cleanLogLine = "/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////";

	void setup(const map<string,ofxApp::UserLambdas> & cfgs, ofxAppDelegate * delegate);

	void update(ofEventArgs &);
	void exit(ofEventArgs &);
	void draw(ofEventArgs &);

	// Crazy Macro magic here!! Beware!!
	// this compounds some classnames to match whatever you decided to name your app;
	// so "OFX_APP_CLASS_NAME(Colors)" becomes "MyAppColors"
	// "MyApp" is a macro you MUST define in your pre-processor macros:
	//  OFX_APP_NAME=MyApp
	OFX_APP_CLASS_NAME(Colors) & 	colors(){return colorsStorage;}
	OFX_APP_CLASS_NAME(Globals) & 	globals(){return globalsStorage;}
	AppFonts &						fonts(){return *fontStorage;}
	//AppContent & 					content(){return *contentStorage;}
	ofxJsonSettings & 				settings(){return ofxJsonSettings::get();}
	AppStaticTextures & 			textures(){return texStorage;}
	ofPtr<ofxSuperLog> 				logger(){return ofxSuperLog::getLogger();}
	ofxTuioClient & 				tuio(){ return tuioClient;}

	ofxScreenSetup					screenSetup;

	// Convinience methods ////////////////////////////////////////////////////////
	// SETTINGS //

	bool&		getBool(const string & key, bool defaultVal = true);
	int&		getInt(const string & key, int defaultVal = 0);
	float&		getFloat(const string & key, float defaultVal = 0.0);
	string&		getString(const string & key, string defaultVal = "uninited!");
	ofColor&	getColor(const string & key, ofColor defaultVal = ofColor::red);

	void		loadSettings();
	void		saveSettings();

	ofxApp::State getState(){return appState.getState();}

	//those are cfgs coming from the main config file
	ofxAssets::DownloadPolicy	getAssetDownloadPolicy(){ return assetDownloadPolicy; }
	ofxAssets::UsagePolicy		getAssetUsagePolicy(){ return assetUsagePolicy;}

	// CALLBACKS ///////////////////////////////////////////////////////////////////////////////////

	void onRemoteUINotification(RemoteUIServerCallBackArg & arg);
	void onStaticTexturesLoaded();
	void onKeyPressed(ofKeyEventArgs&);

	// app params that come from settings json
	ofVec2f			renderSize;
	ofRectangle		getRenderAreaForCurrentWindowSize();

protected:

	void setupListeners();
	void setupStateMachine();
	void setupTimeMeasurements();
	void setupWindow();
	void setupOF();
	void setupRemoteUI();
	void setupLogging();
	void setupTuio();
	void setupApp();
	void setupTextureLoader();
	void setupRuiWatches();
	void startLoadingStaticAssets();
	void setMouseEvents(bool enabled);
	void loadSettingsBundles();
	void logBanner(const string & log);

	// STATE MACHINE ///////////////////////////////////////////////////////////////////////////////
	virtual void updateStateMachine(float dt);
	virtual void onDrawLoadingScreenStatus(ofRectangle & area); //override to customize loading screen

	virtual void onStateChanged(ofxStateMachine<ofxApp::State>::StateChangedEventArgs& change);
	virtual void onStateError(ofxStateMachine<ofxApp::State>::ErrorStateEventArgs& error);
	virtual void onContentManagerStateChanged(string&);

	ofxTuioClient						tuioClient;
	AppStaticTextures					texStorage;
	ofxMullion							mullions;

	// Settings bundles ///////////////////////////////////////

	ofxSimpleHttp::ProxyConfig			proxyCfg;
	std::pair<string,string>			credentials; //http

	ofxAssets::DownloadPolicy			assetDownloadPolicy;
	ofxAssets::UsagePolicy				assetUsagePolicy;
	ofxAssets::ObjectUsagePolicy		objectUsagePolicy;

	//crazy macro magic - beware! read a few lines above to see what's going on
	OFX_APP_CLASS_NAME(Colors)				colorsStorage;
	OFX_APP_CLASS_NAME(Globals)				globalsStorage;
	AppFonts *								fontStorage;
	map<string, AppContent*>				contentStorage; //this will be same # as contentCfgs.size()
	ofxDrawableStateMachine<ofxApp::State>	appState; //App State Machine

	bool									hasLoadedSettings = false;
	float									dt;
	bool									timeSampleOfxApp = false;

	map<string, ofxApp::UserLambdas>		contentCfgs; //this will be as big as the number of jsons to load
	string									currentContentID;
	vector<string>							requestedContent;
	vector<string>							loadedContent;


	ofxAppDelegate *						delegate = nullptr;

	const int								loadingScreenFontSize = 22;

};

} //namespace ofxApp

extern ofxApp::App app; //all global parameters are here - add yor "ofxApp" subclass!
