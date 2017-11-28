//
//  ofxAppUtils.cpp
//  BaseApp
//
//  Created by Oriol Ferrer Mesià on 4/8/16.
//
//

#include "ofxAppUtils.h"
#include "ofxApp.h"
#include <regex>
#include "ofxAppErrorReporter.h"
#include "ofxThreadSafeLog.h"
#include "ofxSimpleHttp.h"

//#define STB_IMAGE_IMPLEMENTATION //note this is not required bc stb_image impl is already included in ofxFontStash2 (nvg)
#include "../lib/stb/stb_image.h"

namespace ofxApp{
namespace utils{

	void assertFileExists(const std::string & path){
		if(!ofFile::doesFileExist(path)){
			const std::string msg = "Required asset not present: '" + path + "'";
			ofLogFatalError("ofxApp") << msg;
			ofxApp::utils::terminateApp("ofxApp", msg);
		}else{
			//ofLogVerbose("ofxApp") << "Confirmed asset is present: '" << path << "'";
		}
	}

	void terminateApp(const std::string & module, const std::string & reason, float secondsOnScreen){
		
		ofLogFatalError("ofxApp") << "terminateApp()!";
		ofxSimpleHttp::destroySslContext();
		ofLogFatalError("ofxApp") << "";
		ofLogFatalError("ofxApp") << "-----------------------------------------------------------------------------------------------------------";
		ofLogFatalError("ofxApp") << "";
		ofLogFatalError("ofxApp") << "ofxApp is terminating because the module \"" << module << "\" found an unrecoverable error.";
		ofLogFatalError("ofxApp") << "\"" << reason << "\"";
		ofLogFatalError("ofxApp") << "This message will be on screen for " << (int)secondsOnScreen << " seconds, then the app will quit.";
		ofLogFatalError("ofxApp") << "";
		ofLogFatalError("ofxApp") << "-----------------------------------------------------------------------------------------------------------";
		ofLogFatalError("ofxApp") << "";
		ofxThreadSafeLog::one()->close();
		if(ofxApp::get().isWindowSetup()){
			ofxSuperLog::getLogger()->setScreenLoggingEnabled(true); //show log if json error
			ofxSuperLog::getLogger()->getDisplayLogger().setPanelWidth(1.0);
			int numFrames = secondsOnScreen * 1000 / 16; //stay up a bit so that you can read logs on screen
			
			OFXAPP_REPORT("ofxAppTerminate_" + module, reason, 2);
			
			//hijack OF and refresh screen & events by hand at ~60fps
			if(ofGetWindowPtr()){
				for(int i = 0; i < numFrames; i++ ){
					ofSetupScreen();
					ofClear(0,0,0,255);
					ofxSuperLog::getLogger()->getDisplayLogger().draw(ofGetWidth(), ofGetHeight());
					ofGetMainLoop()->pollEvents();
					if(ofGetWindowPtr()->getWindowShouldClose()){
						ofLogFatalError("ofxApp") << "Quitting by user action";
						std::exit(-1);
					}
					ofGetWindowPtr()->swapBuffers();
					ofSleepMillis(16);
				}
			}
		}else{
			ofLogFatalError("ofxApp") << "Terminating ofxApp before the app window is setup.";
		}
		std::exit(0);
	};

	ofColor createHsbColor(float h, float sat, float brightness){ //[0..255.0f]
		ofColor c;
		c.setHsb(h,sat,brightness);
		return c;
	}
	
	std::string secondsToHumanReadable(float secs, int decimalPrecision){
		std::string ret;
		if (secs < 60.0f ){ //if in seconds
			ret = ofToString(secs, decimalPrecision) + " seconds";
		}else{
			if (secs < 3600.0f){ //if in min range
				ret = ofToString(secs / 60.0f, decimalPrecision) + " minutes";
			}else{ //hours or days
				if (secs < 86400.0f){ // hours
					ret = ofToString(secs / 3600.0f, decimalPrecision) + " hours";
				}else{ //days
					if (secs < 86400.0f * 7.0f){ // days
						ret = ofToString(secs / (86400.0f), decimalPrecision) + " days";
					}else{ //days
						ret = ofToString(secs / (86400.0f * 7.0f) , decimalPrecision) + " weeks";
					}
				}
			}
		}
		return ret;
	}
	
	
	std::string bytesToHumanReadable(long long bytes, int decimalPrecision){
		std::string ret;
		if (bytes < 1024 ){ //if in bytes range
			ret = ofToString(bytes) + " bytes";
		}else{
			if (bytes < 1024 * 1024){ //if in kb range
				ret = ofToString(bytes / float(1024), decimalPrecision) + " KB";
			}else{
				if (bytes < (1024 * 1024 * 1024)){ //if in Mb range
					ret = ofToString(bytes / float(1024 * 1024), decimalPrecision) + " MB";
				}else{
					ret = ofToString(bytes / float(1024 * 1024 * 1024), decimalPrecision) + " GB";
				}
			}
		}
		return ret;
	}
	
	std::string getNewUUID(){
		static char alphabet[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
		std::string s;
		for(int i = 0; i < 8; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
		s += "-";
		for(int i = 0; i < 4; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
		s += "-4";
		for(int i = 0; i < 3; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
		s += "-a";
		for(int i = 0; i < 3; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
		s += "-";
		for(int i = 0; i < 12; i++) s += ofToString((char)alphabet[(int)floor(ofRandom(16))]);
		return s;
	}
	
	
	std::string getFileSystemSafeString(const std::string & input){
		static char invalidChars[] = {'?', '\\', '/', '*', '<', '>', '"', ';', ':', '#' };
		int howMany = sizeof(invalidChars) / sizeof(invalidChars[0]);
		char replacementChar = '_';
		std::string output = input;
		for(int i = 0; i < howMany; i++){
			std::replace( output.begin(), output.end(), invalidChars[i], replacementChar);
		}
		return output;
	}

	std::string getGlInfo(){
		int i;
		int ii[2];
		#define OFX_APP_GL_INFO1(t)		glGetIntegerv(t, &i);\
										ss << #t << ": " << i << endl;
		#define OFX_APP_GL_INFO2(t)		glGetIntegerv(t, &ii[0]);\
										ss << #t << ": " << ii[0] << " x " << ii[1] << endl;

		std::stringstream ss;
		ss << "GL_RENDERER: " << std::string((char*)glGetString(GL_RENDERER)) << endl;
		OFX_APP_GL_INFO1(GL_MAX_SAMPLES);
		OFX_APP_GL_INFO2(GL_MAX_VIEWPORT_DIMS);
		OFX_APP_GL_INFO1(GL_MAX_TEXTURE_SIZE);
		OFX_APP_GL_INFO1(GL_MAX_TEXTURE_IMAGE_UNITS);
		OFX_APP_GL_INFO1(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
		OFX_APP_GL_INFO1(GL_MAX_TEXTURE_IMAGE_UNITS);
		OFX_APP_GL_INFO1(GL_MAX_TEXTURE_COORDS);
		OFX_APP_GL_INFO1(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);
		OFX_APP_GL_INFO1(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
		OFX_APP_GL_INFO1(GL_MAX_VERTEX_ATTRIBS);
		OFX_APP_GL_INFO1(GL_MAX_VERTEX_UNIFORM_COMPONENTS);
		OFX_APP_GL_INFO1(GL_MAX_VARYING_FLOATS);
		#undef OFX_APP_GL_INFO1
		#undef OFX_APP_GL_INFO2
		return ss.str();
	}

	std::string getGlError(){
		std::string err;
		GLenum glErr = glGetError(); //https://www.opengl.org/wiki/GLAPI/glGetError

		switch (glErr) {
			case GL_INVALID_ENUM: err = "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE: err = "GL_INVALID_VALUE"; break;
			case GL_INVALID_OPERATION: err = "GL_INVALID_OPERATION"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: err = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
			case GL_OUT_OF_MEMORY: err = "GL_OUT_OF_MEMORY"; break;
			case GL_STACK_UNDERFLOW: err = "GL_STACK_UNDERFLOW"; break;
			case GL_STACK_OVERFLOW: err = "GL_STACK_OVERFLOW"; break;
		}
		return err;
	}


	bool loadTexture(ofTexture & tex, const std::string & path, bool mipmap, float bias, int anisotropy){
		bool ok = ofLoadImage(tex, path);
		if (ok){
			tex.generateMipmap();
			tex.enableMipmap();
			tex.setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			tex.bind();
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy); //TODO check for hw support!
			tex.unbind();
			ofLogError("ofxApp") << "Failed to load texture at \"" << path << "\"";
		}else{
			ofLogError("ofxApp") << "Failed to load texture at \"" << path << "\"";
		}
		return ok;
	}


	bool isValidEmail(const std::string email){
		// define a regular expression
		//http://stackoverflow.com/questions/46155/validate-email-address-in-javascript/46181#46181
		const std::regex pattern ("[^ ]*@[^ ]*\\.[^ ]*");
		//a more relaxed version: // [^ ]*@[^ ]*\\.[^ ]*
		//a more relaxed strict version: // ([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,4}(\\.[a-z]{2,4})?)


		// try to match the std::string with the regular expression
		return std::regex_match(email, pattern);
	}


	void logParagraph(const std::string & moduleName, ofLogLevel lev, const std::string & text){
		vector<std::string> jsonLines = ofSplitString(text, "\n");
		for (auto & l : jsonLines) {
			switch(lev){
				case OF_LOG_VERBOSE: ofLogVerbose(moduleName) << l; break;
				case OF_LOG_NOTICE: ofLogNotice(moduleName) << l; break;
				case OF_LOG_WARNING: ofLogWarning(moduleName) << l; break;
				case OF_LOG_ERROR: ofLogError(moduleName) << l; break;
				case OF_LOG_FATAL_ERROR: ofLogFatalError(moduleName) << l; break;
			}
		}
	}


	void logBanner(const std::string & log) {
		ofLogNotice("ofxApp") << "";
		#ifdef TARGET_WIN32
		ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////";
		ofLogNotice("ofxApp") << "// " << log;
		ofLogNotice("ofxApp") << "///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////";
		#else
		ofLogNotice("ofxApp") << "███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████";
		ofLogNotice("ofxApp") << "██ " << log;
		ofLogNotice("ofxApp") << "███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████";
		#endif
		ofLogNotice("ofxApp") << "";
	}


	ImageInfo getImageDimensions(const std::string & filePath){
		std::string path = ofToDataPath(filePath, true);
		ImageInfo info;
		int ret = stbi_info(path.c_str(), &info.width, &info.height, &info.nChannels);
		info.valid = (ret != 0);
		if(!info.valid){
			ofLogError("ofxApp::utils") << "getImageDimensions() failed for image \"" << filePath << "\"";
		}
		return info;
	}

	
	std::string toString(ofLogLevel l){
		switch(l){
			case OF_LOG_VERBOSE: return "OF_LOG_VERBOSE";
			case OF_LOG_NOTICE: return "OF_LOG_NOTICE";
			case OF_LOG_WARNING: return "OF_LOG_WARNING";
			case OF_LOG_ERROR: return "OF_LOG_ERROR";
			case OF_LOG_FATAL_ERROR: return "OF_LOG_FATAL_ERROR";
			case OF_LOG_SILENT: return "OF_LOG_SILENT";
		}
		ofLogError() << "Unknown Log Level! '" << (int)l << "'";
		return "UNKNOWN_OF_LOG_LEVEL";
	}

//namespaces
}}
