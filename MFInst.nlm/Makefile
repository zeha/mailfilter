# NMAKE Makefile for MailFilter/ax

!IF "$(CFG)" == ""
!ERROR WRONG PARAMETERS
!ENDIF 

THISPART=MFConfig.NLM
OutDir=$(OUTDIR)

ALL : "build"

"$(OUTDIR)\$(THISPART)" :
	mkdir "$(OUTDIR)\$(THISPART)"

"CLEAN" :

"build" : "$(OUTDIR)\$(THISPART)\mfconfig.o" "$(OUTDIR)\$(THISPART)\mfconfig.nlm"
		
"$(OUTDIR)\$(THISPART)\mfconfig.o" :
	$(CW_NLM_CL) -c mfconfig.o

