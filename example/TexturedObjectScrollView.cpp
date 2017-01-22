//
//  TexturedObjectScrollView.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 18/04/15.
//
//

#include "TexturedObjectScrollView.h"
#include "ofxTimeMeasurements.h"


// UTILS ////////////////////////////////////////////

void scaleRectToHeight(ofRectangle & r, int newH);
ofRectangle mixRectangles(const ofRectangle & r1, const ofRectangle & r2, float mix /*[0..1]*/);

void scaleRectToHeight(ofRectangle & r, int newH){
	if (newH == 0){
		ofLogError("TexturedObjectScrollView") << "Rectangle of Zero Height";
	}else{
		r.width *= (newH / r.height);
		r.height = newH;
	}
}


ofRectangle mixRectangles(const ofRectangle & r1, const ofRectangle & r2, float mix /*[0..1]*/){
	ofRectangle r;
	float iMix = 1.0f - mix;
	r.x = (mix) * r1.x + (iMix) * r2.x;
	r.y = (mix) * r1.y + (iMix) * r2.y;
	r.width = (mix) * r1.width + (iMix) * r2.width;
	r.height = (mix) * r1.height + (iMix) * r2.height;
	return r;
}


// CLASS ///////////////////////////////////////////

TexturedObjectScrollView::TexturedObjectScrollView(){

	layoutWidth = 0.0f;
	xOffset = 0.0f;
	prevX = 0.0f;
	scrollHandleAlpha = 1.0f;
	scrollInertia = 0.0f;
}


TexturedObjectScrollView::~TexturedObjectScrollView(){

	vector<TexturedObjectTexture> noContent;
	unloadContentGivenNewContent(noContent);

	map<TexturedObject*, int >::iterator it2 = listenerCounter.begin();
	while(it2 != listenerCounter.end()){
		ofRemoveListener(it2->first->textureLoaded, this, &TexturedObjectScrollView::objectTextureDidLoad);
		++it2;
	}
}


void TexturedObjectScrollView::setup(const ofRectangle& r, ScrollViewConfig cfg){

	config = cfg;

	setPosition(ofVec2f(r.x, r.y));
	Node::setSize(r.width, r.height);

	ofFbo::Settings fboSettings;
	fboSettings.width = r.width;
	fboSettings.height = r.height;
	fboSettings.useDepth = false;
	fboSettings.textureTarget = GL_TEXTURE_RECTANGLE_ARB; //mipmaps!
	fboSettings.internalformat = config.internalFormat;
	fboSettings.numSamples = config.numFboSamples;
	fboSettings.numColorbuffers = 1;

	fbo.allocate(fboSettings); //no antialias needed here?
	fbo.begin();
	ofClear(0,0,0,0);
	fbo.end();
	
	canvas.width = r.width;
	canvas.height = r.height;

	expandedCanvas = canvas; //used to control scrollview clipping
	expandedCanvas.x -= config.canvasLookAheadDist;
	expandedCanvas.width += config.canvasLookAheadDist * 2;

	scrollCanvas = expandedCanvas;
	xOffset = scrollInertia = 0.0f;
	touchID = -1;
	fboDirty = 0.2;
	autoScrolling = false;

	gradientR.clear();
	gradientR.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	gradientR.addVertex(ofVec3f(r.width - config.scrollviewEdgeGradientWidth,0));
	gradientR.addVertex(ofVec3f(r.width - config.scrollviewEdgeGradientWidth,r.height));
	gradientR.addVertex(ofVec3f(r.width,0));
	gradientR.addVertex(ofVec3f(r.width,r.height));
	gradientR.addColor(ofColor(config.bgColor,0));
	gradientR.addColor(ofColor(config.bgColor,0));
	if(config.bgColor.a == 0){
		gradientR.addColor(ofColor(config.bgColor,255));
		gradientR.addColor(ofColor(config.bgColor,255));
	}else{
		gradientR.addColor(ofColor(config.bgColor));
		gradientR.addColor(ofColor(config.bgColor));
	}

	gradientL.clear();
	gradientL.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	gradientL.addVertex(ofVec3f(0,0));
	gradientL.addVertex(ofVec3f(0,r.height));
	gradientL.addVertex(ofVec3f(config.scrollviewEdgeGradientWidth,0));
	gradientL.addVertex(ofVec3f(config.scrollviewEdgeGradientWidth,r.height));
	if(config.bgColor.a == 0){
		gradientL.addColor(ofColor(config.bgColor,255));
		gradientL.addColor(ofColor(config.bgColor,255));
	}else{
		gradientL.addColor(ofColor(config.bgColor));
		gradientL.addColor(ofColor(config.bgColor));
	}
	gradientL.addColor(ofColor(config.bgColor,0));
	gradientL.addColor(ofColor(config.bgColor,0));

	ofAddListener(eventTouchMove, this, &TexturedObjectScrollView::onTouchMove);
	ofAddListener(eventTouchUp, this, &TexturedObjectScrollView::onTouchUp);
	ofAddListener(eventTouchDown, this, &TexturedObjectScrollView::onTouchDown);
}


void TexturedObjectScrollView::loadContent(const vector<TexturedObject*>& textures_){

	vector<TexturedObjectTexture> tots;
	for(int i = 0; i < textures_.size(); i++){
		TexturedObjectTexture texObjTex;
		texObjTex.texIndex = 0;
		texObjTex.texObj = textures_[i];
		tots.push_back(texObjTex);
	}
	loadContent(tots);
}


void TexturedObjectScrollView::unloadContentGivenNewContent(vector<TexturedObjectTexture>& textures_){

	//lets walk the list of images to be dropped, and unload them if they are loaded
	for(int i = 0; i < objects.size(); i++){

		ofTexture * t = objects[i].texObj->getRealTexture(config.sizeToLoad, objects[i].texIndex);

		if(loadedTextures.find(t) != loadedTextures.end()){ //found the tex for the gone object as loaded!
			//ofLogNotice("TexturedObjectScrollView") << "unloading LOADED texture " << loadedTextures[t];
			unloadTexForObject(loadedTextures[t]);
		}

		if(loadingTextures.find(t) != loadingTextures.end()){ //found the tex for the gone object as loaded!
			//ofLogNotice("TexturedObjectScrollView") << "unloading LOADING texture " << loadingTextures[t];
			unloadTexForObject(loadingTextures[t]);
		}
	}

	if (textures_.size() == 0 && ( loadingTextures.size() > 0 || loadedTextures.size() > 0)){
		ofLogError("TexturedObjectScrollView") << "wtf!";
	}
}


void TexturedObjectScrollView::loadContent(vector<TexturedObjectTexture>& textures_){

	autoScrolling = false;
	if(textures_.size() == 0){
		ofLogError("TexturedObjectScrollView") << "loadContent: no images provided!";
	}

	unloadContentGivenNewContent(textures_);
	
	this->objects = textures_;
	int currentRow = 0;
	layoutWidth = 0;
	xOffset = scrollInertia = 0;

	layout.clear(); //start new
	objectToLayout.clear();

	//height of each item
	float hh = ((canvas.height - (config.numRows - 1) * config.padding - config.marginPadding * 2  ) / config.numRows);

//	if(objects.size() == 1){
//		ofVec2f dim = objects[0].texObj->getTextureDimensions(config.sizeToLoad, objects[0].texIndex);
//		float drawnW = hh * (dim.x / dim.y);
//		extraLeftSpaceTarget = (canvas.width - drawnW) / 2;
//	}else{
//		extraLeftSpaceTarget = 0;
//	}

	std::vector<double> x(config.numRows, config.marginPadding);
	std::vector<int> cols(config.numRows, 0);

	for(int i = 0; i < objects.size(); i++){

		LayoutObject lo;
		int texIndex = objects[i].texIndex;
		ofVec2f dim = objects[i].texObj->getTextureDimensions(config.sizeToLoad, texIndex);

		std::vector<double>::iterator it = std::min_element(x.begin(), x.end());
		currentRow = it - x.begin();

		ofRectangle r = ofRectangle(x[currentRow],
									config.marginPadding + currentRow * (hh + config.padding),
									dim.x,
									dim.y);
		scaleRectToHeight(r, hh);
		lo.placement = r;
		lo.row = currentRow;
		lo.ID = i;
		lo.texObjTex = objects[i];
		lo.fade.reset(0);
		lo.fade.setCurve(SMOOTHER_STEP);
		lo.enabled = true;
		lo.visible = false;
		lo.fade.setDuration(config.imgFadeInDuration);
		layout.push_back(lo);

		objectToLayout[objects[i].texObj] = i;

		cols[currentRow] = cols[currentRow] + 1; //this row has a new item in it!
		x[currentRow] = x[currentRow] + r.width + config.padding;
		if(x[currentRow] > layoutWidth){
			layoutWidth = x[currentRow];
		}
	}

	layoutWidth -= config.padding;
	layoutWidth += config.marginPadding;

	if(fboDirty < 0.2) fboDirty = 0.2;
	update(0.01);
}


void TexturedObjectScrollView::update(float dt){

	//handle scroll inertias
	if(touchID == -1){ //only rubberBand when not touching

		//extraLeftSpace = 0.01 * extraLeftSpaceTarget + 0.99 * extraLeftSpace;
		xOffset += scrollInertia ;

		scrollInertia *= config.scrollFriction; //slow down
		if(fabs(scrollInertia) > 0.01){ //if we are still moving with inertia, make sure we refresh fbo
			if(fboDirty < 0.2) fboDirty = 0.2;
		}

		xOffset = ofClamp(xOffset, -canvas.width, layoutWidth - getWidth() + canvas.width);
		float viewW = layoutWidth - getWidth();

		//handle bounce-back inside screen if user pushed us out of scroll bounds
		if(xOffset < 0.0f){
			scrollInertia *= config.rubberOffScreenForce;
			xOffset = xOffset * config.rubberForce;
			if(fboDirty < 0.2 && xOffset < -0.1f && layout.size() > 0) fboDirty = 0.2;
		}else{
			if(xOffset > viewW){
				scrollInertia *= config.rubberOffScreenForce;
				if(viewW > 0.0){
					float diff = xOffset - (viewW);
					xOffset -= (diff) * (1.0f - config.rubberForce);
				}else{
					xOffset -= (xOffset) * (1.0f - config.rubberForce);
				}
				if(fboDirty < 0.2 && fabs(xOffset - viewW) > 0.1) fboDirty = 0.2;
			}
		}

		float autoScrollDiff = fabs(autoScrollX - xOffset);

		if(autoScrollDiff > 1 && autoScrolling){
			float r = 0.2;
			float d = autoScrollX - xOffset;
			xOffset = r * autoScrollX + (1.0f - r) * xOffset;
			if(fboDirty < 0.2) fboDirty = 0.2;
		}else{
			if(autoScrollDiff < 1 && autoScrolling){
				autoScrolling = false;
			}
		}

		if (config.snapToImage){ //find the closest slide to snap to
			float closestX = -1;
			float minDiff = FLT_MAX;
			for(int i = 0; i < objects.size(); i++){
				float firstMargin = (i == 0 ? config.marginPadding : config.padding);
				float dist = fabs(xOffset - layout[i].placement.x + firstMargin);
				if(dist < minDiff){
					minDiff = dist;
					closestX = layout[i].placement.x - firstMargin;
				}
			}
			if(closestX >= 0.0f && closestX < (layoutWidth - canvas.width)){ //only if inside the view
				if(minDiff > 0.5){ //if we are far from snap position
					fboDirty = 0.2;
					float snapForce = ofMap(fabs(scrollInertia), 0, 10, config.snapForce, 1.0f, true);
					//float maxOffset = 10;
					float clampDiff = ofClamp(fabs(closestX - xOffset), 0, config.snapMaxOffsetPerFrame); //max diff per frame
					float sign = (closestX - xOffset > 0.0 ? 1.0f : -1.0f);
					float clampedClosestX = xOffset + sign * clampDiff;

					float offsetToApply = (1.0f - snapForce) * clampedClosestX;

					xOffset = xOffset * snapForce + offsetToApply ;
				}
			}
		}
	}

	fboDirty -= ofGetLastFrameTime();

	//re-update the virtual canvas
	scrollCanvas = expandedCanvas;
	scrollCanvas.x += xOffset;

	//take care of loading and unloading textures that are on view
	for(int i = 0; i < layout.size(); i++){

		ofRectangle rWithOffset = layout[i].placement;
		bool onView = scrollCanvas.intersects(rWithOffset);
		bool onViewNow = canvas.intersects(rWithOffset);

		if(layout[i].enabled){
			layout[i].fade.update(dt);

			if(onView && layout[i].fade.isAnimating()){ //if fading, update fbo
				if(fboDirty < 0.2) fboDirty = 0.2;
			}

			//animation - not used!
			//layout[i].placement = rWithOffset; mixRects(layout[i].placement, rWithOffset, config.layoutForce);

			if(layout[i].texObjTex.texObj != NULL){
				if(onView && layout[i].visible == false){
					loadTexForObject(i, onViewNow);
					layout[i].visible = true;
				}else{
					if(!onView && layout[i].visible == true){
						unloadTexForObject(i);
						layout[i].visible = false;
					}
				}
			}else{
				ofLogError("TexturedObjectScrollView") << "Object At Layout index " << i << " is NULL!";
			}
		}
	}

	if(fboDirty > 0.0f){
		updateFbo();
	}
}


void TexturedObjectScrollView::loadTexForObject(int layoutIndex, bool isOnscreen){

	TexturedObject * to = layout[layoutIndex].texObjTex.texObj;
	int texIndex = layout[layoutIndex].texObjTex.texIndex;
	ofTexture * t = to->getRealTexture(config.sizeToLoad, texIndex);

	if(t){ //if object exists in DB
		map<ofTexture*, int>::iterator it = loadingTextures.find(t);
		if(it == loadingTextures.end()){
			layout[layoutIndex].fade.reset(0);
			if(listenerCounter[to] == 0){
				ofAddListener(to->textureLoaded, this, &TexturedObjectScrollView::objectTextureDidLoad);
			}
			listenerCounter[to] ++;

			to->requestTexture(config.sizeToLoad, texIndex, isOnscreen, config.mipmaps);
			loadingTextures[t] = layoutIndex;
		}else{
			ofLogError("TexturedObjectScrollView") << "already loading! " << layoutIndex;
		}
	}else{
		ofLogError("TexturedObjectScrollView") << "wtf no tex!";
	}
}


void TexturedObjectScrollView::unloadTexForObject(int layoutIndex){

	TexturedObject * to = layout[layoutIndex].texObjTex.texObj;
	int texIndex = layout[layoutIndex].texObjTex.texIndex;
	ofTexture * t = to->getRealTexture(config.sizeToLoad, texIndex);

	if(t){ //if object exists in DB

		//see if it was loading, remove from loading list if it was (tex load will get canceld!)
		map<ofTexture*, int>::iterator it = loadingTextures.find(t);
		if(it != loadingTextures.end()){
			loadingTextures.erase(it);
		}

		layout[layoutIndex].fade.reset(0);
		if(config.unloadDelay >= 0){
			to->releaseTexture(config.sizeToLoad, texIndex, config.unloadDelay);
		}else{ //defaults - whatever TexturedObjectConfig::one().setDefaultTextureUnloadDelay() is set to
			to->releaseTexture(config.sizeToLoad, texIndex);
		}

		//remove from loaded tex list
		it = loadedTextures.find(t);
		if (it != loadedTextures.end()){
			loadedTextures.erase(it);
		}
	}else{
		ofLogError("TexturedObjectScrollView") << "wtf no tex!";
	}
}


void TexturedObjectScrollView::objectTextureDidLoad(TexturedObject::TextureEventArg &e){

	if(e.loadedOk){

		map<ofTexture*, int>::iterator it = loadingTextures.find(e.tex);

		if(it != loadingTextures.end()){

			int layoutIndex = it->second;
			TexturedObject * to = layout[layoutIndex].texObjTex.texObj;
			//layout[layoutIndex].fade.animateTo(1);
			layout[layoutIndex].fade.reset(1);
			loadingTextures.erase(it);

			if(listenerCounter[to] == 1){
				ofRemoveListener(layout[layoutIndex].texObjTex.texObj->textureLoaded, this, &TexturedObjectScrollView::objectTextureDidLoad);
			}
			listenerCounter[to]--;
			loadedTextures[e.tex] = layoutIndex;

			fboDirty = FBO_DIRTY_TIME;
		}
	}else{
		ofLogError("TexturedObjectScrollView") << "ERROR at loading texture";
	}
}


void TexturedObjectScrollView::scrollToElement(int index){

	LayoutObject lo = layout[index];
	autoScrollX = lo.placement.x;
	autoScrolling = true;
}


void TexturedObjectScrollView::scrollToElement(TexturedObject* o){

	LayoutObject lo = layout[objectToLayout[o]];
	autoScrollX = lo.placement.x;
	autoScrolling = true;
}


void TexturedObjectScrollView::updateFbo(){

	#if(BENCH_TEXTURED_OBJECT_SCROLLVIEW)
	TS_START_ACC("TexturedObjectScrollView u");
	#endif
	fbo.begin();
	ofClear(config.bgColor);
	
	ofPushMatrix();
	ofTranslate(-xOffset, 0);

	for(int i = 0; i < objects.size(); i++){

		if(layout[i].enabled){
			ofRectangle rWithOffset = layout[i].placement;
			//rWithOffset.x += extraLeftSpaceTarget;
			bool onView = scrollCanvas.intersects(rWithOffset);
			if(onView){
				//layout[i].texObject->getTexture(sizeToLoad, 0)->draw(rWithOffset);
				int texIndex = layout[i].texObjTex.texIndex;
				if(layout[i].texObjTex.texObj->isReadyToDraw(config.sizeToLoad, texIndex)){
					float a = layout[i].fade.val();
					ofSetColor(255, 255 * a);
					layout[i].texObjTex.texObj->getRealTexture(config.sizeToLoad, texIndex)->draw(rWithOffset);

					if(config.customTileDraw){
						DrawTileInfo dti;
						dti.area = rWithOffset;
						dti.who = this;
						dti.alpha = a;
						dti.layoutID = i;
						dti.objTex = layout[i].texObjTex;
						ofNotifyEvent(eventTileDraw, dti, this);
					}
				}else{
					if(config.drawLoadingRects){
						ofSetColor(config.loadingRectColors);
						ofDrawRectangle(rWithOffset);
					}
				}
			}
		}
	}
	ofPopMatrix();

	if(config.drawMoreContentHintGradients){
		drawScrollGradients();
	}

	fbo.end();
	#if(BENCH_TEXTURED_OBJECT_SCROLLVIEW)
	TS_STOP_ACC("TexturedObjectScrollView u");
	#endif
}


void TexturedObjectScrollView::drawDebug(){
	Node::drawDebug();
	//debug fbo refresh
	if(fboDirty > 0.0f){
		if(ofGetFrameNum()%10 > 5){
			ofSetColor(255, 0, 0);
			ofFill();
			ofDrawCircle(10, 10, 3);
		}
	}
	string msg = "Loading: " + ofToString(loadingTextures.size()) + "\nLoaded: " + ofToString(loadedTextures.size());
	msg += "\nxOffset:" + ofToString(xOffset,1);
	ofDrawBitmapStringHighlight(msg, 3, getHeight() - 20);
}


void TexturedObjectScrollView::draw(){

	#if(BENCH_TEXTURED_OBJECT_SCROLLVIEW)
	TS_START_ACC("TOScrollView d");
	#endif
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	fbo.draw(0,0);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);

	//scroll handle math
	float scrollAreaViewable = canvas.width / (layoutWidth );
	float scrollHandleW = ofClamp(canvas.width * scrollAreaViewable, 0, config.maxScrollHandleW);
	ofRectangle scrollHandle = ofRectangle(0,0, scrollHandleW, config.scrollHandleH );
	float scrollHandlePad = 4;
	float maxX = (canvas.width - scrollHandle.width) - 2 * config.scrollHandlePad;
	float scrollPercent = ofClamp( xOffset / (layoutWidth  - canvas.width), 0, 1);
	scrollHandle.x = config.scrollHandlePad + scrollPercent * maxX;
	scrollHandle.y = canvas.height - config.scrollHandleH * 0.5f + config.scrollHandleVertPostion;
	if(scrollHandleAlpha > 1.0f) scrollHandleAlpha = 1.0f;
	if(fboDirty < 0.1){
		scrollHandleAlpha *= 0.8;
	}

	//draw scroll handle
	if(scrollHandleAlpha > 0.01){
		ofSetColor(config.scrollHandleColor, scrollHandleAlpha * 255);
		ofDrawRectangle(scrollHandle);
		ofSetColor(255);
	}
	#if(BENCH_TEXTURED_OBJECT_SCROLLVIEW)
	TS_STOP_ACC("TOScrollView d");
	#endif
}


void TexturedObjectScrollView::drawScrollGradients(){
	if(config.bgColor.a == 0){
		glBlendFuncSeparate(GL_ZERO, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA); // THE RIGHT BLENDING!
	}
	ofPushMatrix();
	float xR = ofClamp(layoutWidth  - canvas.width - xOffset - config.scrollviewEdgeGradientWidth,
					   -config.scrollviewEdgeGradientWidth, 0);
	ofTranslate(-xR,0);
	gradientR.draw();
	ofPopMatrix();

	ofPushMatrix();
	float xL = ofClamp(xOffset - config.scrollviewEdgeGradientWidth, -config.scrollviewEdgeGradientWidth, 0 );
	ofTranslate(xL ,0);
	gradientL.draw();
	ofPopMatrix();
	if(config.bgColor.a == 0){
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	}
}


void TexturedObjectScrollView::onTouchDown(TouchEvent &event){
	
	if(touchID == -1){
		touchID = event.id;
		scrollInertia = 0.0;
		//scrollHandleAlpha = 1.0;
		prevX = toLocal(event.position).x;
	}
}


void TexturedObjectScrollView::onTouchMove(TouchEvent &event){

	if(touchID == event.id){
		
		scrollHandleAlpha = 1.0;
		ofVec2f localPos = toLocal(event.position);

		xOffset += (prevX - localPos.x);
		scrollInertia = (prevX - localPos.x) * config.scrollUserForceGain;
		prevX = localPos.x;
		fboDirty = FBO_DIRTY_TIME;
		xOffset = ofClamp(xOffset, -canvas.width, layoutWidth - getWidth() + canvas.width);
	}
}


void TexturedObjectScrollView::onTouchUp(TouchEvent &event){

	float touchDurationSec = (event.timestamp - event.prevTimestamp) / 1000000.0f;

	if(touchDurationSec < 0.3 && event.position.distance(event.firstPosition) < 20 ){ //tapped! go to browsing

		TexturedObject * touchedObject = NULL;
		int texIndex = -1;

		int ID = -1;
		ofVec2f virtualPoint = toLocal(event.position);
		virtualPoint.x += xOffset;
		for(int i = 0; i < layout.size(); i++){
			if(layout[i].placement.inside(virtualPoint)){
				touchedObject = layout[i].texObjTex.texObj;
				texIndex = layout[i].texObjTex.texIndex;
				ID = layout[i].ID;
				//TODO send event!

				TouchedImage ti;
				ti.objTex.texObj = touchedObject;
				ti.objTex.texIndex = texIndex;
				ti.who = this;
				ti.layoutID = ID;

				ofNotifyEvent(eventTextureClicked, ti, this);
			}
		}
	}

	if(touchID == event.id){
		touchID = -1;
	}
}
