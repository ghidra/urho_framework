#!/bin/bash

# needs 2 arguments
# argument 1: Urho3D source
# argument 2: Urho3D build folder
# argument 3 (optional): triggers a dry run, and doesnt do anything

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

make_folder(){
  #$1 FOLDER
  if [ ! -e $1 ];then
    mkdir $1
    echo "          -"$1" directory CREATED"
  else
    echo "          -"$1" directory ALREADY EXISTS"
  fi
}

URHOPATH=`cd "$1"; pwd`
URHOBUILD=`cd "$2"; pwd`
DRYRUN=$3

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
      up1="$(dirname "$SCRIPTPATH")"
      PROJECTPATH="$(dirname "$up1")"

      #if [ $DRYRUN -eq 1 ];then
      if [ ! -z "$DRYRUN" ];then
        echo "***********************************"
        echo "we are dryrunning it"
        echo      "SCRIPT: "$SCRIPT
        echo      "SCRIPTPATH: "$SCRIPTPATH
        echo      "PROJECTPATH: "$PROJECTPATH
        echo      "SOURCEPATH: "$URHOPATH
        echo      "BUILDPATH: "$URHOBUILD
      fi

      echo "***********************************"
      echo "make bin folder"
      if [ ! -z "$DRYRUN" ];then
        echo "     -DRYRUN"
        echo "     "$PROJECTPATH"/bin"
      else
        make_folder $PROJECTPATH"/bin"
      fi

      echo "***********************************"
      echo "make bin/Resources folder"
      if [ ! -z "$DRYRUN" ];then
        echo "     -DRYRUN"
        echo "     "$PROJECTPATH"/bin/Resources"
      else
        make_folder $PROJECTPATH"/bin/Resources"
      fi

      echo "***********************************"
      echo "linking folders from urho source"
      echo "     -link CMake, CoreData and Data folders"
      if [ ! -z "$DRYRUN" ];then
        echo "     -DRYRUN"
        echo "          CMake"
        echo "          "$URHOPATH"/CMake"
        echo "          "$PROJECTPATH"/CMake"
        echo "          CoreData"
        echo "          "$URHOPATH"/bin/CoreData"
        echo "          "$PROJECTPATH"/bin/CoreData"
        echo "          Data"
        echo "          "$URHOPATH"/bin/Data"
        echo "          "$PROJECTPATH"/bin/Data"
      else
        make_alias "CMake" $URHOPATH"/CMake" $PROJECTPATH"/CMake"
        make_alias "CoreData" $URHOPATH"/bin/CoreData" $PROJECTPATH"/bin/CoreData"
        make_alias "Data" $URHOPATH"/bin/Data" $PROJECTPATH"/bin/Data"
      fi

      echo "***********************************"
      echo "copy CMakeLists.txt"

      if [ ! -f $PROJECTPATH"/CMakeLists.txt" ];then
        if [ ! -z "$DRYRUN" ];then
          echo "     CMakeLists.txt will need to be copied"
        else
          cp $SCRIPTPATH"/CMakeLists.txt" $PROJECTPATH"/CMakeLists.txt"
          echo "     CMakeLists.txt created"
          echo "          -!!!! MANUALLY UPDATE PROJECT AND TARGET NAME !!!!"
        fi
      else
        echo "     CMakeLists.txt already exists"
      fi

      echo "***********************************"
      echo "create launch editor script"

      EDIT=$URHOBUILD"/bin/Urho3DPlayer /Scripts/Editor.as -pp "$PROJECTPATH"/bin -p \"CoreData;Data;Resources\" -w -s"
      EFILE=$PROJECTPATH/editor.sh
      if [ ! -z "$DRYRUN" ];then
        echo "-DRYRUN"
        echo $EDIT
      else
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