//
//  ofxApp.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 3/8/16.
//
//

#pragma once

#define VERBOSE_SETTINGS_ACCESS 	false
#define QUIT_ON_MISSING_SETTING		true


#include "ofMain.h"
#include "ofxAppDelegate.h"
#include "ofxAppContent.h"
#include "ofxAppFonts.h"
#include "ofxAppStaticTextures.h"
#include "ofxAppMacros.h"
#include "ofxJsonSettings.h"
#include "ofxTuio.h"
#include "ofxSuperLog.h"
#include "ofxScreenSetup.h"
#include "ofxRemoteUIServer.h"
#include "ofxTimeMeasurements.h"
#include "ofxDrawableStateMachine.h"
#include "ofxAppErrorReporter.h"

//Check if the user created the required macro to include his custom sub-classes for Colors, Globals and Fonts.
#ifndef OFX_APP_NAME
	//#error you must define a PREPROCESSOR MACRO with the name of your app, as in OFX_APP_NAME=MyApp
	#define OFX_APP_NONAME
	//#warning You should define an app Name for your app in the preprocessor macros; ie OFX_APP_NAME=MyApp
	//#define OFX_APP_NAME MyApp /*you define your App's name in your PREPROCESSOR MACROS*/
	#include "ofxAppColorsBasic.h"
	#include "ofxAppGlobalsBasic.h"
#else
	//some macro magic to include the user defined subclasses of ofxAppColorsBasic, ofxAppFonts, ofxAppGlobalsBasic
	//it takes the user defined macro (ie OFX_APP_NAME=MyApp) and creates an #include "MyAppColors.h"
	#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_COLORS_FILENAME) 	//include MyAppColors.h
	#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_GLOBALS_FILENAME) //include MyAppGlobals.h
#endif

class ofxGoogleAnalytics;
class ofxMullion;

namespace ofxApp{

class App{

	#ifdef TARGET_WIN32
	const std::string FILE_ACCES_ICON = "[!]";
	#else
	const std::string FILE_ACCES_ICON = "💾";
	#endif

public:

	const std::string settingsFile = "configs/ofxAppSettings.json";
	const std::string LogsDir = "logs";
	const std::string configsDir = "configs";
	const std::string pidFileName = "ofxApp.pid";

	static App& one(){ //this holds the app instance
		static App instance;
		return instance;
	}

	~App();

	App(App const&) = delete; //cant copy construct, or assign
	void operator=(App const&) = delete;

	//static App * get(){return theApp;}
	
	void setup(const map<std::string,ofxApp::ParseFunctions> & cfgs, ofxAppDelegate * delegate);
	void setup(ofxAppDelegate * delegate); //if your app has no content ; no lambdas needed

	void update(ofEventArgs &);
	void exit(ofEventArgs &);
	void draw(ofEventArgs &);

	// Crazy Macro magic here!! Beware!!
	// this compounds some classnames to match whatever you decided to name your app;
	// so "OFX_APP_CLASS_NAME(Colors)" becomes "MyAppColors"
	// "MyApp" is a macro you MUST define in your pre-processor macros:
	// OFX_APP_NAME=MyApp
	// alternativelly, only the default Globals & colors will be defined (ofxAppColorsBasic & ofxAppGlobalsBasic)
	//so you would have to cast the globals to get access to your projec's globals
	#ifdef OFX_APP_NONAME
	ofxAppColorsBasic & 			colors(){return colorsStorage;}
	ofxAppGlobalsBasic & 			globals(){return *globalsStorage;}
	#else
	OFX_APP_CLASS_NAME(Colors) & 	colors(){return colorsStorage;}
	OFX_APP_CLASS_NAME(Globals) & 	globals(){return *globalsStorage;}
	#endif
	
	ofxAppFonts &					fonts(){return *fontStorage;}
	ofxJsonSettings & 				settings(){return ofxJsonSettings::get();}
	ofxAppStaticTextures & 			textures(){return texStorage;}
	ofPtr<ofxSuperLog> 				logger(){return ofxSuperLog::getLogger();}
	ofxAppErrorReporter &			errorReporter(){ return errorReporterObj;}
	ofxTuioClient & 				tuio(){ return tuioClient;}
	ofxGoogleAnalytics *			analytics(){ return gAnalytics; }
	ofxScreenSetup					screenSetup;

	// SETTINGS /////////////////////////////////////////////////////////////////////////////
	// Convenience methods to easily get values from "data/configs/ofxAppSettings.json"

	bool&		getBool(const std::string & key, bool defaultVal = true);
	int&		getInt(const std::string & key, int defaultVal = 0);
	float&		getFloat(const std::string & key, float defaultVal = 0.0);
	std::string&		getString(const std::string & key, const std::string & defaultVal = "uninited!");
	ofColor&	getColor(const std::string & key, ofColor defaultVal = ofColor::red);
	bool		settingExists(const std::string & key);

	void		loadSettings(); //load JSON settings (data/configs/ofxAppSettings.json)
	void		loadDynamicSettings(); //load and update values that can be changed while the app runs (call this with 'R' key);
	void		saveSettings();//not really used / tested! TODO!
	void		setupLogLevelModuleOverrides(bool dynamicLoad);

	//State Machine
	ofxApp::Phase getPhase();//this is the end user state types, you most likely want this
	ofxApp::State getState(){return appState.getState();} //most likely you dont want this
	bool isAppReadyToRun(){return appState.getState() == ofxApp::State::RUNNING;}

	// ERROR STATES / MSGs ///////////////////////////////////////////////////////////////////

	bool enterErrorState(std::string errorHeader, std::string errorBody); //app shows error screen
	bool exitErrorState(); //goes back to RUNNING
	bool isInDevInducedErrorState(){return appState.getState() == State::DEVELOPER_REQUESTED_ERROR_SCREEN;}

	// RETRIEVE POLICIES /////////////////////////////////////////////////////////////////////
	// Those are cfgs coming from the main config file
	const ofxAssets::DownloadPolicy &	getAssetDownloadPolicy(){ return assetDownloadPolicy; }
	const ofxAssets::UsagePolicy &		getAssetUsagePolicy(){ return assetUsagePolicy;}

	// CALLBACKS /////////////////////////////////////////////////////////////////////////////

	void onRemoteUINotification(RemoteUIServerCallBackArg & arg);
	void onStaticTexturesLoaded();
	void onKeyPressed(ofKeyEventArgs&);
	void screenSetupChanged(ofxScreenSetup::ScreenSetupArg &arg);

	// retrieve app params that come from settings json

	ofRectangle		getRenderAreaForCurrentWindowSize();
	ofRectangle		getRenderRect();
	ofRectangle 	getStartupScreenViewport(){return startupScreenViewport;} //loading screen rect area
	ofVec2f			getRenderSize(){return renderSize;}
	bool			isWindowSetup(){return windowIsSetup;}
	bool 			isJsonContentDifferentFromLastLaunch(std::string contentID, std::string & freshJsonSha1, std::string & oldJsonSha1);

	//to draw debug msgs
	ofRectangle		drawMsgInBox(std::string msg, int x, int y, int fontSize, ofColor fontColor, ofColor bgColor = ofColor::black, float edgeGrow = 5);

	void tuioAdded(ofxTuioCursor & tuioCursor);
	void tuioRemoved(ofxTuioCursor & tuioCursor);
	void tuioUpdated(ofxTuioCursor & tuioCursor);
	
	//debug log functionality
	void addToScreenLog(const std::string & str);
	void clearScreenLog();
	
	void addToCurrentFrameLog(const std::string & string);

protected:

	void setupListeners();
	void setupStateMachine();
	void setupTimeMeasurements();
	void setupContentData();
	void setupWindow();
	void setupOF();
	void printOpenGlInfo();
	void setupRemoteUI();
	void setupLogging();
	void setupErrorReporting();
	void setupTuio();
	void setupApp();
	void setupTextureLoader();
	void setupGlobalParameters();
	void setupRuiWatches();
	void startLoadingStaticAssets();
	void setMouseEvents(bool enabled);
	void loadModulesSettings();
	void setupGoogleAnalytics();

	//utils
	void printSettingsFile(); //print JSON settings file to stdout (and logs)
	void drawStats();
	void drawAnimatable();
	void drawMaintenanceScreen();
	void drawErrorScreen();

	void updateAnimatable(float dt);

	// STATE MACHINE ///////////////////////////////////////////////////////////////////////////////

	virtual void updateStateMachine(float dt);
	virtual void onDrawLoadingScreenStatus(ofRectangle & area); //override to customize loading screen

	virtual void onStateChanged(ofxStateMachine<ofxApp::State>::StateChangedEventArgs& change);
	virtual void onStateError(ofxStateMachine<ofxApp::State>::ErrorStateEventArgs& error);
	virtual void onContentManagerStateChanged(std::string&);

	////////////////////////////////////////////////////////////////////////////////////////////////

	ofxTuioClient							tuioClient;
	ofxAppStaticTextures					texStorage;
	ofxMullion *							mullions;

	// Settings Values Bundles ///////////////////////////////////////

	//used by ofxSimpleHttp
	ofxSimpleHttp::ProxyConfig				proxyCfg;
	std::pair<std::string,std::string>				credentials;

	//used by ofxAssets
	ofxAssets::DownloadPolicy				assetDownloadPolicy;
	ofxAssets::UsagePolicy					assetUsagePolicy;
	ofxAssets::ObjectUsagePolicy			objectUsagePolicy;


	// ofxApp various user contents ///////////////////////////////////

	#ifdef OFX_APP_NONAME //if the dev didnt define an app name, use default global vars & colors
		ofxAppColorsBasic					colorsStorage;
		ofxAppGlobalsBasic					* globalsStorage = nullptr;
	#else
		//crazy macro magic - beware! read a few lines above to see what's going on
		OFX_APP_CLASS_NAME(Colors)			colorsStorage;
		OFX_APP_CLASS_NAME(Globals)			* globalsStorage = nullptr;
	#endif

	ofxAppFonts *							fontStorage = nullptr; //keeps all loaded fonts

	map<std::string, ofxAppContent*>				contentStorage; //App contents parser - indexed by contentID
	map<std::string, ofxApp::ParseFunctions>		contentCfgs; //user supplied custom parsing code - indexed by contentID

	ofPtr<ofxSuperLog> *					loggerStorage; //note its a * to an ofPtr - TODO!

	ofxDrawableStateMachine<ofxApp::State>	appState; //ofxApp State Machine to handle all loading stages
	std::string									errorStateHeader; //holds current error msg header (only applies when state == DEVELOPER_REQUESTED_ERROR_SCREEN)
	std::string									errorStateBody; //holds current error msg body (only applies when state == DEVELOPER_REQUESTED_ERROR_SCREEN)
	
	ofxAppErrorReporter						errorReporterObj; //send live error reports to our CMS over sensu
	ofxGoogleAnalytics *					gAnalytics = nullptr;

	bool									windowIsSetup = false; //will only be true once the window exists.
	float									dt; //inited based on app target framerate settings - used to update some internal objects
	bool									hasLoadedSettings = false;
	bool									timeSampleOfxApp = false; //internal benchmark of ofxApp timings, usually false
	bool 									enableMouse;
	bool									showMouse;
	bool									reportErrors;

	std::string									currentContentID; //keep track of which content are we getting
	vector<std::string>							requestedContent; //complete list of user supplied contentID's
	vector<std::string>							loadedContent; //user supplied contentID's loaded so far

	ofxAppDelegate *						delegate = nullptr; //this will be the "user"'s app, likely an ofBaseApp subclass
	
	const int								loadingScreenFontSize = 22; //TODO!

	//loaded from json settings
	ofVec2f									renderSize;
	ofRectangle								startupScreenViewport; //loading screen rect area

private:

	App(); //you cant make more than 1 ofxApp::get()
	
	//temp log thingies
	std::string currentFrameLog; //this log is a bit different, its cleared every frame
	std::string currentScreenLog; //this log is user controlled, user can add at any time, and clear at any time
	
};

	App& get(); //how to get the app from the ofxApp namespace

} //namespace ofxApp

extern ofxApp::App * global_ofxApp;
//extern OFX_APP_CLASS_NAME(Globals) * ofxApp_globals;
