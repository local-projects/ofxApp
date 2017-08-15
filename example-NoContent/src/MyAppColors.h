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
		RUI_SHARE_COLOR_PARAM(myColor);
	};

	ofColor myColor;
};

