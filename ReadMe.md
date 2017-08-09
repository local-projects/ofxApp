# ofxApp
[![Build Status](https://travis-ci.org/local-projects/ofxApp.svg?branch=master)](https://travis-ci.org/local-projects/ofxApp)
[![Build status](https://ci.appveyor.com/api/projects/status/c9uonq86tsf0tlu5/branch/master?svg=true)](https://ci.appveyor.com/project/armadillu/ofxapp/branch/master)

What is _ofxApp_? Its a basic skeleton for an interactive CMS-driven installation made in [OpenFrameworks](http://openframeworks.cc). It tries to simplify your life by offering some basic common functionality required by most projects, trying to minimize the effort required on your side to get an installation up and running.

[![ofxApp Startup](ReadMeImages/videoThumb.png)](https://www.youtube.com/watch?v=vQaj6wCZZqs "ofxApp Startup")

There are also some (slightly outdated) [Slides](https://github.com/local-projects/ofxApp/raw/master/introduction.pdf) that might help you understand what _ofxApp_ offers and how it works internally.

## 0. Feature List

A quick summary of the features offered by _ofxApp_:

 * Startup / Loading Screen split up in user manageable phases with on screen progress ([ofxStateMachine](https://github.com/armadillu/ofxStateMachine))
 * CMS Asset download/cache/checksum management ([ofxSimpleHttp](https://github.com/armadillu/ofxSimpleHttp), [ofxAssets](https://github.com/armadillu/ofxAssets), [ofxTagSystem](https://github.com/armadillu/ofxTagSystem))
 * CMS JSON content loading & failover recovery ([ofxJSON](https://github.com/local-projects/ofxJSON), [ofxMTJsonParser](https://github.com/armadillu/ofxMTJsonParser))
 * CPU & GPU profiler ([ofxTimeMeasurements](https://github.com/armadillu/ofxTimeMeasurements))
 * Dynamic texture load / unload ([ofxTexturedObject](https://github.com/armadillu/ofxTexturedObject), [ofxProgressiveTextureLoad](https://github.com/armadillu/ofxProgressiveTextureLoad))
 * Logging to console, file & screen (interactive) ([ofxSuperLog](https://github.com/armadillu/ofxSuperLog), [ofxThreadSafeLog](https://github.com/armadillu/ofxThreadSafeLog))
 * Parameter Tweaking ([ofxRemoteUI](https://github.com/armadillu/ofxRemoteUI))
 * Error Reporting to CMS ([ofxSensu](https://github.com/local-projects/ofxSensu))
 * Static Texture Assets preloading + access by fileName + live edits ([ofxAutoTexture](https://github.com/armadillu/ofxAutoTexture))
 * Analytics ([ofxGoogleAnalytics](https://github.com/armadillu/ofxGoogleAnalytics))
 * Font rendering ([ofxFontStash](https://github.com/armadillu/ofxFontStash), [ofxFontStash2](https://github.com/armadillu/ofxFontStash2))
 * Multitouch setup ([ofxTuio](https://github.com/local-projects/ofxTuio))
 * Screen Management (fullscreen, windows, etc) ([ofxScreenSetup](https://github.com/armadillu/ofxScreenSetup))
 * All the behaviours highly configurable from a single config file ([ofxJsonSettings](https://github.com/loca-projects/ofxJsonSettings))

## 1. How To Use

### 1.1 - Setup a project with all the required addons:

ofxOSC, ofxXmlSettings, ofxOpenCV, ofxNetwork, ofxPoco,  [ofxFontStash](https://github.com/armadillu/ofxFontStash), [ofxFontStash2](https://github.com/armadillu/ofxFontStash2), [ofxTuio](https://github.com/local-projects/ofxTuio), [ofxJSON](https://github.com/local-projects/ofxJSON), [ofxSuperLog](https://github.com/armadillu/ofxSuperLog), [ofxAutoTexture](https://github.com/armadillu/ofxAutoTexture), [ofxMTJsonParser](https://github.com/armadillu/ofxMTJsonParser), [ofxSimpleHttp](https://github.com/armadillu/ofxSimpleHttp), [ofxTagSystem](https://github.com/armadillu/ofxTagSystem), [ofxStateMachine](https://github.com/armadillu/ofxStateMachine), [ofxAssets](https://github.com/armadillu/ofxAssets), [ofxThreadSafeLog](https://github.com/armadillu/ofxThreadSafeLog), [ofxProgressiveTextureLoad](https://github.com/armadillu/ofxProgressiveTextureLoad), [ofxTexturedObject](https://github.com/armadillu/ofxTexturedObject), [ofxMullion](https://github.com/armadillu/ofxMullion), [ofxSensu](https://github.com/local-projects/ofxSensu),  [ofxGoogleAnalytics](https://github.com/armadillu/ofxGoogleAnalytics), [ofxJsonSettings](https://github.com/loca-projects/ofxJsonSettings), [ofxTimeMeasurements](https://github.com/armadillu/ofxTimeMeasurements), [ofxRemoteUI](https://github.com/armadillu/ofxRemoteUI), [ofxHistoryPlot](https://github.com/armadillu/ofxHistoryPlot), [ofxScreenSetup](https://github.com/armadillu/ofxScreenSetup),
[ofxInterface](http://github.com/local-projects/ofxInterface), [ofxApp*](https://github.com/local-projects/ofxApp).

To do so, given the high number of addon dependencies, I suggest you use the OpenFrameworks Project Generator. This applies to Windows and OS X platforms.

* Get OpenFrameworks 0.9.8 or newer, development is done against the OF github master branch.
* Launch the OF Project Generator for your platform and set it up.
* Set a "Project Path", where do you want to create your Project.
* Set a "Project Name", the name you want your project binary to have.
* Click "Generate" to create the project; keep Project Generator open.
* Go to ```Your_Repo/ExternalAddons/ofxApp/``` (or ```/addons/ofxApp``` if you use vanilla OF) and copy the file ```addon_config.make``` to your new project folder; and rename it to ```addons.make```.
* Back to Project Generator, click on the "Project Path" texfield once to make the app refresh - the addons list should update. Click on "Update" to re-create your project files. At this point, all the addons dependencies for _ofxApp_ should be covered.
* Add these __PreProcessor Macros__ to your project: ```USE_OFX_FONTSTASH```, ```USE_OFX_FONTSTASH2```, ```USE_OFX_HISTORYPLOT```, ```NANOVG_GL2_IMPLEMENTATION``` (\*or NANOVG_GL3_IMPLEMENTATION) and ```OFX_APP_NAME=xxxx``` (where xxxx is your "Project Name" - see 1.2).
* Your project should compile cleanly now.

![project generator](https://farm5.staticflickr.com/4430/36424424406_bbd6020214_o_d.png)

### 1.2 - Define Pre-Processor macro with your project name:

```
OFX_APP_NAME=MyApp
```
This is used to automatically handle your custom Global Variables & Global Colors files. This is so that your Global Vars and Colors classes are automatically included in the _ofxApp_ instance. The whole point of this is to avoid casting.

### 1.3 - Create files for your Global Variables & Global Colors:

These Files must follow this naming convention; and it's where you will place global variables/colors that you might need to access from anywhere. These files are only required if you defined ```OFX_APP_NAME``` in the previous step

```
OFX_APP_NAME + Globals.h
OFX_APP_NAME + Colors.h
```
so, if your app is named MyApp, your files should be named "MyAppGlobals.h" and "MyAppColors.h".

Those two files should be classes that inherit from a basic set of globals that _ofxApp_ defines for you; you just extend them with your own as your project grows.

Look at the example to see how they look like.

### 1.4 - Make your ofApp class a subclass of ofxAppDelegate

This usually means your ofApp (a subclass of ofBaseApp, the basic OpenFrameworks app) should also subclass ofxAppDelegate, as shown here:

```
class ofApp : public ofBaseApp, public ofxAppDelegate{

}
```

By doing so, you are guaranteed to get some callbacks when certain things happen during the app launch.

## 2. ofxApp Functionality Coverage

_ofxApp_ covers a lot of areas, we will break them down in several sections:


```
2.1 Content
2.2 Startup & init stages
```

### 2.1 ofxApp Content Abstraction

_ofxApp_ abstracts content fetching, checking and loading through several means; it's basic unit is the ```ContentObject```. A ContentObject is basic unit of content; for example a collection object in a museum. Your objects will have to inherit from ContentObject to gain its functionality. The ```ContentObject``` class inherits from 3 classes that handle different behaviors:

![](ReadMeImages/contentObjectInher.png)

* __ParsedObject__: minimal class that holds the object UUID. This almost generic class is used across the board to have a common ground for any JSON parseable object, and you don't really interact much with it.

* __AssetHolder__: an object that contains assets (i.e. a museum object with N assets of different kinds). Once the ContentObject has its AssetHolder properties defined, the process of checking if the assets required for that object are present locally, if they changed or are damaged (SHA1 checksum), if the object is asset complete, what size are each of the assets, they filetype, etc becomes an automated one.

* __TexturedObject__: this allows your object to dynamically load and unload ofTextures on the fly, without halting your app to do so. It does so by progressively loading them, so you first request a texture you need, and you will get notified when its ready (usually a few frames after the request has been placed). When you don't need the texture (because your object is not on screen anymore), you can release it and it will be unloaded for you.


### The JSON file structure

 Usually you have a JSON file for each content type; for example, the objects of a museum collection will all live in a single JSON file. They are often arranged in a JSON array, or in a JSON dictionary, as shown below:

```
///Objects in JSON array; objectID must be supplied per object
{ "MuseumObjects" : [
		{
			"ID" : "objID1",
			"imageURL" : "http://museum.com/image1.jpg",
			"imageSha1" : "8d25fa0135fe0a3771dfa09f1331e3ea7c8b888f"
		},
		{
			"ID" : "objID2",
			"imageURL" : "http://museum.com/image2.jpg",
			"imageSha1" : "c376992f8a141c388faf6227b9e72749f6065650"
		}
	]
}
```

```
///Objects in JSON dictionary;
{ "MuseumObjects" : {
	"objID1" : {
			"imageURL" : "http://museum.com/image1.jpg",
			"imageSha1" : "8d25fa0135fe0a3771dfa09f1331e3ea7c8b888f"
		},
	"objID2" : {
			"imageURL" : "http://museum.com/image2.jpg",
			"imageSha1" : "c376992f8a141c388faf6227b9e72749f6065650"
		}
	}
}
```

To attach all this to a real world example, let's assume we are working on a project for a museum that has a collection of objects, whose contents are stored in a JSON like the one defined above. We will create our own C++ object and we will call it ```MuseumObject```. It will look like this:

```
class MuseumObject : public ContentObject{
public:
	string imageURL;
	string imageSHA1;
}
```

### What is it ofxApp does for you?

_ofxApp_ allows you to go from a JSON sitting on an API endpoint, to a filtered ```vector<MuseumObject*>``` with all the assets listed in the JSON guaranteed to be on disk and ready to go.

![](ReadMeImages/content.png)

_ofxApp_ takes a divide an conquer approach to parsing; it will break up the JSON into lots of little JSON bits you can parse individually (one per each object defined in the JSON). It does this for clarity, but also to be able to speed up the parsing of big JSON files by using threads.

_ofxApp_ will download, parse and split up the JSON file into N little bits of JSON (ofxJSONElement, a [jsoncpp](https://github.com/open-source-parsers/jsoncpp) Json::Value wrapper) that will deliver to you one by one (through callbacks) so you can focus only in parsing what you need for each object, and create each one of your "MuseumObjects" on each callback.

What defines a "invalid object" can be configured, but it mostly boils down to assets referenced in the JSON not being there (wrong URL), or their checksum not checking out (SHA1 mismatch) or some other custom rules (image size too small, missing fields, etc). Through de definition of various policies (ofxAssets::DownloadPolicy, ofxAssets::UsagePolicy), you can define when assets need to be re-downloaded, if you trust the local files even if the SHA1 doesn't match, etc. And you always have the ultimate say through the parsing lambda you provide to _ofxApp_.


![](ReadMeImages/content2.png)

_ofxApp_ needs you to answer a few questions to be able to deliver on that promise:

* __Where are the Objects?__
	You have to locate objects are inside the JSON (in our case, JSON["MuseumObjects"])

* __What fields do you need from each object?__  
	Extract the data that you need from the JSON data, copying it over to your custom MuseumObject.

* __Is the object valid?__  
	You must decide for each object if it looks valid; i.e. Is it cool if it's missing the title field?

* __What is the Object UUID?__  
	_ofxApp_ needs to know each object's UUID. If the JSON is a dictionary of objects, the UUID will be each object's key; but if it's an array, you will have to manually deliver.

* __Does the object have textures you want to dynamically load and unload?__  
	If you want to use dynamic texture loading, this is where you set it up the TexturedObject par of each ContentAsset.

To answer all the above questions, _ofxApp_ offers you a Function Based protocol; you will define 3 functions that will clarify all these questions.


## ofxApp Startup Stages

So far we have only focused on the JSON content aspect of _ofxApp_, but it loads lots more during startup. In summary, _ofxApp_ does all this at startup, in the following order:

* Create pid file ("data/ofxApp.pid")
* Load _ofxApp_ Settings ("data/configs/ofxAppSettings.json")
* Setup Logging; to screen and file ("data/logs/")[ofxSuperLog]
* Setup ofxRemoteUI
* Setup Error Reports (ofxSensu)
* Setup Google Analytics (ofxGoogleAnalytics)
* Load fonts requested in settings file (ofxFontStash)
* Setup Time Profiler (ofxTimeMeasurements)
* Setup TextureLoader
* Setup TUIO
* User callback to do custom setup ```ofxAppPhaseWillBegin(State::WILL_LOAD_CONTENT)```
* Load Static textures across N threads (any images at data/images)
* For each JSON content source URL:  
	* Download the JSON file  
	* Ask the user where the data is within the JSON (Lambda)
	* Load the JSON file / test for its integrity  
	* Parse the JSON file with custom user Lambdas > creating N "ContentObjects"  
	* For each ContentObject  
	 	* Look at its defined assets, see if they are on disk and checksum matches  
		* If asset is missing or has a checksum mismatch, put in a download list  
		* Download files from asset download list - keeping track of the ones that fail
 	* Remove objects from the ContentObject list that don't comply with the policies
	* Give user a chance to setup TexturedObject for each ContentObject (Lambda)
* For each JSON content source URL
	* Deliver content to the user. ```ofxAppContentIsReady("ID", vector<ContentObject*>)```
* Give User a chance to do custom work with the newly delivered content. ```ofxAppPhaseWillBegin(State::DID_DELIVER_CONTENT)```
* Give User a chance to do custom setup before app starts ```ofxAppPhaseWillBegin(State::WILL_BEGIN_RUNNING)```


and your app will get notified on some of those changes so you can act before / after certain things happen. Those callbacks are chances for you to load custom content that you might need before / after other content is loaded. You don't need to explicitly do anything in those phases, but an opportunity is given so that you may if you need to.

There's a callback mechanism that allows _ofxApp_ to wait or proceed to the next Phase, according to your needs. The mechanism involves 2 callback methods:



```
ofxAppPhaseWillBegin(ofxApp::Phase);
ofxAppIsPhaseComplete(ofxApp::Phase);
```

There is also a callback method that will get called N times, depending on how many content pieces you setup _ofxApp_ to load. This is:

```
void ofxAppContentIsReady(const string & contentID, vector<ContentObject*>);
```


First, _ofxApp_ will call ofxAppPhaseWillBegin(ofxApp::Phase) on the delegate (your app), informing it is time for the delegate to do whatever it needs to do in that particular phase.

There are 4 specified Phases in which you can act:


#### 1. ofxApp::Phase::SETUP_B4_CONTENT_LOAD

This will get called... TODO!

#### 2. ofxApp::Phase::RECEIVE_CONTENT
#### 3. ofxApp::Phase::SETUP_AFTER_CONTENT_LOAD
#### 4. ofxApp::Phase::LAST_SETUP_B4_RUNNING


## Maintenance Mode

Sometimes you just need to keep an installation off because of extraordinary reasons (ie some hardware is missing, cms is down, etc). _ofxApp_ makes it easy to set a placeholder message on screen when the installation needs maintenance. Inside the config file ```bin/data/ofxAppSettings.json``` there is a section named "MaintenanceMode" (in ```App/MaintenanceMode``` ).

The config file should be pretty self-explanatory; just set "enabled" to true to make the app skip all init & CMS steps and jump straight into the "Maintenance Mode" screen at startup.

```
"MaintenanceMode":{
	"enabled" : true,
	"layout" : {
		"x" : 0.5, //this is the normalized X location on screen (0.5 = middle of screen)
		"y" : 0.46, //this is the normalized Y location on screen (0.5 = middle of screen)
		"width" : 0.75, //this is the normalized width of the text column (based on screen Width)
		"rotation" : 0.0, //in degrees
		"scale" : 1.0 //scale up or down as desired
	},
	"header" : {
		"text" : "Zzzzzzzz",
		"fontScaleup" : 1.0,
		"spacing" : 6.0,
		"fontID" : "regular", //this is the ofxFontStash2 ID, built in fontIDs are "mono", "monoBold", "regular", "bold"
		"color" : [0,228,130]
	},
	"body" : {
		"text" : "Sorry! The wall is resting.\nIt will be back at work soon.",
		"fontScaleup" : 1.0,
		"spacing" : 0.0,
		"fontID" : "regular", //this is the ofxFontStash2 ID, built in fontIDs are "mono", "monoBold", "regular", "bold"
		"color" : [255]
	},
	"bgColor" : [0,0,0,255]
},
```

This config would give you a screen like this:

![maintenance.PNG](ReadMeImages/maintenance.PNG)

## ofxApp Keyboard Commands

* __'W'__ : cycle through different window modes (see ofxScreenSetup for mode list)
* __'L'__ : toggle on screen log (mouse & TUIO interactive)
* __'M'__ : toggle mullions	(for microtile/videowall grid preview)
* __'R'__ : reload _ofxApp_ settings file (data/configs/ofxAppSettings.json) for live changes
* __'D'__ : toggle debug mode (changes state of the global var "debug");

---

## MACROS

* __```GLOB```__ : direct access to MyAppGlobals
* __```G_COL | G_COLOR```__ : direct access to MyAppColors
* __```G_TEX("texName")```__ : direct acces to the ofTexture*
* __```G_FONT("fontID")```__ : direct acces to the ofxFontStash*
* __```G_FONT_MONO```__ : direct acces to a monospaced ofxFontStash*
* __```G_FONT_MONO_BOLD```__ : direct acces to a bold monospaced ofxFontStash*
* __```G_FS2```__ : direct acces to ofxFontStash2 font manager
* __```G_FSTYLE("styleID")```__ : get access to a specific ofxFontstash2::Style (styles defined in cfg file)
* __```OFXAPP_REPORT(alertID,msg,severity)```__ : send an ofxSensu alert (that may trigger email on the CMS)
* __```OFXAPP_REPORT_FILE(alertID,msg,severity)```__ : send an ofxSensu alert (to the CMS) with a file attachment
* __```OFXAPP_ANALYTICS()```__ : direct access to ofxGoogleAnalytics*
* __```LOGV```__ : ofLogVerbose(this* typeID)[^1]
* __```LOGN```__ : ofLogNotice(this* typeID)
* __```LOGW```__ : ofLogWarning(this* typeID)
* __```LOGE```__ : ofLogError(this* typeID)
* __```LOGF```__ : ofLogFatal(this* typeID)


[^1]: must be inside an in-class method for it to work; Use like this: ```LOGV << "hello!";```


## Static Textures Automation

_ofxApp_ has a module named ofxAppStaticTextures that aims to simplify access to everyday textures; usually those textures that you need available all the time, and you never dynamically load & unload them; this includes things like icons and such.

As the project evolves, you often find yourself having to add extra images to have ready as ofTextures. Instead of instantiating and hardcoding the path to the texture within code, ofxAppStaticTextures allows a different approach. It will just load any image file sitting in ```data/images/``` recursively at startup, across multiple threads to speed up the process. It will also display them all as they load, so you can keep an eye on textures you might not need loaded anymore and can keep your images dir clean.

For example, given this directory structure for ```data/images```:

```
images/
	image1.png
	icons/
		icon1.png
		icon2.png
```

you can access the ofTexture from code in this way:

```
ofxApp::get().textures().getTexture("image.png"); //returns ofTexture*
ofxApp::get().textures().getTexture("icons/icon1.png");
```

to make things a bit shorter, the following macro is defined in ```ofxAppMacros.h```:

```
G_TEX("icons/icon1.png")->draw(0,0); //access global texture
```

Often you need fine grain control over how these textures are loaded; maybe some of them need to be ```GL_TEXTURE_2D``` because you want your tex coordinates normalized, maybe you want some to have mipmaps created and some not. To allow this fine level of detail, ofxAppStaticTextures allows you to embed how you need the texture loaded in the filename.

There are two filename modifiers you can add to the end of your texture files:

* **```"_t2d"```**: the texture should be loaded as ```GL_TEXTURE_2D```
* **```"_mip"```**: the texture should have with mipmaps.

For example, files named like this, will receive this treatment:

* **```"img.png"```** : will load as ```GL_TEXTURE_RECTANGLE_ARB``` and no mipmaps
* **```"img_mip.png"```** : will load as ```GL_TEXTURE_2D``` with mipmaps
* **```"img_t2d_mip.png"```** : will load as ```GL_TEXTURE_2D``` with mipmaps
* **```"img_t2d.png"```** : will load as ```GL_TEXTURE_2D``` but no mipmaps will be created

#### Things to note about the texture naming:

* The tex loading properties (```"_t2d"``` and ```"_mip"```) are always removed from the texture name
* The file extension ("jpg", etc) is removed from the texture name
* The upper level dirname ("images" in the example above) is removed from the texture name
* Requesting a missing texture will not crash, but you will get a 16x16 garbage texture + an error log entry




#### TODO

+ Globals + Colors macro file naming gimmick is not ideal
+ unhappy about AppSettings.json sharing "default" options with user-defined-content src options
+ _ofxApp_ assumes the app is using a GLFW window...
+ ```renderSize``` in settings unclear...
+ loading screen messages font size as param in settings
+ settings save method? some params (like debug) are linked to ofxRemoteUI, so it could make sense... but lots aren't.
+ unused asset cleanup!


## Included Libraries / assets

* [stb_image](https://github.com/nothings/stb) by Sean Barrett: Public domain
* [Ubuntu] (http://font.ubuntu.com) Font Family : [Ubuntu Font License](http://font.ubuntu.com/ufl/ubuntu-font-licence-1.0.txt)
* [Montserrat](https://www.fontsquirrel.com/fonts/montserrat) Font Family : [SIL Open Font License](https://www.fontsquirrel.com/license/montserrat)
* [Pacifico](https://www.fontsquirrel.com/fonts/pacifico) Font (used in example): [SIL Open Font License](https://www.fontsquirrel.com/license/pacifico)
* [Fantasque Sansa Mono](https://fontlibrary.org/en/font/fantasque-sans-mono) Font (used in example): [SIL Open Font License](http://scripts.sil.org/cms/scripts/page.php?site_id=nrsi&id=OFL)
