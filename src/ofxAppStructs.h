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

class ContentObject;

namespace ofxApp{

	//user created lambdas to do custom actions at the crucial points
	struct UserLambdas{
		std::function<void (ofxMtJsonParserThread::JsonStructureData &)> describeJsonUserLambda;
		std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseSingleObjectUserLambda;
		std::function<void (ContentObject*)> setupTexturedObjectUserLambda;
	};

	enum State{
		SETTING_UP,
		SETUP_USER_APP_PRE_LOAD_CONTENT,
		LOADING_STATIC_TEXTURES,
		LOADING_JSON_CONTENT,
		LOADING_JSON_CONTENT_FAILED,
		DELIVER_CONTENT_LOAD_RESULTS,
		SETUP_USER_APP_POST_LOAD_CONTENT,
		SETUP_USER_APP_JUST_B4_RUNNING,
		RUNNING,
	};

	enum UserAppSetupStage {
		SETUP_B4_CONTENT_LOAD = SETUP_USER_APP_PRE_LOAD_CONTENT,
		RECEIVE_CONTENT_LOAD_RESULTS = DELIVER_CONTENT_LOAD_RESULTS,
		SETUP_AFTER_CONTENT_LOAD = SETUP_USER_APP_POST_LOAD_CONTENT,
		SETUP_JUST_B4_RUNNING = SETUP_USER_APP_JUST_B4_RUNNING
	};

	inline string toString(State s){
		switch(s){
			case SETTING_UP: return "SETTING_UP";
			case LOADING_STATIC_TEXTURES: return "LOADING_STATIC_TEXTURES";
			case LOADING_JSON_CONTENT: return "LOADING_JSON_CONTENT";
			case LOADING_JSON_CONTENT_FAILED: return "LOADING_JSON_CONTENT_FAILED";
			case DELIVER_CONTENT_LOAD_RESULTS: return "DELIVER_CONTENT_LOAD_RESULTS";
			case SETUP_USER_APP_POST_LOAD_CONTENT: return "SETUP_USER_APP_POST_LOAD_CONTENT";
			case SETUP_USER_APP_JUST_B4_RUNNING: return "SETUP_USER_APP_JUST_B4_RUNNING";
			case RUNNING: return "RUNNING";
			default: break;
		}
		ofLogError("ofxApp") << "unknown ofxApp State!";
		return "unknown ofxApp State";
	}

	inline string toString(UserAppSetupStage s) {
		switch (s) {
			case SETUP_B4_CONTENT_LOAD: return "SETUP_B4_CONTENT_LOAD";
			case RECEIVE_CONTENT_LOAD_RESULTS: return "RECEIVE_CONTENT_LOAD_RESULTS";
			case SETUP_AFTER_CONTENT_LOAD: return "SETUP_AFTER_CONTENT_LOAD";
			case SETUP_JUST_B4_RUNNING: return "SETUP_JUST_B4_RUNNING";
			default: break;
		}
		ofLogError("ofxApp") << "unknown ofxApp UserAppSetupStage!";
		return "unknown ofxApp UserAppSetupStage";
	}

};

class HasRuiParams{
public:
	virtual void setupRemoteUIParams() = 0;
};
