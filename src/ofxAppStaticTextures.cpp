//
//  ofxAppStaticTextures.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#include "ofxAppStaticTextures.h"
#include "ofxApp.h"
#include "ofxAppErrorReporter.h"
#include "ofxAutoTexture.h"

std::string ofxAppStaticTextures::filenameHintTex2D = "_t2d";
std::string ofxAppStaticTextures::filenameHintMipMap = "_mip";

void ofxAppStaticTextures::ThreadedLoader::threadedFunction() {

	#ifdef TARGET_WIN32
	#elif defined(TARGET_LINUX)
	pthread_setname_np(pthread_self(), "ofxAppStaticTextures");
	#else
	pthread_setname_np("ofxAppStaticTextures");
	#endif
	data.tex->preloadPixelsFromFile(data.filePath);
	preloaded = true;
}


ofxAppStaticTextures::ofxAppStaticTextures(){
}

void ofxAppStaticTextures::setup(){
	if(!missingTex.isAllocated()){
		missingTex.allocate(16,16,GL_RGBA);
		ofAddListener(ofEvents().update, this, &ofxAppStaticTextures::onUpdate);
	}else{
		ofLogError("ofxAppStaticTextures") << "Already setup! Trying to setup twice?!";
	}
}

void ofxAppStaticTextures::loadTexturesInDir(const std::string& imgDirPath, int maxThreads){
	if(!isLoading){
		//TS_START_NIF("load Textures");
		startLoadTime = ofGetElapsedTimef();
		this->maxThreads = maxThreads;
		string msg = "START Loading all Textures in directory \"" + imgDirPath + "\" across " + ofToString(maxThreads);
		ofLogNotice("ofxAppStaticTextures") << ofxApp::utils::getAsciiHeader(msg, '#', 4, 120);
		isLoading = true;
		dirPath = ofFilePath::addTrailingSlash(imgDirPath);
		#ifdef TARGET_WIN32 //lets make windows path prettier
		ofStringReplace(dirPath, "\\", "/");
		#endif

		ofDirectory dir(imgDirPath);
		dir.listDir();
		if (dir.size() == 0) { //if no images, proceed now.
			ofNotifyEvent(eventAllTexturesLoaded, this);
			ofLogWarning("ofxAppStaticTextures") << "No textures found in the directory! \"" << imgDirPath << "\"";
			return;
		}

		loadTexturesInDirectory(imgDirPath, true);
	}else{
		ofLogError("ofxAppStaticTextures") << "Already loading async!";
	}
}


void ofxAppStaticTextures::loadTexturesInDirectory(const std::string& path, bool recursive){

	ofDirectory dir(path);
	dir.listDir();


	for(int i = 0; i < dir.size(); i++){
		ofFile file = dir.getFile(i);

		if (file.isDirectory() && recursive) {
			loadTexturesInDirectory(path + "/" + file.getFileName(), recursive);
			continue;
		}
		std::string ext = ofToLower(file.getExtension());
		if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif" || ext == "tga" || ext == "tiff" || ext == "tif" || ext == "psd") {
			std::string filepath = path + "/" + file.getBaseName() + "." + file.getExtension();
			
			PreLoadData texData;
			texData.tex = createTexObjForPath(filepath, texData.texName, texData.createMipmap, texData.useTex2D);
			if(texData.tex){
				texData.filePath = filepath;
				texData.loader = new ThreadedLoader();

				textures[texData.texName] = texData.tex;
				texNameOrder.push_back(texData.texName);
				pendingToPreLoad.push_back(texData);
			}else{
				ofLogError("ofxAppStaticTextures") << "Skipping image at \"" << filepath << "\" as its name collides with another one";
			}
		}
	}
}


ofxAutoTexture* ofxAppStaticTextures::loadTexture(PreLoadData data){
	
	//set OF ARB global state according to file naming
	if (data.createMipmap) {
		ofDisableArbTex();
	} else {
		if (!data.useTex2D) {
			ofEnableArbTex();
		} else {
			ofDisableArbTex();
		}
	}

	ofSetLogLevel("ofxAutoTexture", OF_LOG_SILENT);
	bool loaded = data.tex->loadFromFile(data.filePath);
	ofSetLogLevel("ofxAutoTexture", OF_LOG_NOTICE);

	if(loaded){
		float memUsedForThisOne = memUse(data.tex);
		memUsed += memUsedForThisOne;
		if(data.createMipmap){
			data.tex->generateMipmap();
			data.tex->setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		}

		std::string memUsedStr = ofxApp::utils::bytesToHumanReadable(memUsedForThisOne * 1024 * 1024, 2);

		ofLogNotice("ofxAppStaticTextures") << ofxApp::utils::getAsciiHeader("Loaded \"" + data.filePath +"\"", '#', 4, 120);
		ofLogNotice("ofxAppStaticTextures")	<< "     Name:\"" << data.texName << "\"  " << "[" << data.tex->getWidth() << "x" << data.tex->getHeight() << "]" <<
		"  Mipmap:" << data.createMipmap << "  Format:" << ofGetGlInternalFormatName(data.tex->getTextureData().glInternalFormat) << "  Mem: " << memUsedStr ;
		return data.tex;
	}else{
		delete data.tex;
		std::string msg = "FAILED to load tex from \"" + data.filePath + "\"" ;
		OFXAPP_REPORT("ofxAppStaticTexturesFailLoad", msg, 1);
		ofLogError("ofxAppStaticTextures") << msg;
		return NULL;
	}
}

ofxAutoTexture* ofxAppStaticTextures::createTexObjForPath(std::string filePath, std::string & texName, bool & createMipMap, bool & useTex2D) {

	std::string lowercaseFilePath = ofToLower(filePath);
	useTex2D = ofIsStringInString(lowercaseFilePath, filenameHintTex2D);
	createMipMap = ofIsStringInString(lowercaseFilePath, filenameHintMipMap);

	if(forceMipmapsOnAll){
		createMipMap = true;
	}

	texName = ofFilePath::removeExt(filePath);
	#ifdef TARGET_WIN32 //lets make windows path prettier
	ofStringReplace(texName, "\\", "/");
	#endif
	//remove hints from filename, we dont want to have to search with those in
	ofStringReplace(texName, filenameHintTex2D, "");
	ofStringReplace(texName, filenameHintMipMap, "");
	if(texName.size() > dirPath.size()){
		if(texName.substr(0, dirPath.size()) == dirPath){
			texName = texName.substr(dirPath.size(), texName.size() - dirPath.size());
		}
	}

	//see if img is treated for transparent pixels
	const string & transparencyOverrideCmd = ofxAutoTexture::paintTransparentPixelsCommand;
	bool transparencyOverride = false;
	auto it2 = texName.find(transparencyOverrideCmd);
	
	if(it2 != std::string::npos){ //found "_transp"
		transparencyOverride = true;
		size_t beginOfRemovalSpot = it2;
		texName = texName.substr(0,beginOfRemovalSpot);
	}
	
	auto it = textures.find(texName);
	if (it != textures.end()){
		ofLogError("ofxAppStaticTextures") << "file name collision! " << filePath << " >> " << texName;
		ofLogError("ofxAppStaticTextures") << "skipping texture at path: '" << filePath << "'";
		return NULL;
	}
		
	ofxAutoTexture * tex = new ofxAutoTexture();
	return tex;
}


void ofxAppStaticTextures::onUpdate(ofEventArgs & ){
	
	int numPreloading = 0;
	int numPreloaded = 0;
	for(auto & d : pendingToPreLoad){
		if(d.tex->isPreloadingPixels()) numPreloading++;
		if(d.loader->preloaded) numPreloaded++;
	}

	if(numPreloaded < pendingToPreLoad.size() && numPreloading < maxThreads){
		
		int numToStart = MIN( 1 * (maxThreads - numPreloading), pendingToPreLoad.size() - numPreloaded );
		int i = 0;
		while(numToStart > 0 && i < pendingToPreLoad.size()){
			if(!pendingToPreLoad[i].loader->threadStarted){
				pendingToPreLoad[i].loader->threadStarted = true;
				pendingToPreLoad[i].loader->data = pendingToPreLoad[i];
				numToStart--;
				//spawn a loading thread
				pendingToPreLoad[i].loader->startThread();
			}
			i++;
		}
	}

	if(numPreloaded > 0){
		int numLoadedThisFrame = 0;
		float t = ofGetElapsedTimef();
		for(int i = pendingToPreLoad.size()-1; i >= 0; i--){
			if(pendingToPreLoad[i].loader->preloaded){
				PreLoadData currTex = pendingToPreLoad[i];
				pendingToPreLoad.erase(pendingToPreLoad.begin() + i);
				loadTexture(currTex);
				loaded.push_back(currTex);
				if(pendingToPreLoad.size() == 0){
					ofLogNotice("ofxAppStaticTextures") << "#### DONE loading " << textures.size() << " Static Textures! Memory used: " << ofToString(memUsed,2) << "Mb ############################################";
					ofLogNotice("ofxAppStaticTextures") << "Loading StaticTextures took " << ofxApp::utils::secondsToHumanReadable(ofGetElapsedTimef() - startLoadTime, 2);
					ofNotifyEvent(eventAllTexturesLoaded, this);
					//TS_STOP_NIF("load Textures");
				}
			}
			if(ofGetElapsedTimef() - t > 0.05){ //dont hog a frame too long
				break;
			}
		}
	}
}


float ofxAppStaticTextures::memUse(ofTexture * tex){

	if(tex && tex->isAllocated()){
		int w, h;
		if(tex->texData.textureTarget == GL_TEXTURE_RECTANGLE_ARB){
			w = ofNextPow2(tex->getWidth());
			h = ofNextPow2(tex->getHeight());
		}else{
			w = tex->getWidth();
			h = tex->getHeight();
		}

		int numC = ofGetNumChannelsFromGLFormat(ofGetGLFormatFromInternal(tex->texData.glInternalFormat));
		float mem = w * h * numC;
		if(tex->hasMipmap()){
			mem *= 1.3333; //mipmaps take 33% more memory
		}
		return mem / float(1024 * 1024); //return MBytes
	}
	ofLogError("ofxAppStaticTextures") << "Tex doesn't use any memory bc its not allocated!";
	return 0;
}


ofTexture* ofxAppStaticTextures::getTexture(std::string fullPath){

	//remove "/" from beginnig as it makes no sense
	if(fullPath.size() && fullPath[0] == '/'){
		fullPath = fullPath.substr(1, fullPath.size() -1);
	}
	
	auto it = textures.find(fullPath);
	if (it == textures.end()){
		ofLogError("ofxAppStaticTextures") << "requesting a missing texture! " << fullPath;
		return &missingTex;
	}
	return it->second;
}


void ofxAppStaticTextures::drawAll(const ofRectangle & rect){

//	return;

	int n = getNumTextures();
	float ar = rect.width / rect.height;
	float nx = sqrtf(n / ar) * ar;
	float ny = sqrtf(n * ar) / ar;

	int nn = ceil(nx) * ceil(ny);
	if( nn > n){
		if(nn - n > nx){
			ny -= 1;
		}
	}

	float xx = rect.x;
	float yy = rect.y;

	ofSetColor(0);
	ofDrawRectangle(rect);
	ofSetColor(255);

	for(auto & texName : texNameOrder){
		ofxAutoTexture * tex = textures[texName];
		ofRectangle frame = ofRectangle(xx,yy, rect.width / ceil(nx), rect.height / ceil(ny));
		float pad = 0.1 * MAX(frame.width, frame.height);
		ofRectangle paddedFrame = frame;
		paddedFrame.x += pad;
		paddedFrame.y += pad;
		paddedFrame.width -= 2 * pad;
		paddedFrame.height -= 2 * pad;

		if(!tex->isPreloadingPixels() && tex->isAllocated() && tex->getWidth()){
			ofRectangle texR = ofRectangle(0,0,tex->getWidth(), tex->getHeight());
			texR.scaleTo(paddedFrame, OF_ASPECT_RATIO_KEEP, OF_ALIGN_HORZ_CENTER, OF_ALIGN_VERT_TOP);
			tex->draw(texR);
		}
		if(frame.width > 200){
			ofDrawBitmapStringHighlight(texName, xx + 5, yy + frame.height - 8);
		}
		xx += ceil(frame.width);
		if(xx >= rect.x + rect.width){
			yy += frame.height;
			xx = rect.x;
		}
	}
}

