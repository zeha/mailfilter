@echo off
net use t: /del
net use t: \\192.168.130.127\sys /user:admin dajava
xcopy MAILFLT.NLM t:\system\ /y
xcopy MFAVA.NLM t:\system\ /y
pause