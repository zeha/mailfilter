@echo off
if not exist s:\system\mailflt.nlm goto nodest
if not exist mailflt.nlm goto nosource

:copynlms
xcopy *.nlm s:\system\ /y /v
goto ende

:nodest
echo.
echo Check Destination Drive:
echo  S:\System\MailFlt.nlm not found!
goto ende

:nosource
echo.
echo Check Current Directory!
echo  MailFlt.nlm not found!
goto ende

:ende
