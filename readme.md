Dont Forge't to define the preprocessor macro 
```
OFX_APP_NAME=MyApp
```

and to name your classes & files accordingly.

```
MyAppColors.h
MyAppGlobals.h
```


TODO
add a toggle for "service mode" for when the app is "down" so we can inform visitors the installation is not currently working. 

---

#ofxApp

What is ofxApp? Its the basic skeleton for an interactive installation made in OpenFrameworks. It tries to simplify your life by offering some basic functionality without much required on your side.

##Features
A quick summary of the features offered by ofxApp:

 * Startup / Loading Screen with progress (ofxStateMachine)
 * CMS Asset download/cache/checksum management (ofxSimpleHttp, ofxAssets, ofxTagSystem)
 * CMS JSON content loading & failover recovery (ofxMTJsonParser)
 * Time based profiler (ofxTimeMeasurements)
 * Dynamic texture load / unload (TexturedObject)
 * Logging to file & screen (ofxSuperLog)
 * Parameter Editing (ofxRemoteUI)
 * Error Reporting to CMS (ofxSensu)
 * Static Texture Assets preloading + access by fileName + dynamic edits (ofxAutoTexture)
 * Analytics (ofxGoogleAnalytics)
 * Fonts (ofxFontStash)
 * TUIO setup (ofxTuio)
 * Screen Management (fullscreen, windows, etc) (ofxScreenSetup)
 * All behaviors highly configurable from a single config file (ofxJsonSettings)
 
 

##How To Use

#####1 - Setup a project with all the required addons:

```
ofxOSC
ofxXmlSettings
ofxFontStash
ofxTuio
ofxJson
ofxSuperLog
ofxAutoTexture
*ofxApp
ofxMtJsonParser
ofxSimpleHttp
ofxTagSystem
ofxStateMachine
ofxAssets
ofxThreadSafeLog
ofxProgressiveTextureLoad
ofxTexturedObject
ofxOpenCV
ofxMullion
ofxSensu
ofxNetwork
ofxGoogleAnalytics
ofxJsonSettings
ofxTimeMeasurements
ofxRemoteUI
ofxHistoryPlot
ofxScreenSetup
```

#####2 - Define Pre Processor macro with your project name (Optional):

```
OFX_APP_NAME=MyApp
```
This is used to automatically handle your custom Global Variables & Global Colors files.

#####3 - Create files for your Global Variables & Global Colors:

These files must follow this naming convention; and it's where you will place global variables/colors that you might need to access from anywhere. These files are only required if you defined ```OFX_APP_NAME``` in the previous step

```
OFX_APP_NAME + Globals.h
OFX_APP_NAME + Colors.h
```
so, if your app is named MyApp, your files should be named "MyAppGlobals.h" and "MyAppColors.h".

Those two files should be classes that inherit from a basic set of globals that ofxApp defines for you; you just extend them with your own as your project grows.

Look at the example to see how they look like.

#####4 - Make your onApp class a subclass of ofxAppDelegate

This is 

class ofApp : public ofBaseApp, public ofxAppDelegate{