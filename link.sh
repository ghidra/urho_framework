#!/bin/bash

#i need to create a bin folder if it does not exists

#setup.sh /Urho3D_Source
#needs 1 arguents, the urho source folder

make_alias(){
  #$1 FOLDER $2 LINKEDFOLDER $3 NEWFOLDER
  if [ ! -e $3 ];then
    #link does not exist, we can make it
    ln -s $2 $3
    echo "          -"$1" directory LINKED"
  else
    echo "          -"$1" link ALREADY EXISTS"
  fi
}

URHOPATH=$1
URHOBUILD=$2
DRYRUN=1
#first make sure that the given folder is good

if [ $# -eq 0 ];then
  echo "***********************************"
  echo "no arguments given, please provide:"
  echo "     -urho source path"
  echo "     -urho build path"
  echo "***********************************"
else
  if [ -d $URHOPATH ];then
    if [ -d $URHOBUILD ];then

      # Absolute path this script is in, thus /home/user/bin
      SCRIPT=$(readlink -f "$0")
      SCRIPTPATH=$(dirname "$SCRIPT")
      #get the project path by going up two directories
      PROJECTPATH=$(basename $(dirname $SCRIPTPATH))

      if [ $DRYRUN -eq 1 ];then
        echo "***********************************"
        echo "we are dryrunning it"
        echo      "SCRIPT: "$SCRIPT
        echo      "SCRIPTPATH: "$SCRIPTPATH
        echo      "PROJECTPATH: "$PROJECTPATH

      else

        echo "***********************************"
        echo "linking folders from urho source"


        #link the data and core data folder
        echo "     -link CMake, CoreData and Data folders"
        make_alias "CMake" $URHOPATH"/CMake" $SCRIPTPATH"/CMake"
        make_alias "CoreData" $URHOPATH"/bin/CoreData" $SCRIPTPATH"/bin/CoreData"
        make_alias "Data" $URHOPATH"/bin/Data" $SCRIPTPATH"/bin/Data"

        echo "***********************************"
        echo "create launch editor script"

        EDIT=$URHOPATH"/bin/Urho3DPlayer /Scripts/Editor.as -pp "$SCRIPTPATH"/bin -p \"CoreData;Data;Resources\" -w -s"
        EFILE=$SCRIPTPATH/editor.sh
        if [ -f "$EFILE" ];then
          printf "$EDIT" > $EFILE
          echo "     -editor.sh edited"
        else
          touch $EFILE
          printf "$EDIT" > $EFILE
          echo "     -editor.sh created"
        fi
      fi

    else
      echo "***********************************"
      echo "invalid build path:"
      echo "     -source:" $URHOBUILD
      echo "***********************************"
    fi
  else
    echo "***********************************"
    echo "invalid source path:"
    echo "     -source:" $URHOPATH
    echo "***********************************"
  fi
fi