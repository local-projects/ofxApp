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

class ofxAppErrorReporter{

public:
	
	void setup(string host, int port, string email){
		sensu.setup(host, port);
		this->email = email;
	}
	
	void send(string alertName, string msg, int level02, string filePath){
		sensu.send(alertName, msg, ofxSensu::Status(level02), email, filePath, false);
	}

	void send(string alertName, string msg, int level02, vector<string> filePaths){
		vector<string> emails = {email};
		sensu.send(alertName, msg, ofxSensu::Status(level02), emails, filePaths, false);
	}
	
	void sendBlocking(string alertName, string msg, int level02, string filePath){
		sensu.send(alertName, msg, ofxSensu::Status(level02), email, filePath, true);
	}

	void sendBlocking(string alertName, string msg, int level02, vector<string> filePaths){
		vector<string> emails = {email};
		sensu.send(alertName, msg, ofxSensu::Status(level02), emails, filePaths, true);
	}
	

protected:

	string email;
	ofxSensu sensu;
};

