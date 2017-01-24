//
//  AppColors.h
//
//  Created by Oriol Ferrer Mesià on 13/04/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxAppStructs.h"
#include "ofxRemoteUIServer.h"

class ofxAppGlobalsAbstract : public HasRuiParams{
public:
	virtual ~ofxAppGlobalsAbstract() {};
	virtual void setupRemoteUIParams() = 0;
};


class ofxAppGlobalsBasic : public ofxAppGlobalsAbstract{

public:
	
	virtual void setupRemoteUIParams(){

		ofLogNotice("ofxAppGlobals") << "setupRemoteUIParams()";
		RUI_NEW_GROUP("OFXAPP GLOBALS");
		RUI_SHARE_PARAM(debug);
		RUI_NEW_COLOR();
		RUI_SHARE_PARAM(drawAppRunTime);
		RUI_SHARE_PARAM(drawStaticTexturesMemStats);
		RUI_SHARE_PARAM(drawAutoTextureMemStats);
		RUI_SHARE_PARAM(drawTextureLoaderStats);
		RUI_SHARE_PARAM(drawTextureLoaderState);
		
		RUI_NEW_GROUP("OFXAPP PROTOTYPE VARS");
		RUI_SHARE_PARAM(tempFloat1, -1, 1);
		RUI_SHARE_PARAM(tempFloat4k, 0, 3840);

	 };

	virtual ~ofxAppGlobalsBasic() {};

public:

	bool debug = false;
	bool drawAppRunTime = false;
	bool drawTextureLoaderStats  = false;
	bool drawTextureLoaderState = false;
	bool drawStaticTexturesMemStats = false;
	bool drawAutoTextureMemStats = false;
	
	float tempFloat1 = 0;
	float tempFloat4k = 0;
};

