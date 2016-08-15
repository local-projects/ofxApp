//
//  MyAppColors.h
//
//  Created by Oriol Ferrer Mesià on 13/04/15.
//
//

#pragma once

#include "AppGlobalsBasic.h"

class MyAppGlobals : public AppGlobalsBasic{

public:
	
	void setupRemoteUIParams(){
		AppGlobalsBasic::setupRemoteUIParams();
		RUI_NEW_GROUP("APP PARAMS");
		RUI_SHARE_PARAM(myParam, 0, 1);
	};

public:

	float myParam = 0.5;
};

