@echo off
net use t: /del
net use t: \\192.168.130.126\sys /user:admin dajava
xcopy MAILFLT.NLM t:\system\ /y
xcopy MFAVA.NLM t:\system\ /y
xcopy MAILFLT.NLM.MAP t:\system\MAILFLT.MAP /y
xcopy MFAVA.NLM.MAP t:\system\MFAVA.MAP /y
pause