DEBUG=1
TARGET=MAILFLT.NLM

CROSSPATH=/opt/cross/bin
NDK=/mnt/slickdata/Tools


CC=$(CROSSPATH)/i586-netware-gcc
LD=$(CROSSPATH)/i586-netware-nlmconv
CXX=$(CC)
MAKE=make
#MAKE=make -j4
OPTIM=-O2
ifneq ($(DEBUG),"1")
OPTIM=
endif

#INCLUDES= -nostdinc -I/opt/novell/ndk/libc/include -I/opt/novell/ndk/libc/include/winsock
INCLUDES=-I. -I../../../../libs/nrm -I../nlm -I../../Main -I../../Config -I../../Libs/pcre -I../../../iXplat -I../../Libs/zlib-libc/contrib/minizip/nw -I../../Libs/tnef-lib -I../../../../libs/cxxutil -I../../Libs/src/tnef -I../../Libs/src/zlib-libc/contrib/minizip -I../../Libs/src/zlib-libc -D __NOVELL_LIBC__ -D N_PLAT_NLM

CFLAGS=-fno-builtin -fpack-struct -fpcc-struct-return -fno-strict-aliasing $(OPTIM) $(INCLUDES) -s
CXXFLAGS=$(CFLAGS)

OBJS_MF=dynaload-nrm.o MFNLM-LibC.o ../../Main/DCCInterface.o ../../Main/MFChecksum.o ../../Main/MFCleanup.o ../../Main/MFConfig.o ../../Main/MFExports.o ../../Main/MFMail.o ../../Main/MFMalloc.o ../../Main/MFNLM-UI.o ../../Main/MFRelayHost.o ../../Main/MFStatus.o ../../Main/MFUnZip.o ../../Main/MFUtil.o ../../Main/MFVirusScan.o ../../Main/MFWork.o ../../Main/MFZip.o ../nlm/MFAVA-NLM.o ../../Main/DCCInterface.o ../nlm/MFConfigEditor.o ../nlm/MFRestore.o ../nlm/MFNLM.o ../nlm/MFNLM-NRM.o ../../Config/MFConfig.o  ../../Licensing/MFLic.o

OBJS_PCRE=../../Libs/src/pcre-4.5/maketables.o ../../Libs/src/pcre-4.5/pcre.o ../../Libs/src/pcre-4.5/study.o ../../Libs/src/pcre-4.5/get.o

OBJS_LIBS=alloca.o ../../Libs/src/tnef/tnef-lib.o ../../Libs/mime/comi18n.o ../../Libs/mime/mimeenc.o ../../../iXplat/source/ix_getopt.o ../../../iXplat/source/ix_dir.o ../../../iXplat/source/iXDir.o ../../../iXplat/source/iXList.o ../../../iXplat/source/ix_memicmp.o 
# ../../Libs/mime/mozcomi18n.o

OBJS_ZLIB=../../Libs/src/zlib-libc/contrib/minizip/zip.o ../../Libs/src/zlib-libc/contrib/minizip/unzip.o ../../Libs/src/zlib-libc/infutil.o ../../Libs/src/zlib-libc/trees.o ../../Libs/src/zlib-libc/uncompr.o ../../Libs/src/zlib-libc/zutil.o ../../Libs/src/zlib-libc/adler32.o ../../Libs/src/zlib-libc/compress.o ../../Libs/src/zlib-libc/crc32.o ../../Libs/src/zlib-libc/deflate.o ../../Libs/src/zlib-libc/gzio.o ../../Libs/src/zlib-libc/infblock.o ../../Libs/src/zlib-libc/infcodes.o ../../Libs/src/zlib-libc/inffast.o ../../Libs/src/zlib-libc/inflate.o ../../Libs/src/zlib-libc/inftrees.o

OBJS=$(OBJS_MF) $(OBJS_PCRE) $(OBJS_LIBS) $(OBJS_ZLIB)

# ../../Main/SMTPProxy.o
DEF=gcc.def


# default target
default:
	@echo ">> Building MAILFLT.NLM ..."
	@$(MAKE) TARGET=MAILFLT.NLM target

# rebuild both binaries
rebuild:
	@echo ">> Re-Building all ..."
	@$(MAKE) TARGET=MFSYM.NLM clean
	@$(MAKE) TARGET=MFSYM.NLM target
	@$(MAKE) TARGET=MAILFLT.NLM clean
	@$(MAKE) TARGET=MAILFLT.NLM target


target:	$(TARGET)


output.def: $(DEF)
	@rm -f output.def
	@cp $(DEF) output.def
	@echo "output $(TARGET)" >> output.def


MAILFLT.NLM: $(OBJS) output.def
	#@echo "  STRIP    $(TARGET)"
	#@strip $(OBJS)
	@echo "  LINK     $(TARGET)"
	@$(LD) -T output.def
	@ls -lah $(TARGET)


MFSYM.NLM: $(OBJS) output.def
	@echo "  LINK     $(TARGET)"
	@$(LD) -T output.def
	@ls -lah $(TARGET)


%.o : %.c++
	@echo "  CXX      $<"
	@$(CXX) -c $(CXXFLAGS) $< -o $@

%.o : %.cpp
	@echo "  CXX      $<"
	@$(CXX) -c $(CXXFLAGS) $< -o $@

%.o : %.c
	@echo "  CC       $<"
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	@echo "  CLEAN    $(TARGET)"
	@rm -f $(TARGET) $(OBJS) output.def

