CC=mwccnlm
LINK=mwldnlm
CCFLAGS=

OUTDIR=..\MailFilter\out
PROGOBJ=$(OUTDIR)\ixplat.lib

OBJECTS=Build\ix_dir.o \
	Build\ix_getopt.o \
	Build\ix_memicmp.o \
	Build\ix_server.o \
	Build\iXList.o

.c{Build}.o:
	$(CC) $(CCFLAGS) $*

$(OUTDIR):
	if not exist $(OUTDIR)\nul md $(OUTDIR)

$(PROGOBJ): $(OBJECTS)

all: $(OUTDIR) $(PROGOBJ)
	
