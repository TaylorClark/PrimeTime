#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.2.2.0
 Author:         myName

 Script Function:
	Template AutoIt script.

#ce ----------------------------------------------------------------------------
#include <File.au3>
#NoTrayIcon
AutoItSetOption( "MustDeclareVars", 1 )

Dim Const $ORIG_FILE_NAME = "Uninstall.exe"
Dim Const $PROFILE_DIR = "Profiles"


; Get the current path
Dim Const $SCRIPT_PATH = @ScriptDir & "\"

; Pause to ensure the previous application is finished
Sleep( 125 )
		
; Determine if this script should continue
If NOT ShouldRunUninstall() Then
	Exit
EndIf

; If there are profiles
If DoProfilesExist() Then
	; Confirm that the user wants the profiles removed
	Dim $msgBoxType = BitOR( 4, 32 )
	Dim $msgBoxRetVal = MsgBox( $msgBoxType, "Remove Player Profiles?", "Are you sure you want to remove your player profiles? If you ever decide to reinstall Prime Time then you can pick up where you left off by selecting 'No'. If you press 'Yes' your player profiles will be deleted." )
	If $msgBoxRetVal == 6 Then
		RemoveProfiles()
	EndIf
EndIf

; Remove the directory (This call will do nothing if the directory is not empty)
DirRemove( $SCRIPT_PATH & $PROFILE_DIR, 0 )
	
; Ensure the updater is not running and remove the DLUpdates folder
CloseUpdater()
DirRemove( $SCRIPT_PATH & "DLUpdates\", 1 )

RemoveDumpsAndLogs()

RunPostUninstaller()


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Remove the player profile files. If the directory is empty once the profiles are deleted then
; the directory will be deleted as well.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func RemoveProfiles()
	; Get the profile directory search string
	Dim $profileDir = $SCRIPT_PATH & $PROFILE_DIR & "\"
	Dim $profileDirSearch = $profileDir & "*.ptp"
	
	; Open the search handle for profile files
	Dim $hSearch = FileFindFirstFile( $profileDirSearch )
	if $hSearch <> -1 Then
		; Loop through all of the profiles and delete them
		While 1
			Dim $curProfile = $profileDir & FileFindNextFile($hSearch) 
				If @error Then
					ExitLoop
				EndIf
			
			FileDelete( $curProfile )
			
		WEnd

		; Close the file search handle
		FileClose( $hSearch )
	EndIf

EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Remove the Prime Time logs and any crash dumps that are still in the game directory.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func RemoveDumpsAndLogs()
	
	Dim Const $DUMP_PRE_FILE_NAME = "PTErrorReport_"
	Dim Const $LOG_PRE_FILE_NAME = "PTLog_"

	; Setup the paths to find logs and dumps
	Dim $searchPaths[2]
	$searchPaths[0] = $SCRIPT_PATH & $DUMP_PRE_FILE_NAME & "*.dmp"
	$searchPaths[1] = $SCRIPT_PATH & $LOG_PRE_FILE_NAME & "*.log"
	
	; Remove files
	For $curSearchPath In $searchPaths
		; Open the search handle 
		Dim $hSearch = FileFindFirstFile( $curSearchPath )
		if( $hSearch == -1 ) Then
			ContinueLoop
		EndIf
		
		; Loop through all of the profiles and delete them
		While 1
			Dim $curFile = $SCRIPT_PATH & FileFindNextFile($hSearch) 
				If @error Then
					ExitLoop
				EndIf
			
			FileDelete( $curFile )
			
		WEnd

		; Close the file search handle
		FileClose( $hSearch )
	Next

EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Run the uninstaller that removes the rest of the game data. This function starts the
; uninstaller, waits for it to prompt the user to confirm uninstallation, then simulates a key
; press to auto-press the 'OK' button.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func RunPostUninstaller()
	
	; One last safety check
	If @ScriptName == $ORIG_FILE_NAME Then
		Return
	EndIf
	
	; Run the real uninstaller
	Run( $ORIG_FILE_NAME )
	
	Dim $winTitle = "Uninstall"
	Dim $winText = "This program will uninstall Prime Time: Math"
	
	; Wait for the window to display
	if WinWait( $winTitle, $winText, 20 ) == 0 Then
		Return
	EndIf
	
	; Press the OK button
	WinActivate( $winTitle, $winText )
	ControlClick( $winTitle, $winText, "OK" )
	Exit;
	
EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Test if there are any profiles to remove
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func DoProfilesExist()
	
	; Get the profile directory search string
	Dim $profileDir = $SCRIPT_PATH & $PROFILE_DIR & "\*.ptp"
	
	; Get a search handle
	Dim $hSearch = FileFindFirstFile( $profileDir )
	
	; If the search handle is invalid then there are no profiles
	if( $hSearch == -1 ) Then
		Return False
	EndIf
	
	; The search handle is valid so return that there are profiles
	FileClose( $hSearch )
	Return True
	
EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Test if this script should continue the uninstall
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func ShouldRunUninstall()

	; Ensure this is the proper directory
	;If NOT IsPrimeTimeDir() Then
	;	MsgBox( 16, "Prime Time: Math Adventure", "The directory from which this uninstaller is being run is not the Prime Time game directory. If you think this message is wrong then you may need to reinstall Prime Time for the uninstaller to work (Weird, huh? This is a precautionary measure to ensure that no files become corrupt). We apologize for the inconvenience and feel free to visit the website to contact support if needed." )
	;	Return False
	;EndIf
	
	; If the current file name is not the original name
	if( @ScriptName <> "Uninstall.exe" ) Then
		
		; If the current file name is the goal file name
		Dim Const $GOAL_FILE_NAME = "post_uninstall.exe"
		if( @ScriptName == $GOAL_FILE_NAME ) Then
			
			; Remove the temporary file which was passed as a command line parameter
			if( $CmdLine[0] > 0 ) Then
				FileDelete( $CmdLine[1] )
			EndIf
			
			; Continue on
			Return True
			
		EndIf
		
		; This must be the intermediate execution so the first step is to rename the 'hidden'
		; uninstaller to a temporary file
		Dim $tempFile = _TempFile( $SCRIPT_PATH )
		FileMove( $SCRIPT_PATH & "post_uninstall.dll", $tempFile, 1 )
		
		; Rename the original uninstaller to the old name
		FileMove( $SCRIPT_PATH & $ORIG_FILE_NAME, $SCRIPT_PATH & $GOAL_FILE_NAME, 1 )
		
		; Rename the temporary file to the goal name
		FileMove( $tempFile, $SCRIPT_PATH & $ORIG_FILE_NAME, 1 )
		
		; Execute the old file and bail
		Run( $SCRIPT_PATH & $GOAL_FILE_NAME & ' "' & @ScriptFullPath &'"'  )
		
		Return False
	EndIf
	
	; Confirm the user wants to uninstall Prime Time
	Dim $msgBoxType = BitOR( 4, 32 )
	Dim $msgBoxRetVal = MsgBox( $msgBoxType, "Uninstall Prime Time?", "Are you sure you want to uninstall Prime Time: Math Adventure?" )
	
	; If the user pressed no
	If $msgBoxRetVal == 7 Then
		Return False
	EndIf
	
	; The script is the original name so first copy it to a temporary intermediate file
	Dim $tempFile = _TempFile( $SCRIPT_PATH, "~", ".exe" )
	FileCopy( @ScriptFullPath, $tempFile, 1 )
	
	; Run the copy and bail
	Run( $tempFile )
	Return False
EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Test if the current directory is the Prime Time folder
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func IsPrimeTimeDir()

	Dim $neededFiles[5]
	$neededFiles[0] = "PrimeTime.exe"
	$neededFiles[1] = "numbers.rdb"
	$neededFiles[2] = "fmod.dll"
	$neededFiles[3] = "PrimeTime.dll"
	$neededFiles[4] = "post_uninstall.exe"
	
	For $curFile In $neededFiles
	
		Dim $curFilePath = $SCRIPT_PATH & $curFile
		
		If NOT FileExists( $curFilePath ) Then
			Return False
		EndIf
		
	Next
	
	Return True
	
EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Test if the updater is running and, if so, force close it
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func CloseUpdater()
	
	If( ProcessExists( "PrimeTimeUpdater.exe" ) ) Then
		ProcessClose( "PrimeTimeUpdater.exe" );
	EndIf
	
EndFunc