@echo off
set ZIPFLAGS=zip -r -9 -o -X 

rmdir /s /q zips
mkdir zips

echo mfutil.zip
%ZIPFLAGS% zips\mfutil.zip mffilter2csv.exe mfconfig.exe mfconfig.chm

echo mf-nlm.zip
%ZIPFLAGS% zips\mf-nlm mailflt.nlm mfconfig.nlm mfupgr.nlm mfmmx.nlm mfnrm.nlm libc\mailflt.nlm
echo mf-w32.zip
%ZIPFLAGS% zips\mf-w32 mailfilter-axpro-win32.exe

pause
