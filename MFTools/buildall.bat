@echo off
echo.
echo.
echo MailFilter/ax Build-All
echo.
echo   EDIT MAILFILTER/MAIN/MFVERSION.H!
echo.
echo.
cd ..
set CMDIDE=call mftools\build.bat

REM rd /s mailfilter\out
del mailfilter\out\*.nlm
del mailfilter\out\*.ncv
del mailfilter\out\*.map
del mailfilter\out\*.lib

%CMDIDE% iXplat ixplat.mcp /t "iXplat NLM LibC"
%CMDIDE% iXplat ixplat.mcp /t "iXplat NLM CLib"
%CMDIDE% iXplat ixplat.mcp /t "iXplat W32"

%CMDIDE% mailfilter\libs\src\pcre-4.5\pcrelib pcrelib.mcp /t "pcrelib nlm"
%CMDIDE% mailfilter\libs\src\pcre-4.5\pcrelib pcrelib.mcp /t "pcrelib w32"
%CMDIDE% mailfilter\libs\src\pcre-4.5\pcreposixlib pcreposixlib.mcp

REM %CMDIDE% mailfilter\libs\src\zlib-libc\netware\static static.mcp
REM %CMDIDE% mailfilter\libs\src\zlib-libc\contrib\minizip\nw minizip.mcp /t "MiniZip as Static Library"

%CMDIDE% mailfilter\libs\src\tnef tnef-lib.mcp /t "TNEF Lib / NetWare CLIB"
%CMDIDE% mailfilter\libs\src\tnef tnef-lib.mcp /t "TNEF Lib / NetWare LibC"
%CMDIDE% mailfilter\libs\src\tnef tnef-lib.mcp /t "TNEF Lib / Win32"

%CMDIDE% mailfilter\Licensing\project library.mcp /t "Static Library NLM/CLib"
%CMDIDE% mailfilter\Licensing\project library.mcp /t "Static Library NLM/LibC"
%CMDIDE% mailfilter\Licensing\project library.mcp /t "Static Library Win32"

REM %CMDIDE% mailfilter\platform\nlm\mfnrm MFNRM.mcp
REM %CMDIDE% mailfilter\platform\nlm\mfzap MFZAP.mcp

REM %CMDIDE% mfconfig.nlm mfconfig.mcp /t "MFConfig / CLIB"
REM %CMDIDE% mfconfig.nlm mfconfig.mcp /t "MFUpgrade / CLIB"

REM %CMDIDE% mfmmx mfmmx.mcp

REM %CMDIDE% mfrestore mfrestore.mcp


%CMDIDE% mfxor mfxor.mcp

%CMDIDE% EicarTest EicarTest.mcp /t "NLMRelease"

%CMDIDE% mfconfig.nlm-fake MFConfig-fake.mcp /t "MFConfig FAKE"

%CMDIDE% MFAVA MFAVA.mcp /t "AntiVirusAgent"


%CMDIDE% mailfilter\platform\nlm mailfilter-clib.mcp /t "MailFilter CLib"
%CMDIDE% mailfilter\platform\nlm-libc mailfilter-libc.mcp /t "MailFilter LibC"

