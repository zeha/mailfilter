@echo off
if not exist buildenv.cmd goto wrongdir
goto do_commit

:do_commit
cvs commit
goto end

:wrongdir
echo [Abort] Incorrect working directory?
goto end

:end