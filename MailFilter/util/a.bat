@echo off
if not exist buildenv.cmd goto wrongdir

if exist abend.log del abend.log
if not exist s:\system\abend.log goto noabend
move s:\system\abend.log .
start /wait notepad .\abend.log
goto end

:noabend
echo No Abend?
goto end

:wrongdir
echo.
echo Wrong working Directory?
echo.
echo Looked for buildenv.cmd but didn't find it!
echo.
goto end

:end

