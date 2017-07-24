#!/usr/bin/env bash
set -e

echo "Executing ci/linux/install.sh"

export OF_ROOT=~/openFrameworks
export OF_ADDONS=$OF_ROOT/addons

ADDONS="local-projects/ofxFontStash local-projects/ofxTuio local-projects/ofxJSON local-projects/ofxSuperLog local-projects/ofxAutoTexture local-projects/ofxApp local-projects/ofxMTJsonParser local-projects/ofxSimpleHttp local-projects/ofxTagSystem local-projects/ofxStateMachine local-projects/ofxAssets local-projects/ofxThreadSafeLog local-projects/ofxProgressiveTextureLoad local-projects/ofxTexturedObject local-projects/ofxMullion local-projects/ofxJsonSettings local-projects/ofxTimeMeasurements local-projects/ofxRemoteUI local-projects/ofxHistoryPlot local-projects/ofxAnimatable local-projects/ofxScreenSetup local-projects/ofxSensu local-projects/ofxGoogleAnalytics"

cd $OF_ADDONS

for ADDON in $ADDONS
do
  echo "Cloning addon '$ADDON' to " `pwd`
  git clone --depth=1 --branch=$OF_BRANCH https://github.com/$ADDON.git
done

cd -