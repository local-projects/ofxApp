#include "ofApp.h"

void ofApp::setup(){

	ofxApp::get().setup(this); //start the ofxApp setup process
}

void ofApp::ofxAppPhaseWillBegin(ofxApp::Phase s){
	ofLogNotice("ofApp") << "Start User Process " << ofxApp::toString(s);
	phaseStartTime = ofGetElapsedTimef();
	switch (s) {
		case ofxApp::Phase::WILL_LOAD_CONTENT: break;
		case ofxApp::Phase::DID_DELIVER_CONTENT: break;
		case ofxApp::Phase::WILL_BEGIN_RUNNING: break;
	}
};


bool ofApp::ofxAppIsPhaseComplete(ofxApp::Phase){
	if(phaseStartTime + FAKE_LOAD_SCREEN_DURATION < ofGetElapsedTimef()){
		return true;
	}
	return false;
}


void ofApp::ofxAppDrawPhaseProgress(ofxApp::Phase, const ofRectangle & r){
};


string ofApp::ofxAppGetStatusString(ofxApp::Phase p){
	string s;
	switch(p){
		case ofxApp::Phase::WILL_LOAD_CONTENT: s = "doing stuff"; break;
		case ofxApp::Phase::DID_DELIVER_CONTENT: s = "doing more stuff"; break;
		case ofxApp::Phase::WILL_BEGIN_RUNNING: s = "preparing app launch";break;
	}
	return s;
}


float ofApp::ofxAppGetProgressForPhase(ofxApp::Phase){
	float v = (ofGetElapsedTimef() - phaseStartTime) / FAKE_LOAD_SCREEN_DURATION;
	return ofClamp(v, 0, 1);
}


void ofApp::update(){
	float dt = 1./60.;
}


void ofApp::draw(){

	if(ofxApp::get().getState() == ofxApp::State::RUNNING){

		ofxApp::get().textures().drawAll(ofRectangle(100, 100, ofGetMouseX() - 100, ofGetMouseY() - 100));

		G_TEX("emoji/413")->draw(0,0);
		G_FONT("NoManSky")->draw("My Font", 20, 40, ofGetHeight() - 40);
	}
}


void ofApp::keyPressed(int key){

	if(key == 'k'){
		ofxApp::utils::terminateApp("ofApp", "user forced terminate!");
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
