#include "ofMain.h"
#include "ofApp.h"
#include "ofxTimeMeasurements.h"
#include "ofxApp.h"

//========================================================================
int main( ){

	app.loadSettings(); //ofxApp settings loaded early here.
						//usually they are loaded automatically when you call app.setup();
						//but in this case we need to read some settings b4 the app launches
						//and the openGL context is created (AA samples)

	//ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);

	ofGLFWWindowSettings winSettings;
	winSettings.numSamples = app.getInt("App/numAASamples", 8);
	winSettings.width = 1024;
	winSettings.height = 768;

	shared_ptr<ofAppBaseWindow> win = ofCreateWindow(winSettings);	// sets up the opengl context!

	ofRunApp(win, shared_ptr<ofBaseApp>(new ofApp()));
	ofRunMainLoop();
}
