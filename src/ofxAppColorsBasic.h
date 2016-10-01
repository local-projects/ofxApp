//
//  AppColors.h
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#pragma once

#include "ofMain.h"
#include "ofxAppUtils.h"

class ofxAppColorsAbstract : public HasRuiParams{
public:
	virtual void setupRemoteUIParams() = 0;
};


class ofxAppColorsBasic : public ofxAppColorsAbstract{

public:
	
	virtual void setupRemoteUIParams(){
		ofLogNotice("AppColors") << "setupRemoteUIParams()";
		RUI_NEW_GROUP("APP COLORS");
		RUI_SHARE_COLOR_PARAM(bgColor);
	};

public:

	ofColor bgColor = ofColor(22);
};

