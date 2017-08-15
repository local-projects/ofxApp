//
//  AppColors.h
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#pragma once

#include "ofMain.h"
#include "ofxAppStructs.h"
#include "ofxRemoteUIServer.h"

class ofxAppColorsAbstract : public HasRuiParams{
public:
	virtual void setupRemoteUIParams() = 0;
};


class ofxAppColorsBasic : public ofxAppColorsAbstract{

public:
	
	virtual void setupRemoteUIParams(){
		ofLogNotice("ofxAppColors") << "setupRemoteUIParams()";
		RUI_SHARE_COLOR_PARAM(bgColor);
	};

public:

	ofColor bgColor = ofColor(22);
};

