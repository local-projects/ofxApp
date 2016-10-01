#include "ofApp.h"

void ofApp::setup(){

	// other modules LISTENERS
	ofAddListener(app.screenSetup.setupChanged, this, &ofApp::setupChanged);

	app.setup(this); //start the ofxApp setup process
}


void ofApp::startUserProcess(ofxApp::State s){
	ofLogNotice("ofApp") << "start User Process " << ofxApp::toString(s);
};


void ofApp::update(){
	float dt = 1./60.;
}


void ofApp::draw(){

	if(app.getState() == ofxApp::RUNNING){

		float mb = app.textures().getTotalMemUsed();
		ofDrawBitmapStringHighlight("Static Assets Mem Used: " + ofToString(mb, 1) + "Mb", ofVec2f(20,20));

		app.textures().drawAll(ofRectangle(100, 100, ofGetMouseX(), ofGetMouseY()));

		app.textures().getTexture("sf2")->draw(0,0);

	}
}


void ofApp::keyPressed(int key){

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
