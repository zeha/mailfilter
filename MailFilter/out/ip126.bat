@echo off
net use t: /del
net use t: \\192.168.130.126\sys /user:admin
xcopy MAILFLT.NLM t:\system\ /y
xcopy MFAVA.NLM t:\system\ /y
pause