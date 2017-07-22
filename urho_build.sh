#!/bin/bash

# needs 2 arguments
# argument 1: Urho3D source
# argument 2: Urho3D build folder
# argument 3 (optional): build with core-avx-i

URHOPATH=$1
URHOBUILD=$2
#first make sure that the given folder is good

if [ $# -eq 0 ];then
	echo "***********************************"
	echo "no arguments given, please provide:"
	echo "     -urho source path"
	echo "     -urho build path"
	echo "***********************************"
else
	if [ -z "$2" ];then
		echo "***********************************"
		echo "please provide build path"
	else
		if [ ! -d $URHOBUILD ]; then
			echo "***********************************"
			   echo "made directory for build:"
			   echo "     -"$URHOBUILD
			mkdir $URHOBUILD
		fi
		if [ -d $URHOPATH ];then
			echo "***********************************"
			echo "begun build process"
			if [ -z "$3" ];then
				echo "standard build"
				bash $URHOPATH/cmake_generic.sh $URHOBUILD -DURHO3D_64BIT=1 -DURHO3D_OPENGL=1 -DURHO3D_LIB_TYPE=SHARED -DURHO3D_DEBUG=1 -DURHO3D_SAMPLES=1 -DVIDEO_MIR=0 
			else
				echo "build with core-avx-i"
				bash $URHOPATH/cmake_generic.sh $URHOBUILD -DURHO3D_64BIT=1 -DURHO3D_OPENGL=1 -DURHO3D_LIB_TYPE=SHARED -DURHO3D_SAMPLES=1 -DURHO3D_DEPLOYMENT_TARGET=generic -DURHO3D_SSE=1
			fi
			cd $URHOBUILD
			make
		else
			if [ ! -d $URHOPATH ];then
				echo "***********************************"
				echo "     -urho source path invalid"
			fi
		fi
	 fi
fi
