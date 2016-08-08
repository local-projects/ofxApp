//
//  MyAppFonts.h
//
//  Created by Oriol Ferrer Mesià
//
//

#pragma once

#include "AppFontsBasic.h"

class MyAppFonts : public AppFontsBasic{

public:

	const string helNeuMedFile = "fonts/HelveticaNeueMedium.ttf";

	void setup(){

		AppFontsBasic::setup();
		//assertFileExists(helNeuMedFile);
		//helveticaMedium.setup(helNeuMedFile, 1.2, 512, false, 0, 1.0);
	};

public:

	ofxFontStash helveticaMedium;
};

