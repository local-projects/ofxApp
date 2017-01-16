#include "ofApp.h"

void ofApp::setup(){

	app.setup(this); //start the ofxApp setup process
}


void ofApp::ofxAppPhaseWillBegin(ofxApp::Phase s){
	ofLogNotice("ofApp") << "Start User Process " << ofxApp::toString(s);
	switch (s) {
		case ofxApp::Phase::SETUP_B4_CONTENT_LOAD: break;
		case ofxApp::Phase::RECEIVE_CONTENT: break;
		case ofxApp::Phase::SETUP_AFTER_CONTENT_LOAD: break;
		case ofxApp::Phase::LAST_SETUP_B4_RUNNING: break;
	}
};


void ofApp::update(){
	float dt = 1./60.;
}


void ofApp::draw(){

	if(app.getState() == ofxApp::State::RUNNING){

		app.textures().drawAll(ofRectangle(100, 100, ofGetMouseX(), ofGetMouseY()));

		G_TEX("sf2")->draw(0,0);
		G_FONT("NoManSky")->draw("My Font", 20, ofGetMouseX(), ofGetMouseY());
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
