//
//  AppColors.h
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#pragma once

#include "ofMain.h"
#include "AppBaseClasses.h"
#include "ofxAsset.h"
#include "ofxSimpleHttp.h"
#include "ofxDownloadCentral.h"
#include "ofxMtJsonParser.h"
#include "AssetChecker.h"

class AppContentBasic{

public:

	virtual void update(float dt) = 0;

	virtual bool foundError() = 0;
	virtual bool isContentReady() = 0;
	virtual string getStatus() = 0;
	virtual float getPercentDone() = 0;
	virtual bool isBusy() = 0;

protected:

	AssetChecker assetChecker;
	ofxDownloadCentral dlc;
};

