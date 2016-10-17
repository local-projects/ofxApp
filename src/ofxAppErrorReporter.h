//
//  ofxAppErrorReporter.h
//  PosterPlayback
//
//  Created by Oriol Ferrer Mesià on 10/17/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxSensu.h"
#include "ofxAppUtils.h"
#include "ofxSuperLog.h"


class ofxAppErrorReporter{

public:
	
	void setup(string host, int port, string email, bool shouldReportErrors,
			   string hostName = "", string hostIP = "", string binaryName = ""){
		
		sensu.setup(host, port);
		this->email = email;
		enabled = shouldReportErrors;
		hostInfo = hostName + ", " + hostIP + ", " + binaryName;
		
		gitRev = ofSystem("git rev-parse HEAD");
		gitStatus = ofSystem("git status");
		
	}
	
	void send(string alertName, string msg, int level02, string filePath = ""){
		if(enabled){
			if(filePath == "") filePath = ofToDataPath(ofxSuperLog::getLogger()->getCurrentLogFile(), true);
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << addContext(msg) << "' level " << level02 ;
			sensu.send(alertName, msg, ofxSensu::Status(level02), email, filePath, false);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}

	void send(string alertName, string msg, int level02, vector<string> filePaths){
		if(enabled){
			vector<string> emails = {email};
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << addContext(msg) << "' level " << level02 ;
			sensu.send(alertName, msg, ofxSensu::Status(level02), emails, filePaths, false);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}
	
	
	
	void sendBlocking(string alertName, string msg, int level02, string filePath = ""){
		if(enabled){
			if(filePath == "") filePath = ofToDataPath(ofxSuperLog::getLogger()->getCurrentLogFile(), true);
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << addContext(msg) << "' level " << level02 ;
			sensu.send(alertName, msg, ofxSensu::Status(level02), email, filePath, true);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}

	void sendBlocking(string alertName, string msg, int level02, vector<string> filePaths){
		if(enabled){
			vector<string> emails = {email};
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << addContext(msg) << "' level " << level02 ;
			sensu.send(alertName, msg, ofxSensu::Status(level02), emails, filePaths, true);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}
	

protected:
	
	string addContext(const string& msg){
		string msg2 = msg +
						"\nHostInfo: " + hostInfo +
						"\nAppUptime: " + //ofxApp::utils::secondsToHumanReadable(ofGetElapsedTimef(), 2) +
						"\ngitRev:" + gitRev +
						"\ngitStatus: " + gitStatus;
		
		return msg2;
	}

	string email;
	ofxSensu sensu;
	bool enabled;
	string hostInfo;
	string gitRev;
	string gitStatus;
};

