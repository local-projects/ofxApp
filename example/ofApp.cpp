#include "ofApp.h"
#include "ofxAppLambdas.h"

void ofApp::setup(){

	// other modules LISTENERS
	ofAddListener(app.screenSetup.setupChanged, this, &ofApp::setupChanged);

	//create my custom lambdas for parsing / preparing objects
	ofxAppLambdas myLambdas = ofxAppLambdas();

	map<string, ofxApp::UserLambdas> userLambdas;
	//put them in a map - named as the "AppSettings.json" section ("content/JsonSources")
	userLambdas["CWRU"] = myLambdas.cwru;
	userLambdas["CH"] = myLambdas.ch;

	//start the app setup process
	app.setup(userLambdas, this);
}


void ofApp::ofxAppStartUserPhase(ofxApp::UserAppSetupStage s){
	ofLogNotice("ofApp") << "Start User Process " << ofxApp::toString(s);
	switch (s) {
		case ofxApp::SETUP_B4_CONTENT_LOAD: break;
		case ofxApp::RECEIVE_CONTENT_LOAD_RESULTS: break;
		case ofxApp::SETUP_AFTER_CONTENT_LOAD: break;
		case ofxApp::SETUP_JUST_B4_RUNNING: break;
	}
};


void ofApp::ofxAppContentIsReady(const string & contentID, vector<ContentObject*> objs){
	ofLogNotice("ofApp") << "Content '" << contentID << "' is ready! " << objs.size() << " objects!";

	if(contentID == "CWRU"){
		for(auto o : objs){
			cwruObjects.push_back(dynamic_cast<CWRU_Object*>(o)); //cast up to CWRU_Object*
		}
	}

	if(contentID == "CH"){
		for(auto o : objs){
			CH_Object * cho = dynamic_cast<CH_Object*>(o);
			chObjects.push_back(cho); //cast up to CWRU_Object*
			//demo on how to "fish back" asset info
			vector<ofxAssets::Descriptor> assetKeys = cho->getAssetDescsWithTag("isPrimary");
			if(assetKeys.size() == 1){
				ofLogNotice("ofApp") << assetKeys.back().relativePath << " is Primary for " << cho->objectID;
			}
		}
	}
}


void ofApp::update(){
	float dt = 1./60.;
}


void ofApp::draw(){

	if(app.getState() == ofxApp::RUNNING){

		app.textures().drawAll(ofRectangle(100, 100, ofGetMouseX(), ofGetMouseY()));		
		G_TEX("sf2")->draw(0,0);
		G_FONT("NoManSky")->draw("My Font", 20, ofGetMouseX(), ofGetMouseY());

	}
}


void ofApp::keyPressed(int key){
	if (key == 'a') {
		OFXAPP_REPORT("testAlert", "testing", 0);
	}
}


void ofApp::keyReleased(int key){

}


void ofApp::mouseMoved(int x, int y ){

}


void ofApp::mouseDragged(int x, int y, int button){

}


void ofApp::mousePressed(int x, int y, int button){

}


void ofApp::mouseReleased(int x, int y, int button){

}


void ofApp::windowResized(int w, int h){

}


void ofApp::gotMessage(ofMessage msg){

}


void ofApp::dragEvent(ofDragInfo dragInfo){
	
}

void ofApp::setupChanged(ofxScreenSetup::ScreenSetupArg &arg){
	ofLogNotice()	<< "ofxScreenSetup setup changed from " << arg.who->stringForMode(arg.oldMode)
	<< " (" << arg.oldWidth << "x" << arg.oldHeight << ") "
	<< " to " << arg.who->stringForMode(arg.newMode)
	<< " (" << arg.newWidth << "x" << arg.newHeight << ")";
}
