@echo OFF
setlocal EnableDelayedExpansion

if NOT "%1"=="" (
	if NOT "%2"=="" (
		::get the project root folder
		SET URHOPATH=%1
    	SET URHOBUILD=%2

		if exist "!URHOPATH!" (
			if NOT exist "!URHOBUILD!" (
				call:makeFolder !URHOBUILD!
				echo "***********************************"
			   	echo "made directory for build:"
			   	echo "     -"!URHOBUILD!
		    ) 
		    if exist "!URHOBUILD!" (
          		echo "***********************************"
          		echo "begun build process"
          		echo !URHOPATH!
          		if NOT "%3"=="" (
          			echo "build standard"
          			!URHOPATH!\cmake_mingw.bat !URHOBUILD! -DURHO3D_64BIT=1 -DURHO3D_OPENGL=1 -DURHO3D_LIB_TYPE=SHARED -DURHO3D_SAMPLES=1
          		) else (
          			echo "build with core-avx-i"
          			!URHOPATH!\cmake_mingw.bat !URHOBUILD! -DURHO3D_64BIT=1 -DURHO3D_OPENGL=1 -DURHO3D_LIB_TYPE=SHARED -DURHO3D_SAMPLES=1 -DURHO3D_DEPLOYMENT_TARGET=core-avx-i
          		)
          		cd !URHOBUILD!
      		)
    	) else (
      		echo invalid source path given: "!URHOPATH!"
    	)
  	) else (
    	echo second argument required, urho build path
  	)


) else (
	echo "***********************************"
	echo no arguments given, please provide:
	Echo      -urho source path
  	echo      -urho build path
  	echo "***********************************"
)

goto:EOF

:makeFolder
	if exist %~1 (
	  echo           -%~1 already exists
	) else (
	  mkdir %~1
	  echo           -%~1 created
	)
	GOTO:EOF