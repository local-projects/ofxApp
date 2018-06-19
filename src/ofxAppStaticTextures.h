//
//  ofxAppStaticTextures.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#pragma once
#include "ofMain.h"
#include <thread>

/*
 
 This class recurseivelly walks a dir and load any img file it finds in it.
 Images are loaded as ofxAutoTextures, which means they can be edited on the fly
 as the program runs.
 
 Images will load with different properties according to their filenames:
 
 	img.png >> will load as GL_TEXTURE_RECTANGLE_ARB and (of c) no mipmaps
 	img_mip.png >> will load as GL_TEXTURE_2D with mipmaps
 	img_t2d_mip.png >> will load as GL_TEXTURE_2D with mipmaps
	img_t2d.png >> will load as GL_TEXTURE_2D but no mipmaps will be created

 so basically:
 	"_t2d" > stands for GL_TEXTURE_2D - unless specified, the tex will be loaded as GL_TEXTURE_RECTANGLE_ARB
 	"_mip" > stands for mipmaps - mipmaps will only be created when requested with "_mip"
 
 if you request mipmaps the tex will automatically be treated as GL_TEXTURE_2D

 
 To get the textures back, its up to you to describe the path as its in the filesystem:
 
 Given this dir:

 	/images
 		cat_t2d_mip.png
 		/monkeys
 			/monkey1.png
 			/monkey2_mip.png
		/bananas
 			/banana_joe1_t2d.psd
 
 to get the textures, you must call 
 
 	getTexture("cat");
 	getTexture("monkeys/monkey1");
 	getTexture("monkeys/monkey2");
	getTexture("bananas/banana_joe1");
 
 Note that:

 	the tex loading properties ("_t2d" and "_mip") are removed from the texture name,
 	the file extension is removed from the texture name,
 	the upper level dirname ("images" in this case) is removed from the texture name

 */

class ofxAutoTexture;
class ofxAppStaticTextures{

public:

	static std::string filenameHintTex2D;
	static std::string filenameHintMipMap;

	void setup();

	ofxAppStaticTextures();

	void setMipmapLodBias(float bias);
	void setAnisotropy(float anisotropy);

	void loadTexturesInDir(const std::string& imgDirPath, int maxThreads = std::thread::hardware_concurrency());
	ofTexture* getTexture(std::string textureName);

	static float memUse(ofTexture * tex); //in MBytes
	void setForceMipmaps(bool f){forceMipmapsOnAll = f;}

	int getNumTextures(){ return textures.size();}
	int getNumLoadedTextures(){ return loaded.size();}

	vector<std::string> getTextureNames(){return texNameOrder;};

	float getTotalMemUsed(){ return memUsed;} //in MBytes

	void drawAll(const ofRectangle & rect); //debug call to see all textures in one giant grid
											//mostly to check texture names

	ofEvent<void> eventAllTexturesLoaded;


protected:

	int maxThreads;
	
	class ThreadedLoader;
	
	struct PreLoadData {
	public:
		ofxAutoTexture* tex;
		std::string texName;
		std::string filePath;
		bool useTex2D;
		bool createMipmap;
		ThreadedLoader * loader = NULL;
	};

	
	class ThreadedLoader : public ofThread{
		public:
		PreLoadData data;
		bool threadStarted = false;
		bool preloaded = false;
		void threadedFunction();
	};
	
	void onUpdate(ofEventArgs & );

	ofxAutoTexture* loadTexture(PreLoadData data);
	void loadTexturesInDirectory(const std::string& path, bool recursive);

	std::string dirPath; //path to where the textures are
	
	vector<PreLoadData> pendingToPreLoad;
	vector<PreLoadData> loaded;
	
	vector<std::string> texNameOrder;
	unordered_map<std::string, ofxAutoTexture*> textures;
	
	ofxAutoTexture* createTexObjForPath(std::string filePath, std::string & texName, bool & createMipMap, bool & useTex2D);

	float memUsed = 0; //MBytes
	bool isLoading = false;
	ofTexture missingTex;

	float startLoadTime;

	bool forceMipmapsOnAll = false;
	float mipmapLodBias = 0.0f;
	float anisotropy = 0.0f;
	float maxAnisotropy = 0.0f;
};
