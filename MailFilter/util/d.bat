@echo off
if not exist buildenv.cmd goto wrongdir
if not exist util\n.bat goto nonbat
call util\n.bat 0 d
goto end

:nonbat
echo No UTIL\N.BAT ???
goto end

:wrongdir
echo.
echo Wrong working Directory?
echo.
echo Looked for buildenv.cmd but didn't find it!
echo.
goto end


:end
