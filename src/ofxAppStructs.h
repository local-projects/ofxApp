//
//  ofxAppStructs.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 3/8/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxAppMacros.h"
#include "ofxMtJsonParser.h"
#include "ofxAssets.h"
#include "TexturedObject.h"

//all your content objects will have to subclass this class
class ContentObject : public ParsedObject, public AssetHolder, public TexturedObject{

public:

	ContentObject(){ getNumInstances()++; }
	virtual ~ContentObject(){
		getNumInstances()--;
	};

	// Imposed by TexturedObject //
	virtual ofVec2f getTextureDimensions(TexturedObjectSize, int){ return ofVec2f(0,0);}
	virtual std::string getLocalTexturePath(TexturedObjectSize, int){ return "";}

	//this is effectively the destructor of the object - bc of texturedObject loading assets
	//in secondary threads, we can't use an in-place destructor as you could destruct the object
	//while a thread is loading it - thus we have this GarbageCollector-like behavior.
	virtual void deleteWithGC(){}

	//you can at any time during any of the ofxApp::ParseFunctions (parseOneObject, defineObjectAssets, setupTexturedObject)
	//decide you don't want the object by setting isValid to false.
	bool isValid = true;

	static int getNumTotalObjects(){return getNumInstances();}

private:

	static int& getNumInstances(){static int numInstances = 0; return numInstances;}
};


namespace ofxApp{

	struct CatalogAssetsData{ //data sent to the user for him/her to report object assets
		ContentObject * object;
		ofxJSONElement * userData;
		std::string assetsLocation;
		ofxAssets::DownloadPolicy assetDownloadPolicy;
		ofxAssets::UsagePolicy assetUsagePolicy;
	};

	//user created lambdas to do custom actions at the crucial points
	struct ParseFunctions{
		std::function<void (ofxMtJsonParserThread::JsonStructureData &)> pointToObjects;
		std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseOneObject;
		std::function<void (ofxApp::CatalogAssetsData &)> defineObjectAssets;
		std::function<void (ContentObject*)> setupTexturedObject;
		ofxJSON userData;
	};

	enum class State : int{ //internal states used by an ofxApp app
		SETUP_OFXAPP_INTERNALS,
		SETUP_DELEGATE_B4_CONTENT_LOAD,
		LOAD_STATIC_TEXTURES,
		LOAD_JSON_CONTENT,
		LOAD_JSON_CONTENT_FAILED,
		DELIVER_CONTENT_LOAD_RESULTS,
		SETUP_DELEGATE_B4_RUNNING,
		RUNNING,
		MAINTENANCE,
		DEVELOPER_REQUESTED_ERROR_SCREEN
	};

	//ofxAppDelegate exposed states; as they are a subset of the internal ofxApp states, enum
	//values are matched to them
	enum class Phase : int{
		WILL_LOAD_CONTENT = (int)State::SETUP_DELEGATE_B4_CONTENT_LOAD,
		DID_DELIVER_CONTENT = (int)State::DELIVER_CONTENT_LOAD_RESULTS,
		WILL_BEGIN_RUNNING = (int)State::SETUP_DELEGATE_B4_RUNNING,
		UNKNOWN_PHASE = (int)1024
	};

	//convenience methods to be able to print state names
	inline std::string toString(const State & s){
		switch(s){
			case State::SETUP_OFXAPP_INTERNALS: return "SETUP_OFXAPP_INTERNALS";
			case State::SETUP_DELEGATE_B4_CONTENT_LOAD: return "SETUP_DELEGATE_B4_CONTENT_LOAD";
			case State::LOAD_STATIC_TEXTURES: return "LOAD_STATIC_TEXTURES";
			case State::LOAD_JSON_CONTENT: return "LOAD_JSON_CONTENT";
			case State::LOAD_JSON_CONTENT_FAILED: return "LOAD_JSON_CONTENT_FAILED";
			case State::DELIVER_CONTENT_LOAD_RESULTS: return "DELIVER_CONTENT_LOAD_RESULTS";
			case State::SETUP_DELEGATE_B4_RUNNING: return "SETUP_DELEGATE_B4_RUNNING";
			case State::RUNNING: return "RUNNING";
			case State::MAINTENANCE: return "MAINTENANCE";
			case State::DEVELOPER_REQUESTED_ERROR_SCREEN: return "DEVELOPER_REQUESTED_ERROR_SCREEN";
			default: break;
		}
		ofLogError("ofxApp") << "unknown ofxApp State!";
		return "unknown ofxApp State";
	}

	inline std::string toString(const Phase& s){
		switch (s) {
			case Phase::WILL_LOAD_CONTENT: return "WILL_LOAD_CONTENT";
			case Phase::DID_DELIVER_CONTENT: return "DID_DELIVER_CONTENT";
			case Phase::WILL_BEGIN_RUNNING: return "WILL_BEGIN_RUNNING";
			default: break;
		}
		ofLogError("ofxApp") << "unknown ofxApp Phase!";
		return "unknown ofxApp Phase";
	}

};


//mostly protocol enforcment for globals & colors
class HasRuiParams{
public:
	virtual void setupRemoteUIParams() = 0;
};
