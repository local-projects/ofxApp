//
//  ofxAppStaticTextures.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#include "ofxAppStaticTextures.h"


string ofxAppStaticTextures::filenameHintTex2D = "_t2d";
string ofxAppStaticTextures::filenameHintMipMap = "_mip";


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

void ofxAppStaticTextures::loadTexturesInDir(const string& imgDirPath, int maxThreads){
	if(!isLoading){
		this->maxThreads = maxThreads;
		ofLogWarning("ofxAppStaticTextures") << "#### START Loading all Textures in directory \"" << imgDirPath << "\" ############################################";
		isLoading = true;
		dirPath = ofFilePath::addTrailingSlash(imgDirPath);
		#ifdef TARGET_WIN32 //lets make windows path prettier
		ofStringReplace(dirPath, "\\", "/");
		#endif
		loadTexturesInDirectory(imgDirPath, true);
	}else{
		ofLogError("ofxAppStaticTextures") << "Already loading async!";
	}
}


void ofxAppStaticTextures::loadTexturesInDirectory(const string& path, bool recursive){

	ofDirectory dir(path);
	dir.listDir();

	if (dir.size() == 0) { //if no images, proceed now.
		ofNotifyEvent(eventAllTexturesLoaded, this);
		ofLogWarning("ofxAppStaticTextures") << "No textures found in the directory! \"" << path << "\"";
		return;
	}

	for(int i = 0; i < dir.size(); i++){
		ofFile file = dir.getFile(i);

		if (file.isDirectory() && recursive) {
			loadTexturesInDirectory(path + "/" + file.getFileName(), recursive);
			continue;
		}
		string ext = ofToLower(file.getExtension());
		if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif" || ext == "tga" || ext == "tiff" || ext == "tif" || ext == "psd") {
			string filepath = path + "/" + file.getBaseName() + "." + file.getExtension();
			
			PreLoadData texData;
			texData.filePath = filepath;
			texData.loader = new ThreadedLoader();
			texData.tex = createTexObjForPath(filepath, texData.texName, texData.createMipmap );
			textures[texData.texName] = texData.tex;
			texNameOrder.push_back(texData.texName);
			pendingToPreLoad.push_back(texData);
		}
	}
}


ofxAutoTexture* ofxAppStaticTextures::loadTexture(PreLoadData data){
	
	bool loaded = data.tex->loadFromFile(data.filePath);

	if(loaded){
		float memUsedForThisOne = memUse(data.tex);
		memUsed += memUsedForThisOne;
		if(data.createMipmap){
			data.tex->generateMipmap();
			data.tex->setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		}

		ofLogNotice("ofxAppStaticTextures") 	<< "#### Loaded \"" << data.filePath << "\" ######################################################";
		ofLogNotice("ofxAppStaticTextures")	<< "     Name:\"" << data.texName << "\"  " << "[" << data.tex->getWidth() << "x" << data.tex->getHeight() << "]" <<
		"  Mipmap:" << data.createMipmap << "  Format:" << ofGetGlInternalFormatName(data.tex->getTextureData().glInternalFormat) << "  Mem:" << ofToString(memUsedForThisOne, 2) << "Mb";
		return data.tex;
	}else{
		delete data.tex;
		ofLogError("ofxAppStaticTextures") << "FAILED to load tex from \"" << data.filePath << "\"" ;
		return NULL;
	}
}

ofxAutoTexture* ofxAppStaticTextures::createTexObjForPath(string filePath, string & texName, bool & createMipMap ){

	string lowercaseFilePath = ofToLower(filePath);
	bool useTex2D = ofIsStringInString(lowercaseFilePath, filenameHintTex2D);
	createMipMap = ofIsStringInString(lowercaseFilePath, filenameHintMipMap);

	//set OF ARB global state according to file naming
	if(createMipMap){
		ofDisableArbTex();
	}else{
		if (!useTex2D){
			ofEnableArbTex();
		}else{
			ofDisableArbTex();
		}
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

	if(numPreloaded < pendingToPreLoad.size() && numPreloading < maxThreads ){
		
		int numToStart = MIN( maxThreads - numPreloading, pendingToPreLoad.size() - numPreloaded );
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
	
	if(numPreloaded > 0 ){
		for(int i = pendingToPreLoad.size()-1; i >= 0; i--){
			if(pendingToPreLoad[i].loader->preloaded){
				PreLoadData currTex = pendingToPreLoad[i];
				pendingToPreLoad.erase(pendingToPreLoad.begin() + i);
				loadTexture(currTex);
				loaded.push_back(currTex);
				if(pendingToPreLoad.size() == 0){
					ofLogNotice("ofxAppStaticTextures") << "#### DONE loading " << textures.size() << " Static Textures! Memory used: " << ofToString(memUsed,2) << "Mb ############################################";
					ofNotifyEvent(eventAllTexturesLoaded, this);
				}
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


ofTexture* ofxAppStaticTextures::getTexture(string fullPath){

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

	int n = getNumTextures();
	float ar = rect.width / rect.height;
	float nx = sqrtf(n / ar) * ar;// * ar);
	float ny = sqrtf(n * ar) / ar;// * ((1 + ar) * 0.5));

	int nn = ceil(nx) * ceil(ny);
	if( nn > n){
		if(nn - n > nx){
			ny-=1;
		}
	}

	float xx = rect.x;
	float yy = rect.y;

	ofSetColor(0);
	ofDrawRectangle(rect);
	ofSetColor(255);

	for(auto & texName : texNameOrder){
		ofTexture * tex = textures[texName];
		ofRectangle frame = ofRectangle(xx,yy, rect.width / ceil(nx), rect.height / ceil(ny));
		float pad = 0.05 * MAX(frame.width, frame.height);
		ofRectangle paddedFrame = frame;
		paddedFrame.x += pad;
		paddedFrame.y += pad;
		paddedFrame.width -= 2 * pad;
		paddedFrame.height -= 2 * pad;

		ofSetColor(32);
		ofDrawRectangle(frame);
		ofSetColor(16);
		ofNoFill();
		ofDrawRectangle(frame);
		ofFill();

		if(tex->isAllocated() && tex->getWidth()){
			ofRectangle texR = ofRectangle(0,0,tex->getWidth(), tex->getHeight());
			texR.scaleTo(paddedFrame, OF_ASPECT_RATIO_KEEP, OF_ALIGN_HORZ_CENTER, OF_ALIGN_VERT_TOP);
			ofSetColor(255);
			tex->draw(texR);
		}
		ofDrawBitmapStringHighlight(texName, xx + 5, yy + frame.height - 8);
		xx += ceil(frame.width);
		if(xx >= rect.x + rect.width){
			yy += frame.height;
			xx = rect.x;
		}
	}
}

