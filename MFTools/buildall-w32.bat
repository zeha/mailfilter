@echo off
call "e:\Programme\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"
devenv ..\MFFilterICE\MFFilterICE.sln /rebuild Release
pause
devenv ..\MFFilterICE.GUI\FilterEdit.sln /rebuild Release
pause
devenv ..\MFInstWizard\MFInstWizard.sln /rebuild Release
pause
