@echo off
nmake clean /nologo
nmake rel /nologo
copy out\mailfilter.nlm s:\system\mailflt.nlm /y
copy out\mailfilter.map s:\system\mailflt.map /y
REM copy out\mailfilter.nlm t:\system\mailflt.nlm /y
REM copy out\mailfilter.map t:\system\mailflt.map /y

if %1~==~ goto nonlm
if %1==0 goto nonlm
if %1==2 copy out\mailfilter.nlm s:\system\mail2.nlm /y
if %1==3 copy out\mailfilter.nlm s:\system\mail3.nlm /y
if %1==4 copy out\mailfilter.nlm s:\system\mail4.nlm /y
if %1==5 copy out\mailfilter.nlm s:\system\mail5.nlm /y
if %1==6 copy out\mailfilter.nlm s:\system\mail6.nlm /y
if %1==7 copy out\mailfilter.nlm s:\system\mail7.nlm /y
if %1==8 copy out\mailfilter.nlm s:\system\mail8.nlm /y
if %1==9 copy out\mailfilter.nlm s:\system\mail9.nlm /y
if %1==10 copy out\mailfilter.nlm s:\system\mail10.nlm /y
if %1==11 copy out\mailfilter.nlm s:\system\mail11.nlm /y
if %1==12 copy out\mailfilter.nlm s:\system\mail12.nlm /y
if %1==13 copy out\mailfilter.nlm s:\system\mail13.nlm /y

shift

:nonlm

if %1~==~ goto nodist

nmake dist\MailFilter-Setup.exe /nologo
REM copy dist\MailFilter-Setup.exe g:\temp\MailFilter-Setup.exe /y
copy dist\MailFilter-Setup.exe "..\Releases\MailFilter-Setup _NEWEST.exe" /y

shift

:nodist
