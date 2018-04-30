//
//  ofxAppMacros.h
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 3/8/16.
//
//

#pragma once


////////////////////////////////////////////////////////////////////////////////////////////////////

//this is all to achieve variable includes given a user specified macro name for the app
//http://stackoverflow.com/questions/32066204/construct-path-for-include-directive-with-macro
//http://stackoverflow.com/questions/1489932/how-to-concatenate-twice-with-the-c-preprocessor-and-expand-a-macro-as-in-arg
//this is to directly include your AppGlobalsBasic.h subclasses.

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

////////////////////////////////////////////////////////////////////////////////////////////////////

#define GLOB											ofxApp::get().globals()/*(*ofxApp_globals)*/

#define G_TEX(name)										ofxApp::get().textures().getTexture(name)

#define G_FONT(name)									ofxApp::get().fonts().getFont(name)
#define G_FONT_MONO()									ofxApp::get().fonts().getMonoFont()
#define G_FONT_MONO_BOLD()								ofxApp::get().fonts().getMonoBoldFont()

#define G_FS2()											ofxApp::get().fonts().getFontStash2()
#define G_FSTYLE(S)										ofxApp::get().fonts().getFontStyle(S)


#define OFXAPP_REPORT(alertID,msg,severity)						ofxApp::get().errorReporter().send(alertID,msg,severity)
#define OFXAPP_REPORT_FILE(alertID,msg,severity,fileToSend)		ofxApp::get().errorReporter().send(alertID,msg,severity,fileToSend)

#define OFXAPP_ANALYTICS()										ofxApp::get().analytics()

// ofxApp debug logging
#define	OFXAPP_LOG(l)											ofxApp::get().addToScreenLog(l)
#define	OFXAPP_LOG_CLEAR(l)										ofxApp::get().clearScreenLog()
#define	OFXAPP_LOG_FRAME(l)										ofxApp::get().addToCurrentFrameLog(l)

// Logging
//for in-class methods only - will throw compiler error in static methods or classless functions
#define LOGV 										ofLogVerbose(SUPERLOG_TYPE_NAME)
#define LOGN 										ofLogNotice(SUPERLOG_TYPE_NAME)
#define LOGW 										ofLogWarning(SUPERLOG_TYPE_NAME)
#define LOGE 										ofLogError(SUPERLOG_TYPE_NAME)
#define LOGF 										ofLogFatalError(SUPERLOG_TYPE_NAME)

#define LOGFV 										ofLogVerbose(SUPERLOG_TYPE_NAME_WITH_FUNC)
#define LOGFN 										ofLogNotice(SUPERLOG_TYPE_NAME_WITH_FUNC)
#define LOGFW 										ofLogWarning(SUPERLOG_TYPE_NAME_WITH_FUNC)
#define LOGFE 										ofLogError(SUPERLOG_TYPE_NAME_WITH_FUNC)
#define LOGFF 										ofLogFatalError(SUPERLOG_TYPE_NAME_WITH_FUNC)

#define LOGFLV 										ofVerbose(SUPERLOG_TYPE_NAME_WITH_FUNC_AND_LINE)
#define LOGFLN 										ofLogNotice(SUPERLOG_TYPE_NAME_WITH_FUNC_AND_LINE)
#define LOGFLW 										ofLogWarning(SUPERLOG_TYPE_NAME_WITH_FUNC_AND_LINE)
#define LOGFLE 										ofLogError(SUPERLOG_TYPE_NAME_WITH_FUNC_AND_LINE)
#define LOGFLF 										ofLogFatalError(SUPERLOG_TYPE_NAME_WITH_FUNC_AND_LINE)
