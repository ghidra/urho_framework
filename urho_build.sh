#!/bin/bash
#does the mundane task of running my favorite build commands to build urho

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
			sh $URHOPATH/cmake_generic.sh $URHOBUILD -DURHO3D_64BIT=1 -DURHO3D_OPENGL=1 -DURHO3D_LIB_TYPE=SHARED -DURHO3D_SAMPLES=1 
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