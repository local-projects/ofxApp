//
//  ofxAppGlobalsBasic.h
//
//  Created by Oriol Ferrer Mesià on 13/04/15.
//
//

#pragma once

#include "ofMain.h"
#include "ofxAppStructs.h"
#include "ofxRemoteUIServer.h"
#include "ofxAnimatableFloat.h"

class ofxAppGlobalsBasic : public HasRuiParams{

public:
	
	virtual void setupRemoteUIParams();
	virtual ~ofxAppGlobalsBasic() {};

	void setShouldCreateExtraTestingParams(bool d){createTestingExtraParams = d;}

public:

	ofColor bgColor = ofColor(22);

	bool debug = false;
	bool drawAppRunTime = false;
	bool drawTextureLoaderStats  = false;
	bool drawTextureLoaderState = false;
	bool drawStaticTexturesMemStats = false;
	bool drawAutoTextureMemStats = false;
	bool drawGoogleAnalyticsState = false;
	bool drawScreenLogs = true;
	bool drawLiveUpdateStatus = false;
	
	float tempFloat1 = 0;
	float tempFloat4k = 0;
	
	//animatable curve types for quick tests
	bool drawTempAnimCurve1 = false;
	AnimCurve tempAnimCurve1 = TANH;
	float TAC_quadBezierA;
	float TAC_quadBezierB;
	float TAC_expSigmoidSteep;
	float TAC_cubicBezAx;
	float TAC_cubicBezAy;
	float TAC_cubicBezBx;
	float TAC_cubicBezBy;
	float TAC_elasticG;
	float TAC_elasticFreq;
	float TAC_elasticDecay;
	float TAC_easeOutOffset;
	int TAC_bounceNum;
	float TAC_bounceElast;
	AnimCurve tempAnimCurve2 = EXPONENTIAL_SIGMOID_PARAM;

	ofxAnimatableFloat tempAnimCurveInstance;

	//
	bool createTestingExtraParams = true;
};

