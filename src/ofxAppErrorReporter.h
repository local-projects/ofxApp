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
	
	void setup(string host, int port, string email, bool shouldReportErrors){
		sensu.setup(host, port);
		this->email = email;
		enabled = shouldReportErrors;
	}
	
	void send(string alertName, string msg, int level02, string filePath){
		if(enabled){
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << msg << "' level " << level02 ;
			sensu.send(alertName, msg, ofxSensu::Status(level02), email, filePath, false);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}

	void send(string alertName, string msg, int level02, vector<string> filePaths){
		if(enabled){
			vector<string> emails = {email};
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << msg << "' level " << level02 ;
			sensu.send(alertName, msg, ofxSensu::Status(level02), emails, filePaths, false);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}
	
	void sendBlocking(string alertName, string msg, int level02, string filePath){
		if(enabled){
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << msg << "' level " << level02 ;
			sensu.send(alertName, msg, ofxSensu::Status(level02), email, filePath, true);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}

	}

	void sendBlocking(string alertName, string msg, int level02, vector<string> filePaths){
		if(enabled){
			vector<string> emails = {email};
			ofLogError("ofxAppErrorReporter") << "Send Error Report '" << alertName<< "' : '" << msg << "' level " << level02 ;
			sensu.send(alertName, msg, ofxSensu::Status(level02), emails, filePaths, true);
		}else{
			ofLogError("ofxAppErrorReporter") << "Skipping Send Error Report '" << alertName<< "' : '" << msg << "' bc Err Reports are disabled";
		}
	}
	

protected:

	string email;
	ofxSensu sensu;
	bool enabled;
};

