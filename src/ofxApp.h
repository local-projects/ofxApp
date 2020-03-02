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
	#include "ofxAppGlobalsBasic.h"
#else
	//some macro magic to include the user defined subclasses of ofxAppGlobalsBasic
	//it takes the user defined macro (ie OFX_APP_NAME=MyApp) and creates an #include "MyAppGlobals.h"
	#include OFX_APP_INCLUDE(OFX_APP_NAME,OFX_GLOBALS_FILENAME) //include MyAppGlobals.h
#endif

class ofxGoogleAnalytics;
class ofxMullion;
class ofxTuioClient;

namespace ofxApp{

class App{

	#ifdef TARGET_WIN32
	const std::string FILE_ACCES_ICON = "[!]";
	#else
	const std::string FILE_ACCES_ICON = "💾";
	#endif

public:

	const std::string getset(){return settingsFile;} //path to the settings file
	const std::string LogsDir = "logs";
	const std::string configsDir = "configs";
	const std::string pidFileName = "ofxApp.pid";

	static App& one(){ //this holds the ofxApp::app instance
		static App instance;
		return instance;
	}

	~App();

	App(App const&) = delete; //cant copy construct, or assign
	void operator=(App const&) = delete;

	void setup(ofxAppDelegate * delegate); //if your app has no content ; no lambdas needed
	void setup(const map<std::string,ofxApp::ParseFunctions> & cfgs, ofxAppDelegate * delegate); //if you dont care about content gather order
	void setup(const map<std::string,ofxApp::ParseFunctions> & cfgs, //if you want to override content gather order (defaults to alphabetical by contentID)
			   const map<int, std::string> & contentOrder,
			   ofxAppDelegate * delegate);

	void update(ofEventArgs &);
	void exit(ofEventArgs &);
	void draw(ofEventArgs &);

	// Crazy Macro magic here!! Beware!!
	// this compounds some classnames to match whatever you decided to name your app;
	// so "OFX_APP_CLASS_NAME(Globals)" becomes "MyAppGlobals"
	// "MyApp" is a macro you MUST define in your pre-processor macros: OFX_APP_NAME=MyApp
	// alternativelly, only the default Globals & colors will be defined (ofxAppGlobalsBasic)
	// so you would have to cast the globals to get access to your projec's globals
	#ifdef OFX_APP_NONAME
	ofxAppGlobalsBasic & 			globals(){return *globalsStorage;}
	#else
	OFX_APP_CLASS_NAME(Globals) & 	globals(){return *globalsStorage;} //deprecated! this will be removed in a near future
	#endif
	
	ofxAppFonts &					fonts(){return *fontStorage;}
	ofxJsonSettings & 				settings(){return ofxJsonSettings::get();}
	ofxAppStaticTextures & 			textures(){return texStorage;}
	ofPtr<ofxSuperLog> 				logger(){return ofxSuperLog::getLogger();}
	ofxAppErrorReporter &			errorReporter(){ return errorReporterObj;}
	ofxTuioClient * 					tuio(){ return tuioClient;}
	ofxGoogleAnalytics *				analytics(){ return gAnalytics; }
	ofxScreenSetup					screenSetup;

	// SETTINGS /////////////////////////////////////////////////////////////////////////////
	// Convenience methods to easily get values from "data/configs/ofxAppSettings.json"

	bool&			getBool(const std::string & key, bool defaultVal = true);
	int&				getInt(const std::string & key, int defaultVal = 0);
	float&			getFloat(const std::string & key, float defaultVal = 0.0);
	std::string& 	getString(const std::string & key, const std::string & defaultVal = "uninited!");
	ofColor&			getColor(const std::string & key, ofColor defaultVal = ofColor::red);
	bool				settingExists(const std::string & key);

	void 	setSettingsFilePath(const std::string & ofxAppSettingsJsonPath);
	void		loadSettings(); //load JSON settings (data/configs/ofxAppSettings.json)
	void		loadDynamicSettings(); //load and update values that can be changed while the app runs (call this with 'R' key);
	void		saveSettings();//FIXME: not really used / tested!
	void		setupLogLevelModuleOverrides(bool dynamicLoad);

	//State Machine
	ofxApp::Phase getPhase();//this is the end user state types, you most likely want this
	ofxApp::State getState(){return appState.getState();} //most likely you dont want this
	bool isAppReadyToRun(){return appState.getState() == ofxApp::State::RUNNING;}

	// ERROR STATES / MSGs ///////////////////////////////////////////////////////////////////

	bool enterErrorState(std::string errorHeader, std::string errorBody); //app shows error screen
	bool exitErrorState(); //goes back to RUNNING
	bool isInDevInducedErrorState(){return appState.getState() == State::DEVELOPER_REQUESTED_ERROR_SCREEN;}

	void reportError(const string & alertID, const string & msg, int severity);
	void reportErrorWithFile(const string & alertID, const string & msg, int severity, const string & filePath);

	// RETRIEVE POLICIES /////////////////////////////////////////////////////////////////////
	// Those are cfgs coming from the main config file
	const ofxAssets::DownloadPolicy &	getAssetDownloadPolicy(){ return assetDownloadPolicy; }
	const ofxAssets::UsagePolicy &		getAssetUsagePolicy(){ return assetUsagePolicy;}

	// CALLBACKS /////////////////////////////////////////////////////////////////////////////

	void onRemoteUINotification(RemoteUIServerCallBackArg & arg);
	void onStaticTexturesLoaded();
	void onKeyPressed(ofKeyEventArgs&);
	void screenSetupChanged(ofxScreenSetup::ScreenSetupArg &arg);
	void onWindowResized(ofResizeEventArgs & args);

	// Retrieve app params that come from settings json
	ofRectangle		getRenderAreaForCurrentWindowSize();
	ofRectangle		getRenderRect();
	ofRectangle 		getStartupScreenViewport(){return startupScreenViewport;} //loading screen rect area
	ofVec2f			getRenderSize(){return renderSize;}
	bool				isWindowSetup(){return windowIsSetup;}
	bool 			isJsonContentDifferentFromLastLaunch(std::string contentID, std::string & freshJsonSha1, std::string & oldJsonChecksum);
	bool				getIsUsingOfflineJson(){return usingOfflineJson;}


	// Live Updates //////////////////////////////////////////////////////////////////////////
	bool 			forceLiveUpdate(const std::string & contentID);

	//to draw debug msgs
	ofRectangle		drawMsgInBox(std::string msg, int x, int y, int fontSize, ofColor fontColor, ofColor bgColor = ofColor::black, float edgeGrow = 5);

	void tuioAdded(ofxTuioCursor & tuioCursor);
	void tuioRemoved(ofxTuioCursor & tuioCursor);
	void tuioUpdated(ofxTuioCursor & tuioCursor);
	
	//debug log functionality
	void addToScreenLog(const std::string & str);
	void clearScreenLog();
	
	void addToCurrentFrameLog(const std::string & string);

	bool areKeyEventsEnabled();
	void disableKeyEvents();
	void enableKeyEvents();

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
	int statsFontSize = 15;
	void drawAnimatable();
	void drawMaintenanceScreen();
	void drawErrorScreen();

	string getUpdatedContentURL(const string & contentID); //ask the delegate for the most current contentURL given a contentID

	void updateAnimatable(float dt);

	// STATE MACHINE ///////////////////////////////////////////////////////////////////////////////

	virtual void updateStateMachine(float dt);
	virtual void onDrawLoadingScreenStatus(ofRectangle & area); //override to customize loading screen

	virtual void onSetState(ofxStateMachine<ofxApp::State>::StateChangedEventArgs& change);
	virtual void onStateError(ofxStateMachine<ofxApp::State>::ErrorStateEventArgs& error);
	virtual void onContentManagerStateChanged(std::string&);

	////////////////////////////////////////////////////////////////////////////////////////////////

	ofxTuioClient *						tuioClient = nullptr;
	ofxAppStaticTextures					texStorage;
	ofxMullion *							mullions;

	// Settings Values Bundles ///////////////////////////////////////

	//used by ofxSimpleHttp
	struct HttpDownloadConfig{
		ofxSimpleHttp::ProxyConfig				proxyCfg;
		std::pair<std::string,std::string>		credentials;
		std::map<std::string,std::string>		customHeaders;
	};

	HttpDownloadConfig 						assetDownloadsHttpCfg; //config for any assets to download

	//used by ofxAssets
	ofxAssets::DownloadPolicy				assetDownloadPolicy;
	ofxAssets::UsagePolicy					assetUsagePolicy;
	ofxAssets::ObjectUsagePolicy			objectUsagePolicy;


	// ofxApp various user contents ///////////////////////////////////

	#ifdef OFX_APP_NONAME //if the dev didnt define an app name, use default global vars
		ofxAppGlobalsBasic					* globalsStorage = nullptr;
	#else
		//crazy macro magic - beware! read a few lines above to see what's going on
		OFX_APP_CLASS_NAME(Globals)			* globalsStorage = nullptr;
	#endif

	ofxAppFonts *							fontStorage = nullptr; //keeps all loaded fonts

	map<std::string, ofxAppContent*>			contentStorage; //App contents parser - indexed by contentID
	map<int, std::string>						contentStorageOrder; //order in which to load content, starts at 0
	map<std::string, ofxApp::ParseFunctions>	contentParseFuncs; //user supplied custom parsing code - indexed by contentID
	map<std::string, HttpDownloadConfig>		contentHttpConfigs; //user supplied custom parsing code - indexed by contentID
	map<string,string> 							contentSourceVariables; //used to modify / combine URLs in the "JsonSources"

	ofPtr<ofxSuperLog> *					loggerStorage; //FIXME: note its a * to an ofPtr

	ofxDrawableStateMachine<ofxApp::State>	appState; //ofxApp State Machine to handle all loading stages
	std::string								errorStateHeader; //holds current error msg header (only applies when state == DEVELOPER_REQUESTED_ERROR_SCREEN)
	std::string								errorStateBody; //holds current error msg body (only applies when state == DEVELOPER_REQUESTED_ERROR_SCREEN)
	
	ofxAppErrorReporter						errorReporterObj; //send live error reports to our CMS over sensu
	ofxGoogleAnalytics *					gAnalytics = nullptr;

	bool									windowIsSetup = false; //will only be true once the window exists.
	float									dt; //inited based on app target framerate settings - used to update some internal objects
	bool									hasLoadedSettings = false;
	bool									timeSampleOfxApp = false; //internal benchmark of ofxApp timings, usually false
	bool 									enableMouse;
	bool									showMouse;
	bool									reportErrors;
	bool									usingOfflineJson = false; //is the content we are using not fresh? (setting in the content section inside ofxAppSettings.json)

	bool 								areKeyEventsEnabled_ = false; //os ofxApp reposnding to key events (ie 'L' for logs, etc)


	std::string								currentContentID; //keep track of which content are we currently getting
	vector<std::string>						requestedContent; //complete list of user supplied contentID's
	vector<std::string>						loadedContent; //user supplied contentID's loaded so far

	ofxAppDelegate *						delegate = nullptr; //this will be the "user"'s app, likely an ofBaseApp subclass
	
	//loaded from json settings
	ofVec2f									renderSize;
	ofRectangle								startupScreenViewport; //loading screen rect area

	// Live Updates tracking content structures
	struct LiveUpdateState{
		enum State{ IDLE, RUNNING, READY, FAILED };
		State state = IDLE;
		bool enabled = false;
		bool temporaryRequest = false;
		float interval = 300; //in seconds
		float timer = 0;
		int maxThreads = 1;
		int maxConcurrentDownloads = 1;
	};

	map<std::string, LiveUpdateState>		liveContentUpdates;

private:

	App(); //you cant make more than 1 ofxApp::get()
	
	//temp log thingies
	std::string currentFrameLog; //this log is a bit different, its cleared every frame
	std::string currentScreenLog; //this log is user controlled, user can add at any time, and clear at any time

	std::string settingsFile = "configs/ofxAppSettings.json"; //path to the settings file, can be overriden
	
};

	App& get(); //how to get the app from the ofxApp namespace

} //namespace ofxApp

extern ofxApp::App * global_ofxApp;
//extern OFX_APP_CLASS_NAME(Globals) * ofxApp_globals;
