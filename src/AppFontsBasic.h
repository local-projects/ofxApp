//
//  AppFontsBasic.h
//
//  Created by Oriol Ferrer Mesià aug/2016
//
//

#pragma once
#include "ofMain.h"
#include "ofxFontStash.h"
#include "AppBaseClasses.h"

class AppFontsAbstract : public HasAssets{
public:
	virtual void setup() = 0;
};


class AppFontsBasic : public AppFontsAbstract {

public:

	virtual void setup(){

		bool mipmaps = false;
		int padding = 0;
		float retinaScale = 1.0;

		assertFileExists(veraMonoFile);
		assertFileExists(veraMonoBoldFile);

		veraMono.setup(veraMonoFile,
					   1.4, 		//line height
					   512, 		//tex atlas size
					   mipmaps,		//mipmaps
					   padding, 	//atlas extra intra-char padding
					   retinaScale 	//ui Scale (per-char texture upscale)
					   );

		veraMonoBold.setup(veraMonoBoldFile,
						   1.4, 		//line height
						   512, 		//tex atlas size
						   mipmaps,		//mipmaps
						   padding, 	//atlas extra intra-char padding
						   retinaScale 	//ui Scale (per-char texture upscale)
						   );

	}

	ofxFontStash veraMono;
	ofxFontStash veraMonoBold;

protected:

	const string veraMonoFile = "fonts/VeraMono.ttf";
	const string veraMonoBoldFile = "fonts/VeraMono-Bold.ttf";

};