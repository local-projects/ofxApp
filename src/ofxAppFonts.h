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

	const string monoID = "mono";
	const string monoBoldID = "monoBold";
	const string regularID = "regular";
	const string boldID = "bold";

	void setup();

	static string getMonoFontPath(){return "ofxAppAssets/fonts/UbuntuMono-R.ttf";}
	static string getMonoBoldFontPath(){return "ofxAppAssets/fonts/UbuntuMono-B.ttf";}
	static string getRegularFontPath(){return "ofxAppAssets/fonts/Montserrat-Bold.ttf";}
	static string getBoldFontPath(){return "ofxAppAssets/fonts/Montserrat-Regular.ttf";}

	ofxFontStash& getMonoFont(){return monospaced;}
	ofxFontStash& getMonoBoldFont(){return monospacedBold;}
	ofxFontStash* getFont(const string& name);

	ofxFontStash2& getFontStash2(){return fonts2;}
	const ofxFontStashStyle& getFontStyle(const string & styleID);

protected:

	void loadFontStashFonts();

	void loadFontStash2Fonts();
	void loadFontStash2Styles();

	void loadOfxAppFont(const string & file, ofxFontStash & font);

	// ofxFontStash
	ofxFontStash monospaced;
	ofxFontStash monospacedBold;
	std::map<string, ofxFontStash*> userFS_Fonts;

	// ofxFontStash2
	ofxFontStash2 fonts2;
	map<string, ofxFontStashStyle> fs2Styles;

	NVGalign getAlignmentFromString(const string & str);

};
