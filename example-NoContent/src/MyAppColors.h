//
//  MyAppColors.h
//
//  Created by Oriol Ferrer Mesià on 13/04/15.
//
//

#pragma once

#include "ofxAppColorsBasic.h"

class MyAppColors : public ofxAppColorsBasic{

public:
	
	void setupRemoteUIParams(){
		ofxAppColorsBasic::setupRemoteUIParams();
		RUI_SHARE_COLOR_PARAM(myColor);
	};

public:

	ofColor myColor;
};

