#pragma once

#include "ofxApp.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// CMS Object

class CWRU_Object : public ParsedObject, public AssetHolder, public TexturedObject{
public:
	string title;
	string description;
	string imagePath;
	ofVec2f imgSize;

	//TexturedObject imposes some method implementation so it can query about textures:
	//in this case we only have one img per obj to simplify the example
	ofVec2f getTextureDimensions(TexturedObjectSize s, int index){return imgSize;};
	string getLocalTexturePath(TexturedObjectSize, int index){return getAssetDescAtIndex(index).relativePath;}
	void deleteWithGC(){}; //this is effectivelly the TexturedObject destructor
};
