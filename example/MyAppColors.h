//
//  MyAppColors.h
//
//  Created by Oriol Ferrer Mesià on 13/04/15.
//
//

#pragma once

#include "AppColorsBasic.h"

class MyAppColors : public AppColorsBasic{

public:
	
	void setupRemoteUIParams(){
		AppColorsBasic::setupRemoteUIParams();
		RUI_SHARE_COLOR_PARAM(myColor);
	};

public:

	ofColor myColor;
};

