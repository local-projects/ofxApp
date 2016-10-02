//
//  AppColors.h
//
//  Created by Oriol Ferrer Mesià on 13/04/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxAppUtils.h"

class ofxAppGlobalsAbstract : public HasRuiParams{
public:
	virtual void setupRemoteUIParams() = 0;
};


class ofxAppGlobalsBasic : public ofxAppGlobalsAbstract{

public:
	
	virtual void setupRemoteUIParams(){

		ofLogNotice("ofxAppGlobals") << "setupRemoteUIParams()";
		RUI_NEW_GROUP("OFXAPP GLOBALS");
		RUI_SHARE_PARAM(debug);
		RUI_SHARE_PARAM(drawStaticTexturesMemStats);
		RUI_SHARE_PARAM(drawTextureLoaderStats);
		RUI_SHARE_PARAM(drawTextureLoaderState);

	 };

public:

	bool debug = false;
	bool drawTextureLoaderStats  = false;
	bool drawTextureLoaderState = false;
	bool drawStaticTexturesMemStats = false;

};

