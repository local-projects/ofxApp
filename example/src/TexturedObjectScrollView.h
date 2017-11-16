//
//  TexturedObjectScrollView.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 18/04/15.
//
//

#ifndef __BaseApp__TexturedObjectScrollView__
#define __BaseApp__TexturedObjectScrollView__

#include "ofMain.h"
#include "ofxInterface.h"
#include "TexturedObject.h"
#include "ofxAnimatableFloat.h"

#define FBO_DIRTY_TIME	1.0

#define BENCH_TEXTURED_OBJECT_SCROLLVIEW 	FALSE

using namespace ofxInterface;

class TexturedObjectScrollView: public Node{

public:

	struct ScrollViewConfig{

		//layout
		int numRows = 1;
		float padding = 10; //between images
		float marginPadding = 0; //from edges
		float bottomPadding = 0; //apart from global padding (marginPadding) extra pad at bottom
		float topPadding = 0; //apart from global padding (marginPadding) extra pad at bottom

		//unloaded images
		bool drawLoadingRects = false;
		ofColor loadingRectColors = ofColor(255,32);

		//custom draw event sent for each tile drawn
		bool customTileDraw = false;

		//fbo
		int numFboSamples = 0;
		int internalFormat = GL_RGBA;

		//textures
		TexturedObjectSize sizeToLoad;
		bool mipmaps = true;
		float imgFadeInDuration = 0.2; //when img loaded, how long it takes to fade in

		//caching
		float canvasLookAheadDist = 10; //look ahead pixels off each side of the canvas, to pre-load
		float unloadDelay = -1.0; //when img is offscreen, how long to wait until unloading it (cheap cache)
		//defaults to the default TexturedObjectConfig delay; you can set the defaults with
		//TexturedObjectConfig::one().setDefaultTextureUnloadDelay(myDelay);

		//scrolling
		float scrollFriction = 0.97;
		float rubberForce = 0.9; //rubberband effect when pulling out of area
		float rubberOffScreenForce = 0.9; //same
		float scrollviewEdgeGradientWidth = 30; //fade out edges of scrollview when not on the ends
		bool drawMoreContentHintGradients = true;
		float scrollUserForceGain = 1.0;

		//scroll bar
		float scrollHandleH = 2; //height of the scroll thingie
		float scrollHandleVertPostion = 2; //v offset
		float maxScrollHandleW = 300;
		float scrollHandlePad = 4;

		//colors
		ofColor bgColor = ofColor(0,0,0,0);
		ofColor scrollHandleColor = ofColor::white;

		//auto snap to index
		bool snapToImage = true;
		float snapForce = 0.80; /*should be >0.0 && <1.0*/
		float snapMaxOffsetPerFrame = 50;

	};

	//a TextueredObject can contain N textures;
	//we need to identify uniquely; this is how
	struct TexturedObjectTexture{
		TexturedObject * texObj;
		int texIndex;

		//we are using this to drive maps, we need to be able to compare these
		bool operator<( const TexturedObjectTexture & n ) const {
			if(n.texObj != texObj) return texObj < n.texObj;
			else{
				return texIndex < n.texIndex;
			}
		}
		
		bool operator== (const TexturedObjectTexture &n){
			return texObj == n.texObj && texIndex == n.texIndex;
		}
	};

	TexturedObjectScrollView();
	~TexturedObjectScrollView();

	void setup(const ofRectangle& r, ScrollViewConfig config);

	//supply TexturedObject*'s to only draw the first image of the each object
	void loadContent(const vector<TexturedObject*>& objects);

	//if you need more precision, you can supply TexturedObjectTexture's
	//to specify what texture ID(s) to draw for each TextureObject
	void loadContent(vector<TexturedObjectTexture>& textures);

	// touch //
	void onTouchUp(TouchEvent &event);
	void onTouchMove(TouchEvent &event);
	void onTouchDown(TouchEvent &event);

	void scrollToElement(int index);
	void scrollToElement(TexturedObject* obj);

	void update(float dt);
	void draw();
	void drawDebug();

	void forceRedraw(){fboDirty = 0.2;};

	void objectTextureDidLoad(TexturedObject::TextureEventArg &e);

	struct TouchedImage{
		TexturedObjectTexture objTex;
		TexturedObjectScrollView * who;
		int layoutID;
	};

	struct DrawTileInfo{
		ofRectangle area;
		float alpha;
		TexturedObjectScrollView * who;
		int layoutID;
		TexturedObjectTexture objTex;
	};

	ofEvent<TouchedImage> eventTextureClicked;
	ofEvent<DrawTileInfo> eventTileDraw;

	ofFbo& getFbo(){return fbo;};

protected:

	struct LayoutObject{
		int ID;
		int row;
		bool enabled;
		bool visible;
		TexturedObjectTexture texObjTex;
		ofxAnimatableFloat fade;
		ofRectangle placement;
	};

	vector<TexturedObjectTexture> objects;
	vector<LayoutObject> layout;
	map<TexturedObject*, int > objectToLayout; //index to to layout index

	map<ofTexture*, int> loadingTextures; //maps to layout index
	map<ofTexture*, int> loadedTextures; //maps to layout index
	map<TexturedObject*, int > listenerCounter;

	ScrollViewConfig config;

	float layoutWidth;

	ofFbo fbo;
	float fboDirty;
	void updateFbo();

	ofRectangle canvas;

	float xOffset; //controls scrolling
	float prevX;
	float scrollInertia;

	float autoScrollX;
	bool autoScrolling;

//	float extraLeftSpace; //used to center content when there's only one image in the view
//	float extraLeftSpaceTarget;

	float scrollHandleAlpha;
	ofVboMesh gradientR;
	ofVboMesh gradientL;

	ofRectangle scrollCanvas;
	ofRectangle expandedCanvas; //same as canvas, but with lookahead section to cache a bit furhter

	int touchID;

	void drawScrollGradients();

	void loadTexForObject(int layoutIndex, bool isOnscreen = false);
	void unloadTexForObject(int layoutIndex);

	void unloadContentGivenNewContent(vector<TexturedObjectTexture>& textures_);

};

#endif /* defined(__BaseApp__TexturedObjectScrollView__) */

