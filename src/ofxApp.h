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
#include "AppContentBasic.h"
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

//some macro magic to include the user defined subclasses of AppColorsBasic, AppFontsBasic, AppGlobalsBasic
//it takes the user defined macro (ie OFX_APP_NAME=myApp) and creates an #include "myAppColors.h"
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_COLORS_FILENAME) 	//include myAppColors.h
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_FONTS_FILENAME)	//include myAppFonts.h
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_GLOBALS_FILENAME) //include myAppGlobals.h
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_STATES_FILENAME)	//include myAppStates.h

namespace ofxApp{

class App : public HasAssets{

public:

	const string settingsFile = "configs/AppSettings.json";
	const string LogsDir = "logs";
	const string configsDir = "configs";

	App();

	void setup(ofxApp::ContentConfig cfg, ofxAppDelegate * delegate);

	void update(ofEventArgs &);
	void draw(ofEventArgs &);

	// Crazy Macro magic here!! Beware!!
	// this compounds some classnames to match whatever you decided to name your app;
	// so "OFX_APP_CLASS_NAME(Colors)" becomes "MyAppColors"
	// "MyApp" is a macro you MUST define in your pre-processor macros:
	//  OFX_APP_NAME=MyApp
	OFX_APP_CLASS_NAME(Colors) & colors(){return colorsStorage;}
	OFX_APP_CLASS_NAME(Globals) & globals(){return globalsStorage;}
	OFX_APP_CLASS_NAME(Fonts) & fonts(){return fontStorage;}

	AppContentBasic & content(){return contentStorage;}
	ofxJsonSettings& settings(){return ofxJsonSettings::get();}
	AppStaticTextures & textures(){return texStorage;}
	ofPtr<ofxSuperLog> logger(){return ofxSuperLog::getLogger();}
	ofxTuioClient & tuio(){ return tuioClient;}

	ofxScreenSetup screenSetup;

	// Convinience Getters for App Settings ////////////////////////////////////////////////////////
	bool& getBool(const string & key, bool defaultVal = true);
	int& getInt(const string & key, int defaultVal = 0);
	float& getFloat(const string & key, float defaultVal = 0.0);
	string& getString(const string & key, string defaultVal = "uninited!");
	void loadSettings();
	void saveSettings();


	// TUIO ////////////////////////////////////////////////////////////////////////////////////////
	virtual void tuioAdded(ofxTuioCursor & tuioCursor){};
	virtual void tuioRemoved(ofxTuioCursor & tuioCursor){};
	virtual void tuioUpdated(ofxTuioCursor & tuioCursor){};
	ofxTuioCursor getTuioAtMouse(int x, int y);

	// STATE MACHINE ///////////////////////////////////////////////////////////////////////////////
	ofxDrawableStateMachine<ofxApp::State> appState; //App State Machine
	virtual void updateStateMachine(float dt);
	virtual void onDrawLoadingScreenStatus(ofRectangle & area); //override to customize loading screen

	virtual void onStateChanged(ofxStateMachine<ofxApp::State>::StateChangedEventArgs& change);
	virtual void onStateError(ofxStateMachine<ofxApp::State>::ErrorStateEventArgs& error);
	virtual void onContentManagerStateChanged(string&);


	// CALLBACKS ///////////////////////////////////////////////////////////////////////////////////
	void onRemoteUINotification(RemoteUIServerCallBackArg & arg);
	void onStaticTexturesLoaded();
	void onKeyPressed(ofKeyEventArgs&);


protected:

	void setupListeners();
	void setupStateMachine();
	void setupTimeMeasurements();
	void setupWindow();
	void setupOF();
	void setupRemoteUI();
	void setupLogging();
	void setupTuio(int port = 3333);
	void setupApp();
	void setupRuiWatches();

	void startLoadingStaticAssets();

	ofxTuioClient						tuioClient;
	AppStaticTextures					texStorage;
	ofxMullion							mullions;

	//crazy macro magic - beware! read a few lines above to see what's going on
	OFX_APP_CLASS_NAME(Colors)			colorsStorage;
	OFX_APP_CLASS_NAME(Globals)			globalsStorage;
	OFX_APP_CLASS_NAME(Fonts)			fontStorage;
	AppContentBasic						contentStorage;

	bool								hasLoadedSettings = false;
	//bool								shouldQuit;

	const int 							loadingScreenFontSize = 22;
	float								dt;
	ofxApp::ContentConfig 				contentCfg;

	ofxAppDelegate *					delegate = nullptr;
};

}

extern ofxApp::App app; //all global parameters are here - add yor "ofxApp" subclass!