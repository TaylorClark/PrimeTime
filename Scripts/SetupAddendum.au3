#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.2.2.0
 Author:         myName

 Script Function:
	Template AutoIt script.

#ce ----------------------------------------------------------------------------
#include <File.au3>
#include <GuiConstants.au3>
#NoTrayIcon
AutoItSetOption( "MustDeclareVars", 1 )
AutoItSetOption( "RunErrorsFatal", 0 )

; Get the current path
Dim Const $SCRIPT_PATH = @ScriptDir & "\"

FileInstall( "firewall_pic.gif", $SCRIPT_PATH & "firewall_pic.gif", 1 )

; The setup source directory must be passed on the command line
If $CmdLine[0] <> 1  Then
	Exit
EndIf

Dim Const $SETUP_SRC_DIR = $CmdLine[1] & "\"

OverwriteCustomExe()
RenameUninstallFiles()
TestFirewall();

; Delete the example pop-up image
FileDelete( $SCRIPT_PATH & "firewall_pic.gif" );


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Overwrite the game executable with a customized one if it exists
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func OverwriteCustomExe()
	
	Dim Const $GAME_EXE_NAME = "PrimeTime.exe"

	; If there is a game executable in the source directory
	If NOT FileExists( $SETUP_SRC_DIR & $GAME_EXE_NAME ) Then
		;MsgBox( 1, $SETUP_SRC_DIR & $GAME_EXE_NAME, "No EXE Found" )
		Return		
	EndIf
	
	; Overwrite the game executable with the one from the setup directory
	FileMove( $SETUP_SRC_DIR & $GAME_EXE_NAME, $SCRIPT_PATH & $GAME_EXE_NAME, 1 )

EndFunc


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Swap the uninstall file names so that the custom uninstaller is run first
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func RenameUninstallFiles()
	
	Dim Const $CUSTOM_UINSTALL_NAME = "post_uninstall.exe"
	Dim Const $CUSTOM_UINSTALL_NAME_FINAL = "post_uninstall.dll"
	Dim Const $ORIG_UINSTALL_NAME = "Uninstall.exe"

	; Copy the original uninstaller to a temporary name
	Dim $tempFile = _TempFile( $SCRIPT_PATH )
	FileMove( $SCRIPT_PATH & $ORIG_UINSTALL_NAME, $tempFile, 1 )
	
	; Rename the custom uninstaller to the original name
	FileMove( $SCRIPT_PATH & $CUSTOM_UINSTALL_NAME, $SCRIPT_PATH & $ORIG_UINSTALL_NAME, 1 )
	
	; Rename the original uninstaller to the custom name
	FileMove( $tempFile, $SCRIPT_PATH & $CUSTOM_UINSTALL_NAME_FINAL, 1 )
	
EndFunc



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Run the game and updater in 'test firewall' mode
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
Func TestFirewall()
	
	; Display a message box
	Dim $hWindow = GuiCreate("Prime Time: Math Adventure", 600, 400, -1, -1, BitOR( $WS_POPUPWINDOW, $WS_CAPTION ), $WS_EX_TOPMOST )
	GuiSetIcon($SCRIPT_PATH & "\primetime.ico", 0)
	Dim $msgText = "In order to go online, some versions of Windows require you to manually allow a program access to the internet. To ensure that Prime Time and the auto-updater will work properly, please tell the Windows firewall to unblock both.";
	GUISetFont (12, 600, 0, "Arial")
	GuiCtrlCreateLabel($msgText, 10, 10, 580, 85 )
	Dim $closeButton = GUICtrlCreateButton( "Test Firewall Now", 205, 100, 190, 40 )
	GUISetFont (9, 400, 2, "Arial")
	GuiCtrlCreateLabel("The pop-up windows will look similar to this, please click the 'Unblock' button on both", 60, 155, 479, 15 )
	GuiCtrlCreatePic( $SCRIPT_PATH & "firewall_pic.gif", 90, 170, 419, 221, $WS_BORDER )
		
	GUISetState()
	
	; GUI MESSAGE LOOP
	GuiSetState()
	While 1
		Dim $msg = GuiGetMsg()
		if( $msg == $GUI_EVENT_CLOSE OR $msg == $closeButton ) Then
			ExitLoop
		EndIF
	WEnd
	
	GUIDelete( $hWindow );
	
	; Run Prime Time
	RunWait( $SCRIPT_PATH & "PrimeTime.exe /testFirewall /noupdate" )
	RunWait( $SCRIPT_PATH & "PrimeTimeUpdater.exe /testFirewall" )
EndFunc