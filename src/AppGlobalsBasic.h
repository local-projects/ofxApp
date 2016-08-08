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

		ofLogNotice("AppGlobals") << "setupRemoteUIParams()";
		RUI_NEW_GROUP("AppGlobals");
		RUI_SHARE_PARAM(debug);
	 };

	
public:

	bool debug = false;
};

