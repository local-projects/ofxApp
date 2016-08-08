//
//  AppStaticTextures.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#include "AppStaticTextures.h"

AppStaticTextures::AppStaticTextures(){
}

void AppStaticTextures::setup(){
	missingTex.allocate(1,1,GL_RGBA);
}

void AppStaticTextures::loadTexturesInDir(const string& imgDirPath){
	dirPath = imgDirPath;
	dirPath = ofFilePath::addTrailingSlash(dirPath);
	loadTexturesInDirectory(imgDirPath, true);
}

void AppStaticTextures::loadTexturesInDirectory(const string& path, bool recursive){

	ofDirectory dir(path);
	dir.listDir();

	for(int i = 0; i < dir.size(); i++){

		ofFile file = dir.getFile(i);

		if (file.isDirectory() && recursive) {
			loadTexturesInDirectory(path + "/" + file.getFileName(), recursive);
			continue;
		}
		string ext = ofToLower(file.getExtension());
		if (ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif" || ext == "tga" || ext == "tiff" || ext == "psd") {
			string filepath = path + "/" + file.getBaseName() + "." + file.getExtension();
			loadTexture(filepath);
		}
	}
}


void AppStaticTextures::loadTexture(const string& filePath){

	string lowercaseFilePath = ofToLower(filePath);
	bool useTex2D = ofIsStringInString(lowercaseFilePath, filenameHintTex2D);
	bool createMipMap = ofIsStringInString(lowercaseFilePath, filenameHintMipMap);

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

	string texName = ofFilePath::removeExt(filePath);
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
		return;
	}

	ofxAutoTexture * tex = new ofxAutoTexture();
	bool loaded = tex->loadFromFile(filePath);
	if(loaded){

		ofLogNotice("AppStaticTextures") << "loaded static texture from '" << filePath << "'";
		ofLogNotice("AppStaticTextures") << "name: '" << texName << "'  size: [" << tex->getWidth() << "x" << tex->getHeight() << "]";
		textures[texName] = tex;
		float memUsedForThisOne = memUse(tex);
		memUsed += memUsedForThisOne;
	}else{
		delete tex;
		ofLogError("AppStaticTextures") << "FAILED to load static texture from " << filePath;
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

		int numC = ofGetNumChannelsFromGLFormat(tex->texData.glInternalFormat);
		float mem = w * h * numC;
		if(tex->hasMipmap()){
			mem += 1.3333; //mipmaps take 33% more memory
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
		ofLogError("AppStaticTextures") << "requestnig a missing texture! " << fullPath;
		return &missingTex;
	}
	return it->second;
}


void AppStaticTextures::drawAll(const ofRectangle & rect){

	int n = textures.size();
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

	ofSetColor(11);
	ofDrawRectangle(rect);
	ofSetColor(255);

	for(auto it : textures){
		ofRectangle frame = ofRectangle(xx,yy, rect.width / ceil(nx), rect.height / ceil(ny));
		ofRectangle texR = ofRectangle(0,0,it.second->getWidth(), it.second->getHeight());
		texR.scaleTo(frame);
		it.second->draw(texR);
		ofDrawBitmapStringHighlight(it.first, xx + 5, yy + frame.height - 8);
		xx += ceil(frame.width);
		if(xx >= rect.x + rect.width){
			yy += frame.height;
			xx = rect.x;
		}
		ofNoFill();
		ofDrawRectangle(frame);
		ofFill();
	}


}

