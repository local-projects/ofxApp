//
//  ofxAppFonts.h
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#pragma once
#include "ofMain.h"
#include "ofxFontStash.h"
#include "ofxFontStash2.h"

class ofxAppFonts{

public:

	const std::string monoID = "mono";
	const std::string monoBoldID = "monoBold";
	const std::string regularID = "regular";
	const std::string boldID = "bold";

	void setup();

	static std::string getMonoFontPath(){return "ofxAppAssets/fonts/UbuntuMono-R.ttf";}
	static std::string getMonoBoldFontPath(){return "ofxAppAssets/fonts/UbuntuMono-B.ttf";}
	static std::string getRegularFontPath(){return "ofxAppAssets/fonts/Montserrat-Regular.ttf";}
	static std::string getBoldFontPath(){return "ofxAppAssets/fonts/Montserrat-Bold.ttf";}

	ofxFontStash& getMonoFont(){return monospaced;}
	ofxFontStash& getMonoBoldFont(){return monospacedBold;}
	ofxFontStash* getFont(const std::string& name);

	void reloadFontStash2Styles();

	ofxFontStash2::Fonts & getFontStash2(){return fonts2;}
	const ofxFontStash2::Style& getFontStyle(const std::string & styleID);

protected:

	void loadFontStashFonts();

	void loadFontStash2Fonts();
	void loadFontStash2Styles();

	void loadOfxAppFont(const std::string & file, ofxFontStash & font);

	// ofxFontStash
	ofxFontStash monospaced;
	ofxFontStash monospacedBold;
	std::map<std::string, ofxFontStash*> userFS_Fonts;

	// ofxFontStash2
	ofxFontStash2::Fonts fonts2;
	map<std::string, ofxFontStash2::Style> fs2Styles;

	NVGalign getAlignmentFromString(const std::string & str);

};
