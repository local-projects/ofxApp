//
//  AppStaticTextures.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#include "AppStaticTextures.h"


string AppStaticTextures::filenameHintTex2D = "_t2d";
string AppStaticTextures::filenameHintMipMap = "_mip";


AppStaticTextures::AppStaticTextures(){
}

void AppStaticTextures::setup(){
	if(!missingTex.isAllocated()){
		missingTex.allocate(16,16,GL_RGBA);
		ofAddListener(ofEvents().update, this, &AppStaticTextures::onUpdate);
	}else{
		ofLogError("AppStaticTextures") << "Already setup! Trying to setup twice?!";
	}
}

void AppStaticTextures::loadTexturesInDir(const string& imgDirPath, bool async){
	if(!isLoading){
		ofLogWarning("AppStaticTextures") << "#### START Loading all Textures in directory \"" << imgDirPath << "\" ############################################";
		isLoading = true;
		loadAsync = async;
		dirPath = ofFilePath::addTrailingSlash(imgDirPath);
		#ifdef TARGET_WIN32 //lets make windows path prettier
		ofStringReplace(dirPath, "\\", "/");
		#endif
		loadTexturesInDirectory(imgDirPath, true);
		if(async == false) isLoading = false;
	}else{
		ofLogError("AppStaticTextures") << "Already loading async!";
	}
}

void AppStaticTextures::loadTexturesInDirectory(const string& path, bool recursive){

	ofDirectory dir(path);
	dir.listDir();

	if (dir.size() == 0) { //if no images, proceed now.
		ofNotifyEvent(eventAllTexturesLoaded, this);
		ofLogWarning("AppStaticTextures") << "No textures found in the directory! \"" << path << "\"";
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
			if(loadAsync){
				pendingToLoad.push_back(filepath);
			}else{
				loadTexture(filepath); //load in place
			}
		}
	}
}


ofxAutoTexture* AppStaticTextures::loadTexture(const string& filePath){

	string lowercaseFilePath = ofToLower(filePath);
	bool useTex2D = ofIsStringInString(lowercaseFilePath, filenameHintTex2D);
	bool createMipMap = ofIsStringInString(lowercaseFilePath, filenameHintMipMap);

	//set OF ARB global state according to file naming
	bool usingARB = ofGetUsingArbTex();
	if(createMipMap){
		ofDisableArbTex();
	}else{
		if (!useTex2D){
			ofEnableArbTex();
		}else{
			ofDisableArbTex();
		}
	}

	string texName = ofFilePath::removeExt(filePath);
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
		ofLogError("AppStaticTextures") << "file name collision! " << filePath << " >> " << texName;
		ofLogError("AppStaticTextures") << "skipping texture at path: '" << filePath << "'";
		return NULL;
	}

	ofxAutoTexture * tex = new ofxAutoTexture();
	bool loaded = tex->loadFromFile(filePath);

	//restore of arb global state
	if(usingARB){ofEnableArbTex();}else{ofDisableArbTex();}

	if(loaded){

		texNameOrder.push_back(texName);
		textures[texName] = tex;
		float memUsedForThisOne = memUse(tex);
		memUsed += memUsedForThisOne;
		if(createMipMap){
			tex->generateMipmap();
			tex->setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
		}

		ofLogNotice("AppStaticTextures") 	<< "#### Loaded \"" << filePath << "\" ######################################################";
		ofLogNotice("AppStaticTextures")	<< "   Name: '" << texName << "'  " << "[" << tex->getWidth() << "x" << tex->getHeight() << "]" <<
		"  Mipmap: " << createMipMap << "  Format: " << ofGetGlInternalFormatName(tex->getTextureData().glInternalFormat) << "  Mem: " << ofToString(memUsedForThisOne, 2) << "Mb";
		return tex;
	}else{
		delete tex;
		ofLogError("AppStaticTextures") << "FAILED to load tex from \"" << filePath << "\"" ;
		return NULL;
	}
}


void AppStaticTextures::onUpdate(ofEventArgs & ){

	if(pendingToLoad.size() && ofGetFrameNum()%5 == 1){
		string currentFile = pendingToLoad.front();
		pendingToLoad.erase(pendingToLoad.begin());
		ofxAutoTexture * tex = loadTexture(currentFile);
		loadedInOrder.push_back(tex);
		if(pendingToLoad.size() == 0){
			ofLogNotice("AppStaticTextures") << "#### DONE loading " << textures.size() << " Static Textures! Memory used: " << ofToString(memUsed,2) << "Mb ############################################";
			ofNotifyEvent(eventAllTexturesLoaded, this);
		}
	}
}


ofTexture * AppStaticTextures::getLatestLoadedTex(){
	if(loadedInOrder.size()){
		return loadedInOrder.back();
	}else{
		return NULL;
	}
}

float AppStaticTextures::memUse(ofTexture * tex){

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
	ofLogError("AppStaticTextures") << "Tex doesn't use any memory bc its not allocated!";
	return 0;
}


ofTexture* AppStaticTextures::getTexture(string fullPath){

	//remove "/" from beginnig as it makes no sense
	if(fullPath.size() && fullPath[0] == '/'){
		fullPath = fullPath.substr(1, fullPath.size() -1);
	}
	
	auto it = textures.find(fullPath);
	if (it == textures.end()){
		ofLogError("AppStaticTextures") << "requesting a missing texture! " << fullPath;
		return &missingTex;
	}
	return it->second;
}


void AppStaticTextures::drawAll(const ofRectangle & rect){

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

		ofSetColor(55);
		ofDrawRectangle(frame);
		ofSetColor(22);
		ofNoFill();
		ofDrawRectangle(frame);
		ofFill();

		ofRectangle texR = ofRectangle(0,0,tex->getWidth(), tex->getHeight());
		texR.scaleTo(paddedFrame);
		ofSetColor(255);
		tex->draw(texR);
		ofDrawBitmapStringHighlight(texName, xx + 5, yy + frame.height - 8);
		xx += ceil(frame.width);
		if(xx >= rect.x + rect.width){
			yy += frame.height;
			xx = rect.x;
		}
	}
}

