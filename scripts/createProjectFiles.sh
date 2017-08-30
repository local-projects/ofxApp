#/bin/bash

#this script will create the project files for an ofxApp based project
#run this script from the repo root!
#usage script.sh path_to_OF path_to_new_project


echo "This Script will generate Project Files for an ofxApp based project"
echo "Path to OpenFrameworks: \"$1\""
export PG_OF_PATH="$1"

pgBin="/Volumes/LP/LP/REPOS/lp-internal.OF_Project_template/projectGenerator.app/Contents/Resources/app/app/projectGenerator"

#CD into the script dir
#pushd "$(dirname "$0")" > /dev/null

echo "\n"

addonList=`awk '{print $1}' < ExternalAddons/ofxApp/addons.make | paste -s -d, -`
echo "AddonList: | $addonList |"

echo "\n"


eval $pgBin -p"osx,vs" -a"$addonList" --ofPath"$1" "$2"


#popd > /dev/null