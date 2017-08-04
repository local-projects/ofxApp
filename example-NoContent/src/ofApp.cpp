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

string ofApp::ofxAppGetLogString(ofxApp::Phase phase){

	float progress = ofxAppGetProgressForPhase(phase);
	int numLines = 50 * progress;
	string log;
	for(int i = 0; i < numLines; i++){
		log += "this is a log line - " + ofToString(i) + "\n";
	}
	return log;
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

		//see a list of all static textures loaded from the "StaticAssets/textures" section in ofxAppSettings.json
		TS_START("drawImages"); //we are time profiling this section
		ofxApp::get().textures().drawAll(ofRectangle(100, 100, ofGetMouseX() - 100, ofGetMouseY() - 100));
		TS_STOP("drawImages");

		//get a texture from the "StaticAssets/textures" section
		G_TEX("emoji/413")->draw(0,0);

		//use ofxFontStash to draw text
		float y = ofGetHeight() -  120;
		float x = 100;

		G_FONT("pacifico")->draw("Pacifico font test woth ofxFontStash.", 40, x, y);
		y += 30;

		//use ofxFontStash2 to draw text - note font styles are defined in the ofxAppSettings.json file
		G_FS2.draw("MyHeaderStyle font test with ofxFontStash2", G_FSTYLE("MyStyle1"), x, y);
		y += 30;

		G_FS2.draw("MyBodyStyle font test with ofxFontStash2", G_FSTYLE("MyStyle2"), x, y);
		y += 30;

		//you can also draw formated paragraphs combining styles
		string styledText = "<style id='MyStyle1'>And you can</style><style id='MyStyle2'>MIX</style><style id='MyStyle1'>styles!</style>";
		G_FS2.drawFormatted(styledText, x, y);

	}
}


void ofApp::keyPressed(int key){

	if(key == 'k'){
		OFXAPP_REPORT("myAlertID","something went wrong", 2);
		OFXAPP_REPORT_FILE("myAlertID","something went wrong", 2, "path/to/my/File");
		ofxApp::utils::terminateApp("ofApp", "user forced terminate!");
	}

	if(key == 'e'){ //get ofxApp into Error State
		ofxApp::get().enterErrorState("Uh Oh!", "Some error occurred!\nContact oriol@localprojects.com\nPress 'E' to continue.");
	}

	if(key == 'E'){ //recover from Error State
		ofxApp::get().exitErrorState();
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
