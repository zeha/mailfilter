@echo off
del mfold.mdb
ren mailflt.mdb mfold.mdb
E:MSGMAKE.EXE MAILFLT MailFilter 1.4 4 1.0
E:MsgImp MAILFLT MLC MAILFLT.MLC
E:MsgExp MAILFLT Default NLM MAILFLT.MSG
