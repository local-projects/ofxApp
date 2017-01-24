#pragma once

#include "ofxApp.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// CMS Object

class CH_Object : public ContentObject{
public:
	string title;
	string description;
	string objectID;

	struct CH_Image{
		string url;
		string sha1;
		string localAssetPath;
		ofVec2f imgSize;
		bool isPrimary;
	};

	vector<CH_Image> images;


	//TexturedObject imposes some method implementation so it can query about textures:
	//in this case we only have one img per obj to simplify the example
	ofVec2f getTextureDimensions(TexturedObjectSize s, int index){
		return images[index].imgSize;
	};


	string getLocalTexturePath(TexturedObjectSize, int index){
		return getAssetDescAtIndex(index).relativePath;
	}

	void deleteWithGC(){
		TexturedObject::deleteWithGC();
	}; //this is effectivelly the TexturedObject destructor
};
