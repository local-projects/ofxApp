//
//  ofxAppErrorReporter.h
//  PosterPlayback
//
//  Created by Oriol Ferrer Mesià on 10/17/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxAppUtils.h"
#include "ofxSuperLog.h"

class ofxSensu;

class ofxAppErrorReporter{

public:
	
	const string indent = " + ";

	void setup(string host, int port, string email, bool shouldReportErrors,
		string hostName, string hostIP, string binaryName, bool attachGitStatus);
	
	void setup(string host, int port, const vector<string>& emails, bool shouldReportErrors,
		string hostName, string hostIP, string binaryName, bool attachGitStatus);
	
	void send(string alertName, string msg, int level02, string filePath = "");
	void send(string alertName, string msg, int level02, vector<string> filePaths);
	
	
	
	void sendBlocking(string alertName, string msg, int level02, string filePath = "");
	void sendBlocking(string alertName, string msg, int level02, vector<string> filePaths);
	

protected:
	
	void logSend(const string & alertName, const string & msg, int level02, bool blocking);
	string addContext(const string& msg);

	vector<string> emails;
	ofxSensu * sensu = nullptr;
	bool enabled;
	string hostInfo;
	string gitRev;
	string gitStatus;
};

