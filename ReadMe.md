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

```c++
class YourApp : public ofBaseApp, public ofxAppDelegate{
}
```

By doing so, you are guaranteed to get some callbacks when certain things happen during the app launch.

## 2. ofxApp Functionality Coverage

_ofxApp_ covers a lot of areas, we will break them down in several sections:


```
TODO!
2.1 Content
2.2 Startup & Init stages
2.3 Static Textures loading Automation
2.4 Maintenance Mode
2.5 Error Screens
...
2.N The Config file `data/configs/ofxAppSettings.json`
```


### 2.1 ofxApp and Content

One of the goals of _ofxApp_ is to abstract content ingestion and sync with a remote CMS for you.

#### 2.1.1 How does ofxApp Handle Content?

_ofxApp_ allows you to go from a JSON sitting on an API endpoint, to a filtered ```vector<YourContentObject*>``` with all the assets listed in the JSON guaranteed to be on disk at the specified paths and ready to go.

![](ReadMeImages/content.png)

_ofxApp_ takes a divide an conquer approach to JSON parsing; it will break up the large JSON into lots of little JSON bits you can parse individually (one per each object defined in the JSON – one per each museum object). It does this for clarity, but also to be able to speed up the parsing of big JSON files by using threads.

_ofxApp_ will download, parse and split up the JSON file into N little bits of JSON (ofxJSONElement objects, a [jsoncpp](https://github.com/open-source-parsers/jsoncpp) Json::Value wrapper) that will deliver to you one by one (through callbacks) so you can focus only in parsing what you need for each object, and create each one of your ```MuseumObjects``` on each callback. You will write the code to parse each object, allowing to use or ignore whatever fields you need, or processing them as desired.

_ofxApp_ will also handle the rejection of invalid objects. What defines a "invalid object" can be configured, but it mostly boils down to assets referenced in the JSON not being there (wrong URL, 404, no URL, etc), or their checksum not checking out (SHA1 mismatch) or some other custom rules (image size too small, missing fields, etc). Through the definition of various policies (ofxAssets::DownloadPolicy, ofxAssets::UsagePolicy), you can define when assets need to be re-downloaded from source (in case they have been modified), if you trust the local files even if the SHA1 doesn't match, etc. And you always have the ultimate say through the parsing lambda you provide to _ofxApp_.

![](ReadMeImages/content2.png)

_ofxApp_ needs you to answer a few questions to be able to fulfill that content delivery:

* __Where are the Objects?__  
	You have to locate where the objects are inside the JSON (in our case, JSON["MuseumObjects"])

* __What fields do you need from each object?__  
	Extract the data that you need from the JSON data, copying it over to the variables defined in your MuseumObject.

* __Is the object valid?__  
	You must decide for each object if it looks valid; i.e. Is it cool if it's missing the Title field? If it's not valid, it will be discarded so it never shows in your app.

* __What is the Object UUID?__  
	_ofxApp_ needs to know each object's UUID. If the JSON is a dictionary of objects, the UUID will be each object's key; but if it's an array, you will have to manually deliver one.

* __Does the object have textures you want to dynamically load and unload?__  
	If you want to use dynamic texture loading, this is where you set it up. You will tell _ofxApp_ what textures does the object have, and where are they located for each resolution.

To answer all the above questions, _ofxApp_ offers you a Function Based protocol; you will be asked to define a few functions that will clarify all these questions. We will see those a bit further in.


#### 2.1.2 The JSON file structure

 _ofxApp_ assumes your content comes in JSON form. It also assumes you will get one JSON file per content source/type. Let's chose a real world example; imagine your project is an interactive wall that aims to show the collection of a Museum. In such a situation, you will probably have 1 JSON file with a list of all the objects in the collection. But you might also have 1 JSON with the list of all the Artist Bios, and one for all the curators, etc. This content is often arranged in a JSON array, or in a JSON dictionary, as shown below in a fictional ```MuseumObjects``` JSON.

```c++
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

```c++
///Objects in JSON dictionary;
{ "MuseumObjects" : {
	"objID1" : {
            "ID" : "objID1",
            "title" : "Magical Chair",
			"imageURL" : "http://museum.com/image1.jpg",
			"imageSha1" : "8d25fa0135fe0a3771dfa09f1331e3ea7c8b888f"
		},
	"objID2" : {
            "ID" : "objID2",
            "title" : "Man standing looking at birds",
			"imageURL" : "http://museum.com/image2.jpg",
			"imageSha1" : "c376992f8a141c388faf6227b9e72749f6065650"
		}
	}
}
```

Let's continue with our example; to hold the information about each object in your JSON you will create your own C++ object, let's call it `MuseumObject`. It will look like similar to this:

```c++
class MuseumObject : public ContentObject{
public:
    string title;
	string imageURL;
	string imageSHA1;
}
```

Note how your ```MuseumObject``` inherits from ```ContentObject```; by doing so your ```MuseumObject``` gets a lot of functionality out of the box. More on that later.

Alos, note how there's one variable to hold each JSON field. The idea here is to mirror the JSON structure pretty closely. In fact, using the name variable names as the JSON field can really help code understanding later on.


#### 2.1.3 ofxApp Content Abstraction

_ofxApp_ abstracts content fetching, parsing, checking and loading through several means. To do so, it defines a class hierarchy for the minimum content unit you will deal with, the ```ContentObject``` class. A ```ContentObject``` is basic unit of content; for example a collection object in a Museum. All your ```MuseumObject``` objects will inherit from ```ContentObject``` to gain its functionality. The ```ContentObject``` also inherits from three other classes, that handle different behaviors:

![](ReadMeImages/contentObjectInher.png)

* __ParsedObject__: minimal class that holds the object UUID. This almost generic class is used across the board to have a common ground for any JSON parseable object, and you don't really interact much with it.

* __AssetHolder__: an object that contains assets (i.e. a museum object with N assets of different kinds). Assets are any kind of file that might be used by the app to represent that object (jpegs, mp4s, aiff's, etc). Once the ContentObject has its AssetHolder properties defined, the process of checking if the assets required for that object are present locally, if they changed or are damaged (through SHA1 checksum), if the object is asset complete, what size are each of the assets, their filetype, etc becomes an automated one.

* __TexturedObject__: A TexturedObject is an object that owns textures (images) at different resolutions that you might want to draw. This allows your object to dynamically load and unload ofTextures on the fly, without halting your app to do so. It does so by progressively loading them, so you first request a texture you need, and you will get notified when its ready (usually a few frames after the request has been placed). When you don't need the texture (because your object is not on screen anymore), you can release it and it will be unloaded for you. This allows your app to hold a lot of image content (ie an entire museum collection - that would be too much to completely pre-load) and keeping interactive framerates when showing it.

#### 2.1.4 Content Sources

For _ofxApp_ to know what content you want in your app, you must tell it what that is. _ofxApp_ expects you to supply a config file in your `data/configs/ofxAppSettings.json` (see section 2.7). In there, there's a whole section about Content Sources named `Content/JsonSources`.

```c++
data/configs/ofxAppSettings.json
{
...
	"Content":{
		"JsonSources":{
			"MuseumObjects" : { //content ID - this is whatever you want to name that content source.
				"url" : "http://uri.cat/LP/MuseumObjects.json", //API endpoint or local JSON file
				"jsonDownloadDir": "MuseumObject_JsonDownloads", //where to store the downloaded JSON
				"assetsLocation": "MuseumObject_assets",  //where the assets for each obj in the JSON will be stored
				"shouldSkipObjectPolicyTests" : false  //set this to true to avoid the object cleanup according to the policies
			},
			"Artists" : { //another content source's contentID
				"url" : "file://testJson/Artists.json",
				"jsonDownloadDir": "Artists_JsonDownloads",
				"assetsLocation": "Artists_assets",  
				"shouldSkipObjectPolicyTests" : false
			}
		}
	}
	...
}
```

As you can see in the JSON config bit above, each Content Source is identified with a contentID of your choosing; in the example above we have two content sources : "MuseumObjects" and "Artists".

Each Content Source must have a few fields defined:

* `url` : this is where _ofxApp_ will get the content JSON from; can be an endpoint (http://www.../API) or a local file (file://testJson/testContent.json).
* `jsonDownloadDir` : this is the path where the json will be stored after downloading it from the source above.
* `assetsLocation`: this is the path where the assets referenced in every object inside the JSON will be downloaded.
* `shouldSkipObjectPolicyTests `: this bool allows skipping the policy tests (more on that later) to accept all the objects in the JSON regardless - usually you want this to be false.

If you suddenly have another content source, just add it in that section with its own unique contentID. _ofxApp_ will ingest the content in that source during the startup phases (more on that later).

#### 2.1.6 Content Ingestion

We have seen how a content JSON might look like, and how you specify your content sources, and some of the classes involved in the different functionalities offered for your content objects. Now we will see how the content is actually converted from JSON to C++ objects you can use. We have seen in 2.1.1 that _ofxApp_ needs to ask you a few questions to be able to parse JSON; here we see how you will answer them through code. We will do so for the `MuseumObject` hypothetical example JSON we've been looking at.

For every content source you want to use, you will have to provide a set of lambda functions to give _ofxApp_ directions on how to parse it. These std::functions are packaged into a struct named [`ParseFunctions`](https://github.com/local-projects/ofxApp/blob/master/src/ofxAppStructs.h#L28).

```c++
struct ParseFunctions{
	std::function<void (ofxMtJsonParserThread::JsonStructureData &)> pointToObjects;
	std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseOneObject;
	std::function<void (ofxApp::CatalogAssetsData &)> defineObjectAssets;
	std::function<void (ContentObject*)> setupTexturedObject;
	ofxJSON userData;
};
```

As you can see, there's 4 `std::function` objects for you to provide code for, and one ofxJSON object for you to place any data you might need. Let's analyze them one by one:

* _**`std::function<void (ofxMtJsonParserThread::JsonStructureData &)> pointToObjects;`**_  

  The goal here is to simply point _ofxApp_ to the content within the JSON content source. For example, in a JSON like this:

  ```c++
  { "MuseumObjects" : [
  		{
  			"ID" : "objID1",
            "title" : "Magical Chair",
  			"imageURL" : "http://museum.com/image1.jpg",
  			"imageSha1" : "8d25fa0135fe0a3771dfa09f1331e3ea7c8b888f"
  		},
  		{
  			"ID" : "objID2",
            "title" : "Man standing looking at birds",
  			"imageURL" : "http://museum.com/image2.jpg",
  			"imageSha1" : "c376992f8a141c388faf6227b9e72749f6065650"
  		}
  	]
  }
  ```

  where the content is in an array under JSON["MuseumObjects"], you want to write something like:

	```c++
	auto pointToObjects = [](ofxMtJsonParserThread::JsonStructureData & inOutData){
		ofxJSONElement & jsonRef = *(inOutData.fullJson);
		if(jsonRef["MuseumObjects"].isArray()){
			inOutData.objectArray = (ofxJSONElement*) &(jsonRef["MuseumObjects"]);
		}else{
			ofLogError("ofApp") << "JSON has unexpected format!";
			//if the json is not what we exepcted it to be,
			//let the parser know by filling it the data like this:
			inOutData.objectArray = nullptr;
		}
	}
	```

  Let's see what's going on here; the `std::function` signature provides you with an argument `ofxMtJsonParserThread::JsonStructureData & inOutData`. This structure is defined in one of the dependencies of _ofxApp_, [`ofxMtJsonParser`](https://github.com/armadillu/ofxMTJsonParser), which is used to handle most of the JSON parsing. The structure is passed to you by reference intentionally, so that you can write back on that object (you are expected to), which is a bit unusual. If see look at the definition of [`JsonStructureData`](https://github.com/local-projects/ofxMTJsonParser/blob/master/src/ofxMtJsonParserThread.h#L30), we see it contains:

  ```c++
  struct JsonStructureData{
		ofxJSONElement * fullJson; //this will provide you the full json data (aka the whole json)
		ofxJSONElement * objectArray; //you are supposed to send back a ptr to the json structure that has the object array OR dictionary you want to parse
  };
  ```

  So the function signature is providing with the whole JSON (as an `ofxJSONElement *`) but also another pointer named `objectArray` for you to "fill in". The provided `JsonStructureData` is being used as a two-way communication tool between _ofxApp_ and you; _ofxApp_ provides a full JSON, and expects you to give it back a pointer to where the content is within that JSON.

  If we look at the implementation proposed above, line by line, you can see that we first cast the `ofxJSONElement *` of the whole JSON doc to a reference (for nicer syntax):

  ```c++
  ofxJSONElement & jsonRef = *(inOutData.fullJson);
  ```

  And then we check if "MuseumObjects" exists in the JSON, and if it is an array (as we expect, because we know beforehand how the JSON is structured)...

  ```c++
  if(jsonRef["MuseumObjects"].isArray()){
  ```

  If the "MuseumObjects looks correct, we point _ofxApp_ to it by "filling in" the inOutData.objectArray pointer:

  ```c++
  inOutData.objectArray = (ofxJSONElement*) &(jsonRef["MuseumObjects]);
  ```

  If the data in the JSON did not match our expectation, we give back a `_nullptr` to _ofxApp_

  ```c++
  inOutData.objectArray = nullptr;
  ```
  And by doing this, _ofxApp_ now knows where to start parsing from.

  ---

* **`std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseOneObject;`**  
  This function will be called once for each object inside JSON["MuseumObjects"]. This is where you allocate and fill in the data for each of your `MuseumObjects*`. _ofxApp_ (with the help of [`ofxMtJsonParser`](https://github.com/armadillu/ofxMTJsonParser)) will take care of splitting the JSON into different smaller JSON bits, so from your point of view, you are parsing a single JSON object. One thing to be aware is that this `std::function` will be called concurrently from different threads; this is done to speed up the parsing (which can get quite slow for large JSON content sets). This should be ok for most cases, but depending on what you do you should be aware of this; a mutex object is provided in case you may need it.

  Let's look at the function signature, and more specifically, what do we get as an argument. _ofxApp_ provides us with a structure named [`SingleObjectParseData`](https://github.com/local-projects/ofxMTJsonParser/blob/master/src/ofxMtJsonParserThread.h#L41).

  ```c++
  struct SingleObjectParseData{
	string objectID; //user is responsible of providing one if the JSON is an Array (instead of a Dictionary)
	ofxJSONElement * jsonObj; //json data for this object		
	ParsedObject * object; //its the user's job to allocate a new ParsedObject,
    int threadID;
    ofMutex * printMutex; //mutex for your convenience
	ofxJSONElement * userData; //custom user data you might need from inside the thread;
    //be extra careful NOT TO WRITE into it from the thread! READ ONLY!
  }
  ```

  The structure holds quite a bit of fields, some of them are not relevant most of the time but may sometimes be needed. Let's start with the important ones:

  * `string objectID`: _ofxApp_ needs a uniqueID for each object in a content source. If the JSON data comes in the form of a  dictionary, the key for each object will be used as a UUID. But if the JSON data comes in Array form, then _ofxApp_ has no way of knowing what the object's UUID is, so you will need to provide it.
  * `ofxJSONElement * jsonObj`: This is a pointer to the JSON data for that object. For our ongoing Museum example, for the first object call would give us this JSON:
  ```
    {
	    "ID" : "objID1",
        "title" : "Magical Chair",
	    "imageURL" : "http://museum.com/image1.jpg",
	    "imageSha1" : "8d25fa0135fe0a3771dfa09f1331e3ea7c8b888f"
	}
  ```

  * `ParsedObject * object`: This is the pointer to final object that will hold all the information you will extract from the JSON. It is your responsibility to allocate the object and assign it to `object*`. The argument will come initialized as `nullptr`; if you leave it as `nullptr` _ofxApp_ will interpret that the object is to be dropped, and it will not show in the app.


  ```c++
  auto parseOneObject = [](ofxMtJsonParserThread::SingleObjectParseData & inOutData){

		const ofxJSONElement & jsonRef = *(inOutData.jsonObj);
		string objID, title, imgURL, imgSha1; //create temp vars to hold the data in JSON

		try{ //do some parsing - catching exceptions
			objID = jsonRef["ID"].asString();
            title = jsonRef["title"].asString();
			imgURL = jsonRef["imageURL"].asString();
			imgSha1 = jsonRef["imageSha1"].asString();
		}catch(exception exc){}

		MuseumObject * o = new MuseumObject(); //allocate our MuseumObject
		o->title = title;
		o->imgURL = imgURL;
		o->imgSha1 = imgSha1;
		inOutData.objectID = objID; //this is how we tell ofxApp the ObjID for this object		
		inOutData.object = dynamic_cast<ParsedObject*> (o); //this is how we return the fully-parsed object to ofxApp;
	};
  ```
  



* **`std::function<void (ofxApp::CatalogAssetsData &)> defineObjectAssets;`**  
  This too  


* **`std::function<void (ContentObject*)> setupTexturedObject;`**  
  This too  


##### 2.1.6.1 ....
##### 2.1.6.2 Customized Parsing Functions

#### 2.1.5 Content Fail and Recovery

What happens if _ofxApp_ starts but the API endpoint is down? What happens if the JSON is malformed and can't be parsed? .... TODO!

## 2.2 ofxApp Startup Stages

So far we have only focused on the JSON content aspect of _ofxApp_, but it loads lots more during startup. In summary, _ofxApp_ does all this at startup, in the following order:

* Create pid file ("data/ofxApp.pid") for clean exit checks.
* Load _ofxApp_ Settings ("data/configs/ofxAppSettings.json")
* Setup Logging; to screen and file ("data/logs/")[ofxSuperLog]
* Setup ofxRemoteUI - for parameter tweaking
* Setup CMS Error Reports (ofxSensu)
* Setup Google Analytics (ofxGoogleAnalytics)
* Load fonts requested in settings file (ofxFontStash, ofxFontStash2)
* Setup a CPU and GPU Profiler (ofxTimeMeasurements)
* Setup TextureLoader (for dynamic texture loading)
* Setup TUIO (for multitouch input)
* User callback to allow you to do custom setup ```ofxAppPhaseWillBegin(State::WILL_LOAD_CONTENT)```
* Load Static textures across N threads (any images at ```data/images```)
* For each JSON content source URL:  
	* Download the JSON file  
	* Ask the user where the data is within the JSON (user supplied Lambda)
	* Load the JSON file / test for its integrity  
	* Parse the JSON file with custom user Lambdas > creating N "ContentObjects"  
	* For each ContentObject
	   * Look at its defined assets, see if they are on disk and checksum matches
       * If asset is missing or has a checksum mismatch, put in a download list
	   * Download files from asset download list - keeping track of the ones that fail  
 	* Remove objects from the ContentObject list that don't comply with the policies
	* Give user a chance to setup TexturedObject for each ContentObject (custom user Lambda)
* For each JSON content source URL
	* Deliver content to the user through the ```ofxAppContentIsReady("ID", vector<ContentObject*>)``` callback.
* Give User a chance to do custom setup with the newly delivered content. ```ofxAppPhaseWillBegin(State::DID_DELIVER_CONTENT)```
* Give User a chance to do custom setup before app starts ```ofxAppPhaseWillBegin(State::WILL_BEGIN_RUNNING)```


Your app will get notified on some of those changes so you can act before / after certain things happen. Each those steps are called an ```ofxApp::Phase```, and that's how _ofxApp_ divides the startup process. Those callbacks are chances for you to load custom content / interface with hardware that you might need before / after other content is loaded. You may not need to do anything in those phases, but an opportunity is given so that you can if you need to.

There are three phases you will get callbacks for;

```c++
enum Phase{
	WILL_LOAD_CONTENT,
	DID_DELIVER_CONTENT,
	WILL_BEGIN_RUNNING
};
```

You will get this callbacks because as we saw before, your app will have to subclass ```ofxAppDelegate``` to use _ofxApp_.

```c++
class YourApp : public ofBaseApp, public ofxAppDelegate{  };
```
By doing so, you adhere to a protocol that includes quite a few callbacks; some of them are designed to allow you to interact with the startup process.

You can see all those callback definitions in the `ofxAppDelegate.h` file, which defines all the deleagte methods _ofxApp_ will need you to respond to.

There's a callback mechanism that allows _ofxApp_ to wait or proceed to the next Phase, according to your needs. The goal here is for ofxApp to give you a chance to do your custom setup at key moments during startup; maybe you need to pre-process all the content before the app starts, maybe you need to connect to some external hardware before you get the content, etc. _ofxApp_ allows you to do your setup when you need to, but also it makes it easy for you to inform the staff of progress or potential problems during setup by letting you control what's drawn on screen during each phase. The mechanism involves 2 callback methods, with a few more optional methods:

```c++
void ofxAppPhaseWillBegin(ofxApp::Phase);
bool ofxAppIsPhaseComplete(ofxApp::Phase);
```

Your app will get the callback `ofxAppPhaseWillBegin(ofxApp::Phase currentPhase)` for every phase in which you can intervene, and you can make _ofxApp_ hold on that phase as long as you want. You control when the phase is complete by returning `true` or `false` to the `bool ofxAppIsPhaseComplete(ofxApp::Phase);` callback. Once a phase has started, _ofxApp_ will fire the `ofxAppIsPhaseComplete` callback asking you if you are done with that phase every frame, if you return `false`, _ofxApp_ will stay on that phase until the next frame, when it will ask you again.

There are more callbacks that you can implement to control the look of the startup process if you are so inclined;

```c++
//override the loading screen drawing
void ofxAppDrawPhaseProgress(ofxApp::Phase, const ofRectangle & r);

//override the progress bar status text
string ofxAppGetStatusString(ofxApp::Phase){return "";};

//override the text message above the progress bar (ie for showing script logs)
string ofxAppGetLogString(ofxApp::Phase){return "";};

//return [0..1] to report progressbar; -1 for
float ofxAppGetProgressForPhase(ofxApp::Phase){return -1;} indeterminate

```

Those will allow you to control how the startup screen looks like as the phase takes place; This is how the startup screen looks for each phase:

<img src="ReadMeImages/initScreen.png" width="740">

The Progress Bar shows the current phase name right in the middle; and the progress is directly mapped to the value you return on the `ofxAppGetProgressForPhase(ofxApp::Phase)` callback. You should return a value in the [0..1] range, or -1 if the progress bar should be indeterminate.

The Progress Bar also allows you to control the little bit of text that goes besides the phase name; basically a quick status update. You can specify this by returning the string in `ofxAppGetStatusString(ofxApp::Phase)`.

The startup process also allows you to show text that will match the overall style of the screen, useful for logs and other information you might want to have on screen while the app starts. This is designed so that you can easily feed it the output of a script, etc. Just return that output (or any text you want drawn) at the `ofxAppGetLogString(ofxApp::Phase)` callback.

And for ultimate customization, you can also completely override what's drawn in the startup screen for a particular phase by just drawing it yourself; implement the `ofxAppDrawPhaseProgress(ofxApp::Phase, const ofRectangle & r)` callback and just draw what you need in the specified bounds, which are roughly the area above the progress bar.


There is also a callback method that will get called N times, depending on how many content sources you setup _ofxApp_ to load. This is callback is executed whenever a content source is ready for you to use;

```c++
void ofxAppContentIsReady(const string & contentID, vector<ContentObject*> content);
```

it contains the `contentID` (which is a string that identifies a content type, ie "MuseumObjects") and the vector containing the list of all the objects in the JSON API that passed all the content filters that are setup.

This diagram shows the most relevant callbacks will receive over time:

<img src="ReadMeImages/callbacks.png" width="740">

### 2.3 Static Textures loading Automation

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

```c++
ofxApp::get().textures().getTexture("image.png"); //returns ofTexture*
ofxApp::get().textures().getTexture("icons/icon1.png");
```

to make things a bit shorter, the following macro is defined in ```ofxAppMacros.h```:

```c++
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

__Things to note about the texture naming:__

* The tex loading properties (```"_t2d"``` and ```"_mip"```) are always removed from the texture name
* The file extension ("jpg", etc) is removed from the texture name
* The upper level dirname ("images" in the example above) is removed from the texture name
* Requesting a missing texture will not crash, but you will get a 16x16 garbage texture + an error log entry


### 2.4 Maintenance Mode

Sometimes you just need to keep an installation off because of extraordinary reasons (ie some hardware is missing, cms is down, etc). _ofxApp_ makes it easy to set a placeholder message on screen when the installation needs maintenance. Inside the config file ```bin/data/ofxAppSettings.json``` there is a section named "MaintenanceMode" (in ```App/MaintenanceMode``` ).

The config file should be pretty self-explanatory; just set "enabled" to true to make the app skip all init & CMS steps and jump straight into the "Maintenance Mode" screen at startup.

```c++
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
* __```LOGV```__ : ofLogVerbose(this* typeID);   
* __```LOGN```__ : ofLogNotice(this* typeID)
* __```LOGW```__ : ofLogWarning(this* typeID)
* __```LOGE```__ : ofLogError(this* typeID)
* __```LOGF```__ : ofLogFatal(this* typeID)


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
