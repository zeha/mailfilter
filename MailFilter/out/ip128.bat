@echo off
net use t: /del
net use t: \\192.168.130.128\sys /user:admin dajava
xcopy MAILFLT.NLM t:\system\ /y
xcopy MFAVA.NLM t:\system\ /y
xcopy MFDBG.NLM t:\system\ /y
xcopy MFSYM.NLM t:\system\ /y
xcopy MFLT50.NLM t:\system\ /y
xcopy MFCONFIG.NLM t:\system\ /y
xcopy MFBUG.NLM t:\system\ /y
pause
