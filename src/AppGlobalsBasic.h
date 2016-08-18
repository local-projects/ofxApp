//
//  AppColors.h
//
//  Created by Oriol Ferrer Mesià on 13/04/15.
//
//

#pragma once

#include "ofMain.h"
#include "AppBaseClasses.h"

class AppGlobalsAbstract : public HasRuiParams{
public:
	virtual void setupRemoteUIParams() = 0;
};


class AppGlobalsBasic : public AppGlobalsAbstract{

public:
	
	virtual void setupRemoteUIParams(){

		ofLogNotice("ofxApp Globals") << "setupRemoteUIParams()";
		RUI_NEW_GROUP("OFXAPP GLOBALS");
		RUI_SHARE_PARAM(debug);
		RUI_SHARE_PARAM(drawTextureLoaderStats);
		RUI_SHARE_PARAM(drawTextureLoaderState);
		RUI_SHARE_PARAM(drawStaticTexturesMemStats);

	 };

public:

	bool debug = false;
	bool drawTextureLoaderStats  = false;
	bool drawTextureLoaderState = false;
	bool drawStaticTexturesMemStats = false;

};

