#include "ofApp.h"
#include "ofxAppParsers.h"

void ofApp::setup(){

	//create my custom lambdas for parsing / preparing objects
	ofxAppParsers myParsers = ofxAppParsers();

	map<string, ofxApp::ParseFunctions> objectParsers;
	//put them in a map - named as the "ofxAppSettings.json" section ("Content/JsonSources")
	objectParsers["CWRU"] = myParsers.cwru;
	objectParsers["CH"] = myParsers.ch;

	//start the ofxApp setup process
	ofxApp::get().setup(objectParsers, this);
}


void ofApp::ofxAppPhaseWillBegin(ofxApp::Phase s){
	phaseStartTime = ofGetElapsedTimef();
	ofLogNotice("ofApp") << "Start User Process " << ofxApp::toString(s);
	switch (s) {
		case ofxApp::Phase::WILL_LOAD_CONTENT: break;
		case ofxApp::Phase::DID_DELIVER_CONTENT: break;
		case ofxApp::Phase::WILL_BEGIN_RUNNING:
			setupScrollViews();
			break;
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

	//upate TextureObject:: for all our objects (to handle load & unload textures)
	TS_START("update CH objects");
	for(auto chObj : chObjects){
		chObj->TexturedObject::update();
	}
	TS_STOP("update CH objects");

	TS_START("update CWRU objects");
	for(auto cwruO : cwruObjects){
		cwruO->TexturedObject::update();
	}
	TS_STOP("update CWRU objects");

	//update ofxInterface
	if(scene){
		TS_START("update scene");
		scene->updateSubtree(dt);
		TS_STOP("update scene");
	}
}


void ofApp::draw(){

	if(ofxApp::get().getState() == ofxApp::State::RUNNING){

		TS_START("Scene Node D");
		scene->render();
		if(GLOB.debug){
			scene->renderDebug(true);
		}
		TS_STOP("Scene Node D");

		if(selectedObject){
			CH_Object * chO = dynamic_cast<CH_Object*>(selectedObject);
			CWRU_Object * cwruO = dynamic_cast<CWRU_Object*>(selectedObject);
			string info;
			if(chO){
				info = "ObjectID: " + chO->objectID +
				"\nTitle: " + chO->title +
				"\nDescription: " + chO->description +
				"\nNum Images: " + ofToString(chO->images.size()) +
				"\nImg Size: " + ofToString(chO->getTextureDimensions(TEXTURE_ORIGINAL,0));

			}
			if(cwruO){
				info = "UUID: " + cwruO->getObjectUUID() +
				"\nTitle: " + cwruO->title +
				"\nDescription: " + cwruO->description +
				"\nNum Images: " + cwruO->imagePath +
				"\nImg Size: " + ofToString(cwruO->getTextureDimensions(TEXTURE_ORIGINAL,0));
			}

			G_FONT_MONO_BOLD.drawMultiLine(info, 16, 20, 682);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////


void ofApp::setupScrollViews(){

	//setup ofxInterface
	scene = new ofxInterface::Node();
	scene->setPosition(ofVec3f());
	scene->setSize(ofGetWidth(), ofGetHeight());
	scene->setName("scene");
	ofxInterface::TouchManager::one().setup(scene, false);

	float padding = 4;
	float paddingH = 20;

	//scrollview config
	TexturedObjectScrollView::ScrollViewConfig config;
	config.numRows = 3;
	config.drawLoadingRects = true;
	config.customTileDraw = true; //calls onDrawTile() for each on-sreen tile
	config.imgFadeInDuration = 0.1;
	config.padding = padding;
	config.sizeToLoad = TEXTURE_ORIGINAL;
	config.mipmaps = true;
	config.maxScrollHandleW = 200;
	config.scrollFriction = 0.99;
	config.snapToImage = false;
	config.marginPadding = padding;
	config.bgColor = ofColor(22);
	config.scrollHandleColor = ofColor(128);
	config.scrollHandleVertPostion = 4;
	config.scrollviewEdgeGradientWidth = 30;
	config.numFboSamples = 4;
	config.canvasLookAheadDist = ofGetWidth() ; // load offscreen images that are 200 far from screen
	config.unloadDelay = 5.0; //only unload images once they have been N seconds offscreen

	scrollView = new TexturedObjectScrollView();
	scrollView->setName("scrollView");
	ofRectangle viewArea = ofRectangle(paddingH, paddingH + 25, ofGetWidth() - 2 * paddingH, 600);

	scrollView->setup(viewArea, config);
	scene->addChild(scrollView);

	//register for click notification
	ofAddListener(scrollView->eventTextureClicked, this, &ofApp::onSrollImageClicked);

	//register for draw custom notifications
	ofAddListener(scrollView->eventTileDraw, this, &ofApp::onDrawTile);

	//prepare the list of objects you want to show on the scrollview
	vector<TexturedObjectScrollView::TexturedObjectTexture> imagesToShow;

	//add CH content
	for(auto chObj : chObjects){
		TexturedObjectScrollView::TexturedObjectTexture tex;
		tex.texObj = chObj;
		tex.texIndex = 0;
		imagesToShow.push_back(tex);
	}

	//add cwru content
	for(auto cwruObj : cwruObjects){
		TexturedObjectScrollView::TexturedObjectTexture tex;
		tex.texObj = cwruObj;
		tex.texIndex = 0;
		imagesToShow.push_back(tex);
	}

	//finally load the contents
	scrollView->loadContent(imagesToShow);

}

void ofApp::onSrollImageClicked(TexturedObjectScrollView::TouchedImage & ti){

	int index = ti.objTex.texIndex;
	TexturedObject * to = dynamic_cast<TexturedObject *>(ti.objTex.texObj);

	ofLogNotice() << "Clicked Image #" << ti.layoutID << " (" <<
	to->getLocalTexturePath(TEXTURE_ORIGINAL, index) << ") on ScrollView " << ti.who->getName();

	ti.who->forceRedraw();

	selectedObject = to;

	ContentObject * co = dynamic_cast<ContentObject*>(ti.objTex.texObj);
	ofLogNotice("ofxApp") << "clicked on object " << co->getObjectUUID();
	
}

void ofApp::onDrawTile(TexturedObjectScrollView::DrawTileInfo & d){

	TexturedObject * to = d.objTex.texObj;
	ContentObject * co = dynamic_cast<ContentObject*>(d.objTex.texObj);

	ofSetColor(0,0,0, 200);
	int h = 20;
	ofDrawRectangle(d.area.x, d.area.y + d.area.height - h, d.area.width, h);
	ofSetColor(255);
	G_FONT_MONO.draw(co->getObjectUUID(), 16, d.area.x + 3, d.area.y + d.area.height - 5);

	if(to == selectedObject){
		ofNoFill();
		ofSetLineWidth(3);
		ofSetColor(255, 0, 0, 255);
		ofDrawRectangle(d.area);
		ofSetLineWidth(1);
		ofFill();
		ofSetColor(255);
	}
};

/////////////////////////////////////////////////////////////////////////////////////


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
	ofxInterface::TouchManager::one().touchMove(0, ofVec2f(x,y));
}


void ofApp::mousePressed(int x, int y, int button){
	ofxInterface::TouchManager::one().touchDown(0, ofVec2f(x,y));
}


void ofApp::mouseReleased(int x, int y, int button){
	ofxInterface::TouchManager::one().touchUp(0, ofVec2f(x,y));
}


void ofApp::windowResized(int w, int h){

}


void ofApp::gotMessage(ofMessage msg){

}


void ofApp::dragEvent(ofDragInfo dragInfo){
	
}

