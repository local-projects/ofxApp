//
//  ofxAppMacros.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesi√† on 3/8/16.
//
//

#pragma once


////////////////////////////////////////////////////////////////////////////////////////////////////

//this is all to achieve variable includes given a user specified macro name for the app
//http://stackoverflow.com/questions/32066204/construct-path-for-include-directive-with-macro
//http://stackoverflow.com/questions/1489932/how-to-concatenate-twice-with-the-c-preprocessor-and-expand-a-macro-as-in-arg
//this is to directly include your AppColorsBasic.h, AppHobalsBasic.h, AppFonts.h subclasses.

#define OFX_APP_IDENT(x) x
#define OFX_APP_XSTR(x) #x
#define OFX_APP_STR(x) OFX_APP_XSTR(x)
#define OFX_APP_INCLUDE(x,y) OFX_APP_STR(OFX_APP_IDENT(x)OFX_APP_IDENT(y))

//lots of indirection for this to work...
#define OFX_APP_PASTER(x,y) x ## y
#define OFX_APP_EVALUATOR(x,y)  OFX_APP_PASTER(x,y)
#define OFX_APP_CLASS_NAME(class) OFX_APP_EVALUATOR(OFX_APP_NAME,class)

#define OFX_COLORS_FILENAME Colors.h
#define OFX_GLOBALS_FILENAME Globals.h


#ifdef TARGET_WIN32
//#define FILE_ACCES_ICON "💾"
const string FILE_ACCES_ICON = "💾";
#else
//#define FILE_ACCES_ICON "💾"
const string FILE_ACCES_ICON = "💾";
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
