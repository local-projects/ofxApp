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

////////////////////////////////////////////////////////////////////////////////////////////////////

//this is all to achieve variable includes given a user specified macro name for the app
//http://stackoverflow.com/questions/32066204/construct-path-for-include-directive-with-macro
//http://stackoverflow.com/questions/1489932/how-to-concatenate-twice-with-the-c-preprocessor-and-expand-a-macro-as-in-arg
//this is to directly include your AppColorsBasic.h, AppHobalsBasic.h, AppFontsBasic.h subclasses.

#define OFX_APP_IDENT(x) x
#define OFX_APP_XSTR(x) #x
#define OFX_APP_STR(x) OFX_APP_XSTR(x)
#define OFX_APP_INCLUDE(x,y) OFX_APP_STR(OFX_APP_IDENT(x)OFX_APP_IDENT(y))

//lots of indirection for this to work...
#define OFX_APP_PASTER(x,y) x ## y
#define OFX_APP_EVALUATOR(x,y)  OFX_APP_PASTER(x,y)
#define OFX_APP_CLASS_NAME(class) OFX_APP_EVALUATOR(OFX_APP_NAME,class)

#define OFX_COLORS_FILENAME Colors.h
#define OFX_FONTS_FILENAME Fonts.h
#define OFX_GLOBALS_FILENAME Globals.h

////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ofMain.h"
#include "ofxJsonSettings.h"

//some macro magic to include the user defined subclasses of AppColorsBasic, AppFontsBasic, AppGlobalsBasic
//it takes the user defined macro (ie OFX_APP_NAME=myApp) and creates an #include "myAppColors.h"
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_COLORS_FILENAME)
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_FONTS_FILENAME)
#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_GLOBALS_FILENAME)

#include "ofxTuio.h"
#include "ofxSuperLog.h"
#include "ofxScreenSetup.h"
#include "ofxRemoteUIServer.h"
#include "ofxTimeMeasurements.h"
#include "AppBaseClasses.h"
#include "AppStaticTextures.h"

class ofxApp : public HasAssets{

public:

	const string settingsFile = "configs/AppSettings.json";
	const string LogsDir = "logs";
	const string configsDir = "configs";

	static ofxApp& one(){
		static ofxApp instance; // Instantiated on first use.
		return instance;
	}

	ofxApp();

	void setup();
	void loadStaticAssets();
	void postSetup();

	void loadSettings();
	void saveSettings();


	ofxJsonSettings& settings(){return ofxJsonSettings::get();}


	// Crazy Macro magic here!! Beware!!
	// this compounds some classnames to match whatever you decided to name your app;
	// so "OFX_APP_CLASS_NAME(Colors)" becomes "MyAppColors"
	// "MyApp" is a macro you MUST define in your pre-processor macros:
	//  OFX_APP_NAME=MyApp
	OFX_APP_CLASS_NAME(Colors) & colors(){return colorsStorage;}
	OFX_APP_CLASS_NAME(Globals) & globals(){return globalsStorage;}
	OFX_APP_CLASS_NAME(Fonts) & fonts(){return fontStorage;}

	AppStaticTextures & textures(){return texStorage;}

	ofPtr<ofxSuperLog> logger(){return ofxSuperLog::getLogger();}
	ofxTuioClient & tuio(){ return tuioClient;}

	ofxScreenSetup screenSetup;

	// convinience getters for settings //
	inline bool& getBool(const string & key, bool defaultVal = true);
	inline int& getInt(const string & key, int defaultVal = 0);
	inline float& getFloat(const string & key, float defaultVal = 0.0);
	inline string& getString(const string & key, string defaultVal = "uninited!");

	// tuio //
	virtual void tuioAdded(ofxTuioCursor & tuioCursor){};
	virtual void tuioRemoved(ofxTuioCursor & tuioCursor){};
	virtual void tuioUpdated(ofxTuioCursor & tuioCursor){};
	ofxTuioCursor getTuioAtMouse(int x, int y);

	void update(ofEventArgs & args);

	//rui callback
	void remoteUIClientDidSomething(RemoteUIServerCallBackArg & arg);

protected:

	void setupTimeMeasurements();
	void setupRemoteUI();
	void setupLogging();
	void setupTuio(int port = 3333);
	void setupApp();
	void setupRuiWatches();

	ofxTuioClient						tuioClient;
	AppStaticTextures					texStorage;

	//crazy macro magic - beware! read a few lines above to see what's going on
	OFX_APP_CLASS_NAME(Colors)			colorsStorage;
	OFX_APP_CLASS_NAME(Globals)			globalsStorage;
	OFX_APP_CLASS_NAME(Fonts)			fontStorage;

	bool 								isSetup = false;
	bool								hasLoadedSettings = false;
	//bool								shouldQuit;
};


//extern ofxApp app; //all global parameters are here - add yor "ofxApp" subclass!