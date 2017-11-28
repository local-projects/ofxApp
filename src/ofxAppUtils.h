//
//  ofxAppUtils.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//


#pragma once
#include "ofMain.h"
#include "ofxAppMacros.h"

namespace ofxApp{
	
	namespace utils{

		struct ImageInfo{
			bool valid;
			int width;
			int height;
			int nChannels;
		};

		void assertFileExists(const std::string & path);
		void terminateApp(const std::string & module, const std::string & reason, float secondsOnScreen = 15);
				
		ofColor createHsbColor(float h, float sat, float brightness); //[0..255.0f]
	
		std::string secondsToHumanReadable(float secs, int decimalPrecision);
		std::string bytesToHumanReadable(long long bytes, int decimalPrecision);
		
		std::string getNewUUID();
		std::string getFileSystemSafeString(const std::string & input);

		std::string getGlInfo();
		std::string getGlError();

		bool loadTexture(ofTexture & tex, const std::string & path, bool mipmap, float bias, int anisotropy);

		bool isValidEmail(const std::string email);

		void logBanner(const std::string & log); //to make prettier log headers
		void logParagraph(const std::string & moduleName, ofLogLevel lev, const std::string & text);

		ImageInfo getImageDimensions(const std::string & filePath);
		
		std::string toString(ofLogLevel l);

	} //utils
}; //ofxApp
