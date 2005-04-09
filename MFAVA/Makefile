
CC=i586-netware-gcc
LD=i586-netware-nlmconv
CXX=i586-netware-gcc


#INCLUDES= -nostdinc -I/opt/novell/ndk/libc/include -I/opt/novell/ndk/libc/include/winsock
INCLUDES=

CFLAGS=-fno-builtin -fpack-struct -fpcc-struct-return -fno-strict-aliasing -O2 $(INCLUDES) 
CXXFLAGS=$(CFLAGS)

TARGET=MFAVA.NLM
OBJS=MFAVA.o eTrust7.o Sophos-stub.o ../MailFilter/Main/MFMalloc.o
DEF=asdf.def


$(TARGET): $(OBJS) $(DEF)
	strip $(OBJS)
	$(LD) -T $(DEF)
	ls -lah $(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
