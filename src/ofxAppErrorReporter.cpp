//
//  ofxAppErrorReporter.h
//  PosterPlayback
//
//  Created by Oriol Ferrer Mesià on 10/17/16.
//
//

#include "ofxAppErrorReporter.h"
#include "ofxSensu.h"

void ofxAppErrorReporter::setup(std::string host, int port, std::string email, bool shouldReportErrors,
			std::string hostName, std::string hostIP, std::string binaryName, bool attachGitStatus){
		
	vector<std::string>emails;
	emails.push_back(email);
	setup(host, port, email, shouldReportErrors, hostName, hostIP, binaryName, attachGitStatus);
}

	
void ofxAppErrorReporter::setup(std::string host, int port, const vector<std::string>& emails, bool shouldReportErrors,
			std::string hostName , std::string hostIP , std::string binaryName, bool attachGitStatus ){
	
	sensu = new ofxSensu();
	sensu->setup(host, port);
	this->emails = emails;
	enabled = shouldReportErrors;
		
	if(attachGitStatus){
		gitRev = ofSystem("git rev-parse HEAD");
		ofStringReplace(gitRev, "\n", "");
		gitStatus = ofSystem("git status");
	}

	hostInfo = indent + "Platform: " + std::string(ofGetTargetPlatform() == OF_TARGET_OSX ? "osx" : "win") +
	"\n" + indent + "HostName: " + hostName +
	"\n" + indent + "Host IP: " + hostIP +
	"\n" + indent + "BinaryName: " + binaryName + "\n";
	
	if(attachGitStatus){
		hostInfo += indent + "Git Revision: " + gitRev + "\"n";
		hostInfo += indent + "Git Status: " + gitStatus + "\n";
	}
}
	
void ofxAppErrorReporter::send(std::string alertName, std::string msg, int level02, std::string filePath){
	if(enabled){
		if(filePath == "") filePath = ofToDataPath(ofxSuperLog::getLogger()->getCurrentLogFile(), true);
		std::string msg2 = addContext(msg);
		logSend(alertName, msg, level02, false);
		sensu->send(alertName, msg2, ofxSensu::Status(level02), emails, filePath, false);
	}else{
		if(level02 != 0) ofLogNotice("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
	}
}

void ofxAppErrorReporter::send(std::string alertName, std::string msg, int level02, vector<std::string> filePaths){
	if(enabled){
		logSend(alertName, msg, level02, false);
		std::string msg2 = addContext(msg);
		sensu->send(alertName, msg2, ofxSensu::Status(level02), emails, filePaths, false);
	}else{
		if(level02 != 0) ofLogNotice("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
	}
}
	
	
	
void ofxAppErrorReporter::sendBlocking(std::string alertName, std::string msg, int level02, std::string filePath){
	if(enabled){
		if(filePath == "") filePath = ofToDataPath(ofxSuperLog::getLogger()->getCurrentLogFile(), true);
		logSend(alertName, msg, level02, true);
		std::string msg2 = addContext(msg);
		sensu->send(alertName, msg2, ofxSensu::Status(level02), emails, filePath, true);
	}else{
		if(level02 != 0) ofLogNotice("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
	}
}

void ofxAppErrorReporter::sendBlocking(std::string alertName, std::string msg, int level02, vector<std::string> filePaths){
	if(enabled){
		logSend(alertName, msg, level02, true);
		std::string msg2 = addContext(msg);
		sensu->send(alertName, msg2, ofxSensu::Status(level02), emails, filePaths, true);
	}else{
		if(level02 != 0) ofLogNotice("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
	}
}
	

void ofxAppErrorReporter::logSend(const std::string & alertName, const std::string & msg, int level02, bool blocking) {
	switch (level02) {
		case 0: ofLogNotice("ofxAppErrorReporter") << "Send RESOLVE Report " << std::string(blocking ? "blocking " : "") << "'" << alertName << "' : '" << msg << "'"; break;
		case 1: ofLogWarning("ofxAppErrorReporter") << "Send WARNING Report " << std::string(blocking ? "blocking " : "") << "'" << alertName << "' : '" << msg << "'"; break;
		case 2: ofLogError("ofxAppErrorReporter") << "Send CRITICAL Report " << std::string(blocking ? "blocking " : "") << "'" << alertName << "' : '" << msg << "'"; break;
		default: ofLogError("ofxAppErrorReporter") << "Send UNKNOWN LEVEL Report " << std::string(blocking ? "blocking " : "") << "'" << alertName << "' : '" << msg << "'"; break;
	}
}

std::string ofxAppErrorReporter::addContext(const std::string& msg){
		
	std::string msg2 = msg +
	"\n\n/////////////////////////////////////////////////////////////////////////////////////////////////////"+
		
	"\n\n" + hostInfo +
	"\n" + indent + "AppUptime: " + ofxApp::utils::secondsToHumanReadable(ofGetElapsedTimef(), 2) +
		
	"\n\n/////////////////////////////////////////////////////////////////////////////////////////////////////";
		
	return msg2;
}
