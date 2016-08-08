//
//  HasRuiParams.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#pragma once
#include "ofxRemoteUIServer.h"
#include "ofMain.h"

class HasRuiParams{
public:
	virtual void setupRemoteUIParams() = 0;
};


class HasAssets{
public:
	static void assertFileExists(string path){
		if(!ofFile::doesFileExist(path)){
			ofLogError("HasAssets") << "Required asset not present: '" << path << "'";
			ofLogError("HasAssets") << "Terminating app";
			ofExit();
			exit(-1);
		}else{
			//ofLogNotice("HasAssets") << "Confirmed asset is present: '" << path << "'";
		}
	}
};