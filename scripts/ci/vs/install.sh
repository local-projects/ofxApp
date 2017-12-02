#!/usr/bin/env bash
set -e

echo "Executing ci/vs/install.sh"

export OF_ROOT="$APPVEYOR_BUILD_FOLDER/../openFrameworks"
export OF_ADDONS=$OF_ROOT/addons

pwd;
echo "OF_ADDONS: $OF_ADDONS"

ADDONS="local-projects/ofxFontStash local-projects/ofxFontStash2 local-projects/ofxTuio local-projects/ofxJSON local-projects/ofxSuperLog local-projects/ofxAutoTexture local-projects/ofxMTJsonParser local-projects/ofxSimpleHttp local-projects/ofxTagSystem local-projects/ofxStateMachine local-projects/ofxAssets local-projects/ofxThreadSafeLog local-projects/ofxProgressiveTextureLoad local-projects/ofxTexturedObject local-projects/ofxMullion local-projects/ofxJsonSettings local-projects/ofxTimeMeasurements local-projects/ofxRemoteUI local-projects/ofxHistoryPlot local-projects/ofxAnimatable local-projects/ofxScreenSetup local-projects/ofxSensu local-projects/ofxGoogleAnalytics local-projects/ofxInterface"


cd $OF_ADDONS

for ADDON in $ADDONS
do
  echo "Cloning addon '$ADDON' to " `pwd`
  git clone --depth=1  https://github.com/$ADDON.git
done

cd -