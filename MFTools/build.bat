@echo off
echo Building: %2 in %1
echo     Parameters: %3 %4 %5 %6 %7 %8 %9
setlocal
cd %1
REM 
start /wait e:\Programme\Metrowerks\CodeWarrior8\bin\cmdide.exe %3 %4 %5 %6 %7 %8 %9 /r /b /q /c /s %2
set ERR=%ERRORLEVEL%
echo     Results: %ERR%
if %ERR%==0 goto done
pause
:done
endlocal





