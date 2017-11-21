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
			   string hostName, string hostIP, string binaryName, bool attachGitStatus){
		
		vector<string>emails;
		emails.push_back(email);
		setup(host, port, email, shouldReportErrors, hostName, hostIP, binaryName, attachGitStatus);
	}

	
	void setup(string host, int port, const vector<string>& emails, bool shouldReportErrors,
			   string hostName , string hostIP , string binaryName, bool attachGitStatus ){
		
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
		
		hostInfo += indent + "Git Revision: " + gitRev + "\"n";

		if(attachGitStatus){
			hostInfo += indent + "Git Status: " + gitStatus + "\n";
		}
	}
	
	void send(string alertName, string msg, int level02, string filePath = ""){
		if(enabled){
			if(filePath == "") filePath = ofToDataPath(ofxSuperLog::getLogger()->getCurrentLogFile(), true);
			string msg2 = addContext(msg);
			logSend(alertName, msg, level02, false);
			sensu.send(alertName, msg2, ofxSensu::Status(level02), emails, filePath, false);
		}else{
			if(level02 != 0) ofLogNotice("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}

	void send(string alertName, string msg, int level02, vector<string> filePaths){
		if(enabled){
			logSend(alertName, msg, level02, false);
			string msg2 = addContext(msg);
			sensu.send(alertName, msg2, ofxSensu::Status(level02), emails, filePaths, false);
		}else{
			if(level02 != 0) ofLogNotice("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}
	
	
	
	void sendBlocking(string alertName, string msg, int level02, string filePath = ""){
		if(enabled){
			if(filePath == "") filePath = ofToDataPath(ofxSuperLog::getLogger()->getCurrentLogFile(), true);
			logSend(alertName, msg, level02, true);
			string msg2 = addContext(msg);
			sensu.send(alertName, msg2, ofxSensu::Status(level02), emails, filePath, true);
		}else{
			if(level02 != 0) ofLogNotice("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}

	void sendBlocking(string alertName, string msg, int level02, vector<string> filePaths){
		if(enabled){
			logSend(alertName, msg, level02, true);
			string msg2 = addContext(msg);
			sensu.send(alertName, msg2, ofxSensu::Status(level02), emails, filePaths, true);
		}else{
			if(level02 != 0) ofLogNotice("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}
	

protected:
	
	void logSend(const string & alertName, const string & msg, int level02, bool blocking) {
		switch (level02) {
			case 0: ofLogNotice("ofxAppErrorReporter") << "Send RESOLVE Report " << string(blocking ? "blocking " : "") << "'" << alertName << "' : '" << msg << "'"; break;
			case 1: ofLogWarning("ofxAppErrorReporter") << "Send WARNING Report " << string(blocking ? "blocking " : "") << "'" << alertName << "' : '" << msg << "'"; break;
			case 2: ofLogError("ofxAppErrorReporter") << "Send CRITICAL Report " << string(blocking ? "blocking " : "") << "'" << alertName << "' : '" << msg << "'"; break;
			default: ofLogError("ofxAppErrorReporter") << "Send UNKNOWN LEVEL Report " << string(blocking ? "blocking " : "") << "'" << alertName << "' : '" << msg << "'"; break;
		}
	}

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

