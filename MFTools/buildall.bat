@echo off
echo.
echo.
echo MailFilter/ax Build-All
echo.
echo   EDIT MAILFILTER/MAIN/MFVERSION.H!
echo.
echo.

set CMDIDE=call mftools\build.bat

rd /s mailfilter\out

%CMDIDE% iXplat ixplat.mcp /t "iXplat NLM"
%CMDIDE% iXplat ixplat.mcp /t "iXplat W32"

%CMDIDE% mailfilter\libs\src\pcre-3.9\pcrelib pcrelib.mcp /t "pcrelib nlm"
%CMDIDE% mailfilter\libs\src\pcre-3.9\pcrelib pcrelib.mcp /t "pcrelib w32"
%CMDIDE% mailfilter\libs\src\pcre-3.9\pcreposixlib pcreposixlib.mcp

%CMDIDE% mailfilter\libs\src\zlib-libc\netware\static static.mcp
%CMDIDE% mailfilter\libs\src\zlib-libc\contrib\minizip\nw minizip.mcp /t "MiniZip as Static Library"

%CMDIDE% mailfilter\libs\src\tnef tnef-lib.mcp /t "TNEF Lib / NetWare CLIB"
%CMDIDE% mailfilter\libs\src\tnef tnef-lib.mcp /t "TNEF Lib / NetWare LibC"
%CMDIDE% mailfilter\libs\src\tnef tnef-lib.mcp /t "TNEF Lib / Win32"

%CMDIDE% mailfilter\Licensing\project library.mcp /t "Static Library NLM/CLib"
%CMDIDE% mailfilter\Licensing\project library.mcp /t "Static Library NLM/LibC"
%CMDIDE% mailfilter\Licensing\project library.mcp /t "Static Library Win32"

%CMDIDE% mailfilter\platform\nlm\mfnrm MFNRM.mcp
%CMDIDE% mailfilter\platform\nlm\mfzap MFZAP.mcp

%CMDIDE% mailfilter\platform\nlm mailfilter-clib.mcp /t "MailFilter/ax CLib"
%CMDIDE% mailfilter\platform\nlm-libc mailfilter-libc.mcp /t "MailFilter/ax LibC"

%CMDIDE% mfconfig.nlm mfconfig.mcp /t "MFConfig / CLIB"
%CMDIDE% mfconfig.nlm mfconfig.mcp /t "MFUpgrade / CLIB"

REM %CMDIDE% mfmmx mfmmx.mcp

%CMDIDE% mfrestore mfrestore.mcp

%CMDIDE% mfxor mfxor.mcp

