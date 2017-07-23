@ECHO OFF

REM Remove the setup addendum executable
:Repeat
del "SetupAddendum.exe"
if exist "SetupAddendum.exe" goto Repeat

REM Remove this file
del "CompleteSetup.bat"