//
//  ofxAppStructs.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 3/8/16.
//
//

#pragma once
//#include "AssetHolder.h"
#include "ofxMtJsonParser.h"

namespace ofxApp{

	struct ContentConfig{
		std::function<void (ofxMtJsonParserThread::JsonStructureData &)> describeJsonUserLambda;
		std::function<void (ofxMtJsonParserThread::SingleObjectParseData &)> parseSingleObjectUserLambda;
	};


};