//
//  AppStaticTextures.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#pragma once
#include "ofMain.h"
#include "ofxAutoTexture.h"

/*
 
 This class recurseivelly walks a dir and load any img file it finds in it.
 Images are loaded as ofxAutoTextures, which means they can be edited on the fly
 as the program runs.
 
 Images will load with different properties according to their filenames:
 
 	img.png >> will be load as GL_TEXTURE_RECTANGLE_ARB and (of c) no mipmaps
 	img_mip.png >> will be load as GL_TEXTURE_2D with mipmaps
 	img_t2d_mip.png >> will be load as GL_TEXTURE_2D with mipmaps
	img_t2d.png >> will be loaded as GL_TEXTURE_2D but no mipmaps will be created

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


class AppStaticTextures{

public:

	void setup();
	
	AppStaticTextures();

	void loadTexturesInDir(const string& imgDirPath);
	ofTexture* getTexture(string fullPath);

	static float memUse(ofTexture * tex); //in MBytes

	float getTotalMemUsed(){return memUsed;}
	void drawAll(const ofRectangle & rect);

protected:

	void loadTexture(const string& filename);
	void loadTexturesInDirectory(const string& path, bool recursive);

	string dirPath; //path to where the textures are
	unordered_map<string, ofxAutoTexture*> textures;

	const string filenameHintTex2D = "_t2d";
	const string filenameHintMipMap = "_mip";

	float memUsed = 0; //MBytes
	ofTexture missingTex;
};
