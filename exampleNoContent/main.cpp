#include "ofMain.h"
#include "ofApp.h"
#include "ofxTimeMeasurements.h"
#include "ofxApp.h"

//========================================================================
int main( ){

	ofxApp::get().loadSettings(); //ofxApp settings loaded here!

	//ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);

	ofGLFWWindowSettings winSettings;
	winSettings.numSamples = ofxApp::get().getInt("App/numAASamples", 8);
	winSettings.width = 1027;
	winSettings.height = 768;
	winSettings.glVersionMajor = 3;
	winSettings.glVersionMinor = 2;

	shared_ptr<ofAppBaseWindow> win = ofCreateWindow(winSettings);	// sets up the opengl context!
	((ofAppGLFWWindow*)win.get())->setMultiDisplayFullscreen(true);

	ofRunApp(win, shared_ptr<ofBaseApp>(new ofApp()));
	ofRunMainLoop();
}
