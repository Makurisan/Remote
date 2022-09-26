CC = g++
C = gcc
AR = ar
LFLAGS = -Wall
CFLAGS = -I. -D_LIB -D_MT -D$(OSTYPEDEFINE) -D_THREAD_SAFE -D_REENTRANT $(GLOBAL_DEFINES)
#-Wall
#-DPREVENT_LINK_WARNINGS

TARGET_NAME=libzlib.a

ifndef CFG
	CFG=DEBUG
endif

-include ../../make_defines

ifeq ($(CFG),DEBUG)
	CONFIG_INFO = Configuration is DEBUG
	LFLAGS += -g
	CFLAGS += -g -D_DEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS =
else
	CONFIG_INFO = Configuration is RELEASE
	LFLAGS += -O2
	CFLAGS += -O2 -DNDEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS =
endif



OBJS= $(OUTDIR)zutil.o $(OUTDIR)uncompr.o $(OUTDIR)trees.o $(OUTDIR)minigzip.o \
$(OUTDIR)inftrees.o $(OUTDIR)inflate.o $(OUTDIR)inffast.o \
$(OUTDIR)infback.o $(OUTDIR)gzio.o $(OUTDIR)deflate.o $(OUTDIR)crc32.o \
$(OUTDIR)compress.o $(OUTDIR)adler32.o

all: $(OUTDIR) $(OUTDIR)$(TARGET_NAME)

$(OUTDIR):
	mkdir $(OUTDIR)

###############################################
# link
$(OUTDIR)$(TARGET_NAME): $(OBJS)
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Linking $@ $(CONFIG_INFO)
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(AR) rcs $@ $(OBJS) $(XPUBLIBS)
	cp $(OUTDIR)$(TARGET_NAME) ../$(COMMONOUTDIR)
###############################################
#
$(OUTDIR)zutil.o : zutil.c zutil.h zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)uncompr.o : uncompr.c zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)trees.o : trees.c trees.h zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)minigzip.o : minigzip.c zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)inftrees.o : inftrees.c inftrees.h zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)inflate.o : inflate.c inflate.h zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)inffast.o : inffast.c inffast.h zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)infback.o : infback.c zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)gzio.o : gzio.c zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)deflate.o : deflate.c deflate.h zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)crc32.o : crc32.c zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)compress.o : compress.c zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)adler32.o : adler32.c zlib.h zconf.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<

clean:
	@if [ -d $(OUTDIR) ] ; then rm -f $(OUTDIR)*; rmdir $(OUTDIR) ; fi

