@echo OFF
setlocal EnableDelayedExpansion EnableExtensions
SET DRYRUN=%4

if NOT "%1"=="" (
	if NOT "%2"=="" (
		::get the project root folder
		SET URHOPATH=%1
		SET URHOBUILD=%2
		SET OVERWRITE=%3

		if exist "!URHOPATH!" (
			if exist "!URHOBUILD!" (

				for %%f in ("%~dp0\..\..\") do (
					call :resolve PROJECTPATH %%~ff
				)

				if NOT "!DRYRUN!"=="" (
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
				if NOT "!DRYRUN!"=="" (
					echo 	-DRYRUN
					echo 	"!PROJECTPATH!bin"
				) else (
					call:makeFolder "bin" "!PROJECTPATH!"
				)
				echo ***********************************
				echo -make bin\Resources folder
				if NOT "!DRYRUN!"=="" (
					echo 	-DRYRUN
					echo 	"!PROJECTPATH!bin\Resources"
				) else (
					call:makeFolder "Resources" "!PROJECTPATH!bin\"
				)

				echo ***********************************
				echo linking folders from urho source
				echo 	-link CMake, CoreData and Data folders
				if NOT "!DRYRUN!"=="" (
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
					call:makeAlias "CoreData" "!URHOPATH!\bin\CoreData" "!PROJECTPATH!bin\CoreData" "!OVERWRITE!"
					call:makeAlias "Data" "!URHOPATH!\bin\Data" "!PROJECTPATH!bin\Data" "!OVERWRITE!"
					call:makeAlias "CMake" "!URHOPATH!\CMake" "!PROJECTPATH!CMake" "!OVERWRITE!"
				)
				echo ***********************************
				echo -copy dll file
				if NOT "!DRYRUN!"=="" (
					echo 	-DRYRUN
					echo 	"!URHOBUILD!\bin\Urho3D.dll !PROJECTPATH!bin\Urho3D.dll"
				) else (
					call:copyFile "Urho3D.dll" "!URHOBUILD!\bin\Urho3D.dll" "!PROJECTPATH!bin\Urho3D.dll" "!OVERWRITE!"
				)
				echo ***********************************
				echo copy CMakeLists.txt
				if exist "!PROJECTPATH!\CMakeLists.txt" (
					echo           "-CMakeLists.txt already exists"
				) else (
					if NOT "!DRYRUN!"=="" (
						echo "CMakeLists.txt will need to be copied"
					) else (
						echo      "CMakeLists.txt created"
						echo            "-!!!! MANUALLY UPDATE PROJECT AND TARGET NAME !!!!"
						call:copyFile "CmakeLists.txt" "%~dp0\CMakeLists.txt" "!PROJECTPATH!\CMakeLists.txt" "!OVERWRITE!"
					)
				)

				echo ***********************************
				echo make cmakebat file
				SET CMAKEBAT=cmake . -G "MinGW Makefiles"
				if exist "!PROJECTPATH!\cmakebat.bat" (
					echo           "-cmakebat.bat already exists"
				) else (
					if NOT "!DRYRUN!"=="" (
						echo "cmakebat.bat will be created"
					) else (
						echo      "cmakebat.bat created"
						echo. 2>!PROJECTPATH!cmakebat.bat
						echo !CMAKEBAT! > !PROJECTPATH!cmakebat.bat
					)
				)

				echo ***********************************
				echo editor script
				SET EDITOR=!URHOBUILD!\bin\Urho3DPlayer.exe \Scripts\Editor.as -pp !PROJECTPATH!bin -p "CoreData;Data;Resources"
				if NOT "!DRYRUN!"=="" (
					echo 	-DRYRUN
					echo 	!EDITOR!
				) else (
					if exist "!PROJECTPATH!\editor.bat" (
						if 1==0 (
							echo !EDITOR! > !PROJECTPATH!editor.bat
							echo           -editor.bat edited
						) else (
							echo           "-editor.bat already exists"
						)
					) else (
					  echo. 2>!PROJECTPATH!editor.bat
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
		if %~4 == 1 (
			call:removeLinkedFolder %~3
			mklink /J %~3 %~2
			echo                -%~1 but we replaced it anyway
		)
	) else (
	  mklink /J %~3 %~2
	)
	GOTO:EOF

:makeAliasFile
	if exist %~3 (
	  echo           -%~1 already exists
	) else (
	  mklink %~3 %~2
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

:copyFile
	if exist %~3 (
		echo           -%~1 already exists
		if %~4 == 1 (
			call:removeLinkedFile %~3
			 xcopy /s %~2 %~3*
			echo                -%~1 but we replaced it anyway
		)
	) else (
	  xcopy /s %~2 %~3*
	)
	GOTO:EOF