//
//  HasRuiParams.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#include "ofxAppUtils.h"
#include "ofxApp.h"


namespace ofxApp{
namespace utils{


	void assertFileExists(const string & path){
		if(!ofFile::doesFileExist(path)){
			string msg = "Required asset not present: '" + path + "'";
			ofLogFatalError("HasAssets") << msg;
			ofxApp::utils::terminateApp("ofxApp", msg);
		}else{
			//ofLogNotice("HasAssets") << "Confirmed asset is present: '" << path << "'";
		}
	}

		
	void terminateApp(const string & module, const string & reason, float secondsOnScreen){
		
		ofLogFatalError("ofxApp") << "terminateApp()!";
		ofxSimpleHttp::destroySslContext();
		ofLogFatalError("ofxApp") << "";
		ofLogFatalError("ofxApp") << "-----------------------------------------------------------------------------------------------------------";
		ofLogFatalError("ofxApp") << "";
		ofLogFatalError("ofxApp") << "ofxApp is terminating because the module \"" << module << "\" found an unrecoverable error.";
		ofLogFatalError("ofxApp") << "\"" << reason << "\"";
		ofLogFatalError("ofxApp") << "This message will be on screen for " << (int)secondsOnScreen << " seconds, then the app will quit.";
		ofLogFatalError("ofxApp") << "";
		ofLogFatalError("ofxApp") << "-----------------------------------------------------------------------------------------------------------";
		ofLogFatalError("ofxApp") << "";
		ofxThreadSafeLog::one()->close();
		ofxSuperLog::getLogger()->setScreenLoggingEnabled(true); //show log if json error
		ofxSuperLog::getLogger()->getDisplayLogger().setPanelWidth(1.0);
		int numFrames = secondsOnScreen * 1000 / 16; //stay up a bit so that you can read logs on screen
		
		OFXAPP_REPORT("ofxAppTerminate_" + module, reason, 2);
		
		//hijack OF and refresh screen & events by hand
		if(ofGetWindowPtr()){
			for(int i = 0; i < numFrames; i++ ){
				ofSetupScreen();
				ofClear(0,0,0,255);
				ofxSuperLog::getLogger()->getDisplayLogger().draw(ofGetWidth(), ofGetHeight());
				ofGetMainLoop()->pollEvents();
				if(ofGetWindowPtr()->getWindowShouldClose()){
					ofLogFatalError("ofxApp") << "Quitting by user action";
					std::exit(-1);
				}
				ofGetWindowPtr()->swapBuffers();
				ofSleepMillis(16);
			}
		}
		std::exit(0);
	};

	ofColor createHsbColor(float h, float sat, float brightness){ //[0..255.0f]
		ofColor c;
		c.setHsb(h,sat,brightness);
		return c;
	}
	
	string secondsToHumanReadable(float secs, int decimalPrecision){
		string ret;
		if (secs < 60.0f ){ //if in seconds
			ret = ofToString(secs, decimalPrecision) + " seconds";
		}else{
			if (secs < 3600.0f){ //if in min range
				ret = ofToString(secs / 60.0f, decimalPrecision) + " minutes";
			}else{ //hours or days
				if (secs < 86400.0f){ // hours
					ret = ofToString(secs / 3600.0f, decimalPrecision) + " hours";
				}else{ //days
					if (secs < 86400.0f * 7.0f){ // days
						ret = ofToString(secs / (86400.0f), decimalPrecision) + " days";
					}else{ //days
						ret = ofToString(secs / (86400.0f * 7.0f) , decimalPrecision) + " weeks";
					}
				}
			}
		}
		return ret;
	}
	
	
	string bytesToHumanReadable(long long bytes, int decimalPrecision){
		string ret;
		if (bytes < 1024 ){ //if in bytes range
			ret = ofToString(bytes) + " bytes";
		}else{
			if (bytes < 1024 * 1024){ //if in kb range
				ret = ofToString(bytes / float(1024), decimalPrecision) + " KB";
			}else{
				if (bytes < (1024 * 1024 * 1024)){ //if in Mb range
					ret = ofToString(bytes / float(1024 * 1024), decimalPrecision) + " MB";
				}else{
					ret = ofToString(bytes / float(1024 * 1024 * 1024), decimalPrecision) + " GB";
				}
			}
		}
		return ret;
	}
	
	string getNewUUID(){
		static char alphabet[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
		string s;
		for(int i = 0; i < 8; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
		s += "-";
		for(int i = 0; i < 4; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
		s += "-4";
		for(int i = 0; i < 3; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
		s += "-a";
		for(int i = 0; i < 3; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
		s += "-";
		for(int i = 0; i < 12; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
		return s;
	}
	
	
	string getFileSystemSafeString(const string & input){
		static char invalidChars[] = {'?', '\\', '/', '*', '<', '>', '"', ';', ':', '#' };
		int howMany = sizeof(invalidChars) / sizeof(invalidChars[0]);
		char replacementChar = '_';
		string output = input;
		for(int i = 0; i < howMany; i++){
			std::replace( output.begin(), output.end(), invalidChars[i], replacementChar);
		}
		return output;
	}

//namespaces
}}