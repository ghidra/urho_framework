@echo OFF
setlocal EnableDelayedExpansion
SET DRYRUN=0

if NOT "%1"=="" (
	if NOT "%2"=="" (
		::get the project root folder
		SET URHOPATH=%1
    	SET URHOBUILD=%2

		
		if exist "!URHOPATH!" (
			if exist "!URHOBUILD!" (

				for %%f in ("%~dp0\..\..\") do (
					call :resolve PROJECTPATH %%~ff
				)

				if "%DRYRUN%" == "1" (
					echo ***********************************
					echo -some debug information
			      	echo 	Launched: "%~dp0"
			      	echo 	Current: "%CD%"
			      	echo 	Source: "%1"
			      	echo 	Build: "%2"
			      	echo 	Project: "!PROJECTPATH!"
			    )
			    echo ***********************************
			    echo -make bin folder
			    if "%DRYRUN%" == "1" (
			    	echo 	-DRYRUN
			    	echo 	"!PROJECTPATH!bin"
			    ) else (
			    	call:makeFolder "bin" "!PROJECTPATH!"
			    )
			    echo ***********************************
		    	echo linking folders from urho source
		    	echo 	-link CMake, CoreData and Data folders
				if "%DRYRUN%" == "1" (
					echo 	-DRYRUN
					echo 	"CoreData"
					echo 		"!URHOPATH!\bin\CoreData"
					echo 		"!PROJECTPATH!bin\CoreData" 
	        		echo 	"Data"
	        		echo 		"!URHOPATH!\bin\Data"
	        		echo 		"!PROJECTPATH!bin\Data" 
	        		echo 	"CMake" 
	        		echo 		"!URHOPATH!\CMake"
	        		echo 		"!PROJECTPATH!CMake"
        		) else (
	        		call:makeAlias "CoreData" "!URHOPATH!\bin\CoreData" "!PROJECTPATH!bin\CoreData"
        			call:makeAlias "Data" "!URHOPATH!\bin\Data" "!PROJECTPATH!bin\Data"
        			call:makeAlias "CMake" "!URHOPATH!\CMake" "!PROJECTPATH!CMake"
        		)
        		echo ***********************************
    			echo editor script
    			SET EDITOR=!URHOBUILD!\bin\Urho3DPlayer.exe \Scripts\Editor.as -pp !PROJECTPATH!\bin -p "CoreData;Data;Resources"
		        if "%DRYRUN%" == "1" (
		        	echo 	-DRYRUN
		        	echo 	!EDITOR!
		        ) else (
			        if exist "editor.bat" (
			          echo !EDITOR! > !PROJECTPATH!editor.bat
			          echo           -editor.bat edited
			        ) else (
			          echo. 2>editor.bat
			          echo !EDITOR! > !PROJECTPATH!editor.bat
			          echo           -editor.bat created
			        )
		        )

		    
		    ) else (
          		echo invalid build path given "%URHOBUILD%"
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


::FUNCTIONS
:resolve
	SETLOCAL
	ENDLOCAL & SET %~1=%~dp2
	GOTO:EOF

:makeAlias
	if exist %~3 (
	  echo           -%~1 already exists
	) else (
	  mklink /J %~3 %~2
	)
	GOTO:EOF

:makeAliasFile
	if exist %~3 (
	  echo           -%~1 already exists
	) else (
	  mklink /H %~3 %~2
	)
	GOTO:EOF

:makeFolder
	if exist %~2%~1 (
	  echo           -%~1 already exists
	) else (
	  mkdir %~2%~1
	  echo           -%~1 created
	)
	GOTO:EOF

:removeLinkedFolder
	if exist %~2 (
	  rmdir %~2
	  echo           -%~2 removed
	) else (
	  echo           -%~1 does not exist
	)
	GOTO:EOF

:removeLinkedFile
	if exist %~2 (
	  del %~2
	  echo           -%~2 removed
	) else (
	  echo           -%~1 does not exist
	)
	GOTO:EOF