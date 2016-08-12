/*

 AppStatesBasic.h
 
 This defines basic states for a generic app;
 as your app will need more custom states, you can extend this
 state enum by:
 
 	1 - as with everything else in ofxApp, defining a pre-processor macro OFX_APP_NAME=myApp

 	2 - creating and a header file named myAppStates.h that contains something like:
		
 		/////// myAppStates.h /////////////
 		CUSTOM_LOAD_STATE,
 		CUSTOM_STATE_2,
 		CUSTOM_STATE_3,
 		//////////////////////////////////

 */

// down here the default basic states for an ofxApp

	SETTING_UP,
	LOADING_STATIC_TEXTURES,
	LOADING_CONTENT,
	LOADING_CONTENT_FAILED,
	SETUP_USER_APP, //in most cases, you will pick it up at this state and either go RUNNING or your Custom States
	RUNNING,
