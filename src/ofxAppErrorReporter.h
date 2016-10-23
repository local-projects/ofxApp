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
	
	const string indent = " + ";

	void setup(string host, int port, string email, bool shouldReportErrors,
			   string hostName = "", string hostIP = "", string binaryName = ""){
		
		vector<string>emails;
		emails.push_back(email);
		setup(host, port, email, shouldReportErrors, hostName, hostIP, binaryName);
	}

	
	void setup(string host, int port, const vector<string>& emails, bool shouldReportErrors,
			   string hostName = "", string hostIP = "", string binaryName = ""){
		
		sensu.setup(host, port);
		this->emails = emails;
		enabled = shouldReportErrors;
		
		gitRev = ofSystem("git rev-parse HEAD");
		ofStringReplace(gitRev, "\n", "");
		gitStatus = ofSystem("git status");

		hostInfo = indent + "Platform: " + string(ofGetTargetPlatform() == OF_TARGET_OSX ? "osx" : "win") +
		"\n" + indent + "HostName: " + hostName +
		"\n" + indent + "Host IP: " + hostIP +
		"\n" + indent + "BinaryName: " + binaryName + "\n";
		
		hostInfo += indent + "Git Revision: " + gitRev + "\n" +
					indent + "Git Status: " + gitStatus + "\n";
		
		
	}
	
	void send(string alertName, string msg, int level02, string filePath = ""){
		if(enabled){
			if(filePath == "") filePath = ofToDataPath(ofxSuperLog::getLogger()->getCurrentLogFile(), true);
			string msg2 = addContext(msg);
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << msg << "' level " << level02 ;
			sensu.send(alertName, msg2, ofxSensu::Status(level02), emails, filePath, false);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}

	void send(string alertName, string msg, int level02, vector<string> filePaths){
		if(enabled){
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << addContext(msg) << "' level " << level02;
			string msg2 = addContext(msg);
			sensu.send(alertName, msg2, ofxSensu::Status(level02), emails, filePaths, false);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}
	
	
	
	void sendBlocking(string alertName, string msg, int level02, string filePath = ""){
		if(enabled){
			if(filePath == "") filePath = ofToDataPath(ofxSuperLog::getLogger()->getCurrentLogFile(), true);
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << addContext(msg) << "' level " << level02 ;
			string msg2 = addContext(msg);
			sensu.send(alertName, msg2, ofxSensu::Status(level02), emails, filePath, true);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}

	void sendBlocking(string alertName, string msg, int level02, vector<string> filePaths){
		if(enabled){
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << addContext(msg) << "' level " << level02 ;
			string msg2 = addContext(msg);
			sensu.send(alertName, msg2, ofxSensu::Status(level02), emails, filePaths, true);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}
	

protected:
	
	string addContext(const string& msg){
		
		string msg2 = msg +
		"\n\n/////////////////////////////////////////////////////////////////////////////////////////////////////"+
		
		"\n\n" + hostInfo +
		"\n" + indent + "AppUptime: " + ofxApp::utils::secondsToHumanReadable(ofGetElapsedTimef(), 2) +
		
		"\n\n/////////////////////////////////////////////////////////////////////////////////////////////////////";
		
		return msg2;
	}

	vector<string> emails;
	ofxSensu sensu;
	bool enabled;
	string hostInfo;
	string gitRev;
	string gitStatus;
};

