@echo off

REM
REM fetch.bat
REM
REM fetches MailFilter things from CVS
REM

set ACTION=checkout

if %1~==~ goto tag
set ACTIOn=%1

:tag
if %ACTION%==checkout goto settag
if %CVSTAG%~==~ set CVSTAG=HEAD

:settag
if %CVSTAG%~==~ goto start
set TAG=-r %CVSTAG%


:start

set CVS=E:\Programme\TortoiseCVS\cvs.exe
set CVSROOT=:local:I:\Projekte\MailFlt\CVS

echo CVSROOT: %CVSROOT%
echo TAG: %CVSTAG%
echo ACTION: %ACTION%

%CVS% -q -z6 %ACTION% %TAG% DistributionKit
%CVS% -q -z6 %ACTION% %TAG% iXplat
%CVS% -q -z6 %ACTION% %TAG% MailFilter
%CVS% -q -z6 %ACTION% %TAG% MFConfig
%CVS% -q -z6 %ACTION% %TAG% MFConfig.NLM
%CVS% -q -z6 %ACTION% %TAG% MFFilterICE
%CVS% -q -z6 %ACTION% %TAG% MFInstall.W32
%CVS% -q -z6 %ACTION% %TAG% MFMMX
%CVS% -q -z6 %ACTION% %TAG% MFRestore
%CVS% -q -z6 %ACTION% %TAG% MFTools
%CVS% -q -z6 %ACTION% %TAG% MFXOR
%CVS% -q -z6 %ACTION% %TAG% MkLicense

pause
