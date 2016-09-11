//
//  AppFonts.cpp
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#include "AppFonts.h"
#include "ofxJsonSettings.h"


void AppFonts::setup(){
	loadOfxAppFont("monospaced", monospaced);
	loadOfxAppFont("monospacedBold", monospacedBold);
	loadUseFonts();
}

ofxFontStash* AppFonts::getFont(const string& name){

	auto search = userFonts.find(name);
	if (search != userFonts.end()){
		return search->second;
	}else{
		ofLogError("AppFonts") << "Can't find a font with that name! (" << name << ")";
		terminateApp();
		return nullptr;
	}
}

void AppFonts::loadUseFonts(){

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

				assertFileExists(fontFile);
				ofxFontStash * font = new ofxFontStash();

				font->setup(fontFile,
							lineH, 		//line height
							atlasSize, 		//tex atlas size
							mipmaps,		//mipmaps
							mipmapPadding, 	//atlas extra intra-char padding
							uiScale 	//ui Scale (per-char texture upscale)
						   );

				bool doKerning = (*itr)["kerning"].asBool();
				font->setKerning(doKerning);

				auto search = userFonts.find(fontName);
				if (search == userFonts.end()){
					ofLogNotice("AppFonts") << "Loading User Font '" << fontName << "' from '" << fontFile << "'";
					userFonts[fontName] = font;
				}else{
					ofLogError("AppFonts") << "User Font with this name already exists! (" << fontName << ")";
					terminateApp();
				}

				float charSpacing = 0;
				if(!(*itr)["charSpacing"].isNull()){
					charSpacing = (*itr)["charSpacing"].asFloat();
					font->setCharacterSpacing(charSpacing);
					ofLogNotice("AppFonts") << "Setting custom Character Spacing for font \""<< fontName << "\" : " << charSpacing;
				}

			}
		}else{
			ofLogFatalError("AppFonts") << "User Fonts (\"Fonts/user\")is not a Json Object! Check you AppSettings.json file!";
			terminateApp();
		}
	}
}

void AppFonts::loadOfxAppFont(const string & key, ofxFontStash & font){

	ofxJsonSettings & settings = ofxJsonSettings::get();

	if(settings.exists("Fonts/ofxApp/" + key)){

		string fontFile = settings.getString("Fonts/ofxApp/" + key + "/fontFile");
		int atlasSize = settings.getInt("Fonts/ofxApp/" + key + "/atlasSize");
		float lineH = settings.getFloat("Fonts/ofxApp/" + key + "/lineHeight");
		bool mipmaps = settings.getBool("Fonts/ofxApp/" + key + "/mipmaps");
		int mipMapPadding = settings.getInt("Fonts/ofxApp/" + key + "/mipmapPadding");
		float retinaScale = settings.getFloat("Fonts/ofxApp/" + key + "/uiScale");

		assertFileExists(fontFile);

		ofLogNotice("AppFonts") << "Loading ofxApp Font'" << fontFile << "'";
		font.setup(	fontFile,
					lineH, 		//line height
					atlasSize, 		//tex atlas size
					mipmaps,		//mipmaps
					mipMapPadding, 	//atlas extra intra-char padding
					retinaScale 	//ui Scale (per-char texture upscale)
					);


	}else{
		ofLogFatalError("AppFonts") << "Missing required ofxApp font! Check you AppSettings.json file in \"Fonts/ofxApp/" << key << "\"";
		terminateApp();
	}
}
