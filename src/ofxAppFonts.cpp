//
//  ofxAppFonts.cpp
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#include "ofxAppFonts.h"
#include "ofxJsonSettings.h"
#include "ofxAppUtils.h"


void ofxAppFonts::setup(){
	loadOfxAppFont("monospaced", monospaced);
	loadOfxAppFont("monospacedBold", monospacedBold);
	loadUseFonts();
}

ofxFontStash* ofxAppFonts::getFont(const string& name){

	auto search = userFonts.find(name);
	if (search != userFonts.end()){
		return search->second;
	}else{
		string msg = "Can't find a font with that name! (" + name + ")";
		ofxApp::utils::terminateApp("ofxAppFonts", msg);
		return nullptr;
	}
}

void ofxAppFonts::loadUseFonts(){

	ofxJsonSettings & settings = ofxJsonSettings::get();
	if(settings.exists("Fonts/user")){
		ofxJSON json = settings.getJson("Fonts/user");

		if(json.isObject()){
			for( auto itr = json.begin(); itr != json.end(); itr++ ) { //walk all user fonts
				string fontName = itr.key().asString();
				string fontFile = (*itr)["fontFile"].asString();
				int atlasSize = (*itr)["atlasSize"].asInt();
				float lineH = (*itr)["lineHeight"].asFloat();
				bool mipmaps = (*itr)["mipmaps"].asBool();
				int mipmapPadding = (*itr)["mipmapPadding"].asInt();
				float uiScale = (*itr)["uiScale"].asFloat();

				ofxApp::utils::assertFileExists(fontFile);
				ofxFontStash * font = new ofxFontStash();

				font->setup(fontFile,
							lineH, 		//line height
							atlasSize, 		//tex atlas size
							mipmaps,		//mipmaps
							mipmapPadding, 	//atlas extra intra-char padding
							uiScale 	//ui Scale (per-char texture upscale)
						   );

				if (!(*itr)["kerning"].isNull()) {
					bool doKerning = (*itr)["kerning"].asBool();
					font->setKerning(doKerning);
				}

				auto search = userFonts.find(fontName);
				if (search == userFonts.end()){
					ofLogNotice("ofxAppFonts") << "Loading User Font '" << fontName << "' from '" << fontFile << "'";
					userFonts[fontName] = font;
				}else{
					ofxApp::utils::terminateApp("ofxAppFonts", "User Font with this name already exists! (" + fontName + ")");
				}

				float charSpacing = 0;
				if(!(*itr)["charSpacing"].isNull()){
					charSpacing = (*itr)["charSpacing"].asFloat();
					font->setCharacterSpacing(charSpacing);
					ofLogNotice("ofxAppFonts") << "Setting custom Character Spacing for font \""<< fontName << "\" : " << charSpacing;
				}
			}
		}else{
			ofxApp::utils::terminateApp("ofxAppFonts", "User Fonts (\"Fonts/user\")is not a Json Object! Check you \"AppSettings.json\" file!");
		}
	}
}

void ofxAppFonts::loadOfxAppFont(const string & key, ofxFontStash & font){

	ofxJsonSettings & settings = ofxJsonSettings::get();

	if(settings.exists("Fonts/ofxApp/" + key)){

		string fontFile = settings.getString("Fonts/ofxApp/" + key + "/fontFile");
		int atlasSize = settings.getInt("Fonts/ofxApp/" + key + "/atlasSize");
		float lineH = settings.getFloat("Fonts/ofxApp/" + key + "/lineHeight");
		bool mipmaps = settings.getBool("Fonts/ofxApp/" + key + "/mipmaps");
		int mipMapPadding = settings.getInt("Fonts/ofxApp/" + key + "/mipmapPadding");
		float retinaScale = settings.getFloat("Fonts/ofxApp/" + key + "/uiScale");

		ofxApp::utils::assertFileExists(fontFile);

		ofLogNotice("ofxAppFonts") << "Loading ofxApp Font'" << fontFile << "'";
		font.setup(	fontFile,
					lineH, 		//line height
					atlasSize, 		//tex atlas size
					mipmaps,		//mipmaps
					mipMapPadding, 	//atlas extra intra-char padding
					retinaScale 	//ui Scale (per-char texture upscale)
					);


	}else{
		ofxApp::utils::terminateApp("ofxAppFonts", "Missing required ofxApp font! Check your \"AppSettings.json\" file in \"Fonts/ofxApp/" + key + "\"");
	}
}
