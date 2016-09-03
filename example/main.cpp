#include "ofMain.h"
#include "ofApp.h"
#include "ofxTimeMeasurements.h"
#include "ofxApp.h"

//========================================================================
int main( ){

	app.loadSettings(); //ofxApp settings loaded here!

	//ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);

	ofGLFWWindowSettings winSettings;
	winSettings.numSamples = app.getInt("App/numAASamples", 8);
	winSettings.width = 1027;
	winSettings.height = 768;


	shared_ptr<ofAppBaseWindow> win = ofCreateWindow(winSettings);	// sets up the opengl context!
	((ofAppGLFWWindow*)win.get())->setMultiDisplayFullscreen(true);

	ofRunApp(win, shared_ptr<ofBaseApp>(new ofApp()));
	ofRunMainLoop();
}
