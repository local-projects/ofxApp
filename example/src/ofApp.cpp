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


string ofApp::ofxAppWillFetchContentFromURL(const std::string & contentID, const std::string & jsonURL){
	//This is your chance to change up the JSON URL from which content will be downloaded from
	//ofxApp provides you with the URL that you defined in ofxAppSettings.json for that contentID,
	//This can be used for example to add a timestamp to the query URL (ie url = url+"&time=4324345");
	//don't forget to return the url updated to ofxApp.
	string modifiedURL = jsonURL;
	//if(ofIsStringInString(jsonURL, "http://")) modifiedURL = modifiedURL + "&timeStamp=" + ofToString(ofGetSystemTimeMillis());
	return modifiedURL;
}


void ofApp::ofxAppPhaseWillBegin(ofxApp::Phase s){
	phaseStartTime = ofGetElapsedTimef();
	ofLogNotice("ofApp") << "Start User Process " << ofxApp::toString(s);
	switch (s) {
		case ofxApp::Phase::WILL_LOAD_CONTENT: break;
		case ofxApp::Phase::DID_DELIVER_CONTENT: break;
		case ofxApp::Phase::WILL_BEGIN_RUNNING:
			setupScene();
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

		//here we demsontrate how to find out if the content is different from the last launch or not.
		string freshJsonSha1, oldJsonSha1;
		bool jsonChanged = ofxApp::get().isJsonContentDifferentFromLastLaunch("CWRU", freshJsonSha1, oldJsonSha1);
	}

	if(contentID == "CH"){
		for(auto o : objs){
			CH_Object * cho = dynamic_cast<CH_Object*>(o);
			chObjects.push_back(cho); //cast up to CWRU_Object*
			//demo on how to "fish back" asset info
			vector<ofxAssets::Descriptor> assetKeys = cho->getAssetDescsWithTag("isPrimary");
//			if(assetKeys.size() == 1){
//				ofLogNotice("ofApp") << assetKeys.back().relativePath << " is Primary for " << cho->objectID;
//			}
		}
	}
}


void ofApp::ofxAppContentUpdate(const std::string & contentID, vector<ContentObject*>objs){

	ofLogNotice("ofApp") << "got a Live Content Update for \"" << contentID << "\" with " << objs.size() << " objects.";

	//delete all old objects clear the vector. Note we dont directly delete them as they are
	//TextureObjects, we use deleteWithGC() [GC == GarbageCollector] which will delete it safely
	//when no textures are being loaded
	if(contentID == "CH"){
		for(auto obj: chObjects){
			obj->deleteWithGC();
		}
		chObjects.clear();
		ofxAppContentIsReady(contentID, objs);
	}

	//idem as for "CH", remove all content
	if(contentID == "CWRU"){
		for(auto obj: cwruObjects){
			obj->deleteWithGC();
		}
		cwruObjects.clear();
		ofxAppContentIsReady(contentID, objs);
	}
	deleteScrollViews();
	setupScrollViews();
}


void ofApp::ofxAppContentUpdateFailed(const std::string & contentID, const std::string & errorMsg){
	ofLogError("ofApp") << "Live Content Update for \"" << contentID << "\" failed with this error msg: \"" << errorMsg << "\".";
};


void ofApp::update(){

	float dt = 1./60.;

	//upate TextureObject:: for all our objects (to handle load & unload textures)
	float time = ofGetElapsedTimef();
	TS_START("update CH objects");
	for(auto chObj : chObjects){
		chObj->TexturedObject::update(time);
	}
	TS_STOP("update CH objects");

	TS_START("update CWRU objects");
	for(auto cwruO : cwruObjects){
		cwruO->TexturedObject::update(time);
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
				"\nNum Images: 1" +
				"\nImg Size: " + ofToString(cwruO->getTextureDimensions(TEXTURE_ORIGINAL,0));
			}

			G_FONT_MONO_BOLD().drawMultiLine(info, 16, 20, 682);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////

void ofApp::setupScene(){
	//setup ofxInterface
	scene = new ofxInterface::Node();
	scene->setPosition(ofVec3f());
	scene->setSize(ofGetWidth(), ofGetHeight());
	scene->setName("scene");
	ofxInterface::TouchManager::one().setup(scene, false);
}


void ofApp::setupScrollViews(){

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
	ofRectangle viewArea = ofRectangle(paddingH, paddingH + 25, ofGetWidth() - 2 * paddingH, 610);

	scrollView->setup(viewArea, config);
	scene->addChild(scrollView);

	//register for click notification
	ofAddListener(scrollView->eventTextureClicked, this, &ofApp::onSrollImageClicked);

	//register for draw custom notifications
	ofAddListener(scrollView->eventTileDraw, this, &ofApp::onDrawTile);

	//prepare the list of objects you want to show on the scrollview
	vector<TexturedObjectScrollView::TexturedObjectTexture> imagesToShow;

	//add CH content - every image inside every object
	for(auto chObj : chObjects){
		TexturedObjectScrollView::TexturedObjectTexture tex;
		tex.texObj = chObj;
		int c = 0;
		for(auto img : chObj->images){
			tex.texIndex = c;
			imagesToShow.push_back(tex);
			c++;
		}
	}

	//add CWRU content
	for(auto cwruObj : cwruObjects){
		TexturedObjectScrollView::TexturedObjectTexture tex;
		tex.texObj = cwruObj;
		tex.texIndex = 0;
		imagesToShow.push_back(tex);
	}

	//shuffle images
	//std::random_shuffle(imagesToShow.begin(), imagesToShow.end());

	//finally load the contents
	scrollView->loadContent(imagesToShow);
	ofLogNotice("ofApp") << "showing " << imagesToShow.size() << " images.";
}


void ofApp::deleteScrollViews(){
	//remove listeners to scrollview
	ofRemoveListener(scrollView->eventTextureClicked, this, &ofApp::onSrollImageClicked);
	ofRemoveListener(scrollView->eventTileDraw, this, &ofApp::onDrawTile);
	//remove scrollview from scene, delete it too to avoid leaking.
	delete scene->removeChild(scrollView);
}


void ofApp::onSrollImageClicked(TexturedObjectScrollView::TouchedImage & ti){

	int index = ti.objTex.texIndex;
	TexturedObject * to = dynamic_cast<TexturedObject *>(ti.objTex.texObj);

	ofLogNotice("ofApp") << "Clicked Image #" << ti.layoutID << " (" <<
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
	G_FONT_MONO().draw(co->getObjectUUID(), 16, d.area.x + 3, d.area.y + d.area.height - 5);

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
	if (key == '1' || key == '2' || key == '3') {
		OFXAPP_REPORT("testAlert", "testing", key - '1' /* [0..2]*/);
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

