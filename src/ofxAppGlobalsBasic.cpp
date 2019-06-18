//
//  ofxAppGlobalsBasic.cpp
//
//  Created by Oriol Ferrer Mesià on 13/04/15.
//
//
#include "ofxAppGlobalsBasic.h"

void ofxAppGlobalsBasic::setupRemoteUIParams(){

	ofLogNotice("ofxAppGlobals") << "setupRemoteUIParams()";
	RUI_NEW_GROUP("OFXAPP GLOBALS");
	RUI_SHARE_COLOR_PARAM(bgColor);
	RUI_SHARE_PARAM(debug);
	RUI_NEW_COLOR();
	RUI_SHARE_PARAM(drawAppRunTime);
	RUI_SHARE_PARAM(drawStaticTexturesMemStats);
	RUI_SHARE_PARAM(drawAutoTextureMemStats);
	RUI_SHARE_PARAM(drawTextureLoaderStats);
	RUI_SHARE_PARAM(drawTextureLoaderState);
	RUI_SHARE_PARAM(drawGoogleAnalyticsState);
	RUI_SHARE_PARAM(drawScreenLogs);
	RUI_SHARE_PARAM(drawLiveUpdateStatus);

	if(createTestingExtraParams){

		RUI_NEW_GROUP("OFXAPP PROTOTYPE VARS");
		RUI_SHARE_PARAM(tempFloat1, -1, 1);
		RUI_SHARE_PARAM(tempFloat4k, 0, 3840);

		RUI_NEW_GROUP("OFX_ANIMATABLE TEST CURVES");
		RUI_SHARE_PARAM(drawTempAnimCurve1);
		RUI_NEW_COLOR();
		RUI_SHARE_ENUM_PARAM(tempAnimCurve1, 0, NUM_ANIM_CURVES - 1, ofxAnimatable::getAllCurveNames());

		RUI_NEW_COLOR();
		TAC_quadBezierA = 0.87; TAC_quadBezierB = 0.18;
		RUI_SHARE_PARAM(TAC_quadBezierA, 0, 1);
		RUI_SHARE_PARAM(TAC_quadBezierB, 0, 1);

		RUI_NEW_COLOR();
		TAC_expSigmoidSteep = 0.65;
		RUI_SHARE_PARAM(TAC_expSigmoidSteep, 0, 1);

		RUI_NEW_COLOR();
		TAC_cubicBezAx = 0.19; TAC_cubicBezAy = 0.91;
		TAC_cubicBezBx = 0.97; TAC_cubicBezBy = 0.2;
		RUI_SHARE_PARAM(TAC_cubicBezAx, -0.5, 2);
		RUI_SHARE_PARAM(TAC_cubicBezAy, -0.5, 2);
		RUI_NEW_COLOR();
		RUI_SHARE_PARAM(TAC_cubicBezBx, -0.5, 2);
		RUI_SHARE_PARAM(TAC_cubicBezBy, -0.5, 2);

		RUI_NEW_COLOR();
		TAC_elasticG = 0.26; TAC_elasticFreq = 0.52; TAC_elasticDecay = 0.08;
		RUI_SHARE_PARAM(TAC_elasticG, 0, 1);
		RUI_SHARE_PARAM(TAC_elasticFreq, 0, 1);
		RUI_SHARE_PARAM(TAC_elasticDecay, 0, 1);

		RUI_NEW_COLOR();
		TAC_easeOutOffset = -0.78;
		RUI_SHARE_PARAM(TAC_easeOutOffset, -4, 4);

		RUI_NEW_COLOR();
		TAC_bounceNum = 4; TAC_bounceElast = 0.38;
		RUI_SHARE_PARAM(TAC_bounceNum, 0, 10);
		RUI_SHARE_PARAM(TAC_bounceElast, 0, 1);
		RUI_NEW_COLOR();
		RUI_SHARE_ENUM_PARAM(tempAnimCurve2, 0, NUM_ANIM_CURVES - 1, ofxAnimatable::getAllCurveNames());
	}

};
