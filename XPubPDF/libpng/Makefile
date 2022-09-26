CC = g++
C = gcc
AR = ar
LFLAGS = -Wall
CFLAGS = -I. -D$(OSTYPEDEFINE) -D_THREAD_SAFE -D_REENTRANT $(GLOBAL_DEFINES)
#-Wall
#-DPREVENT_LINK_WARNINGS

TARGET_NAME=libpng.a

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



OBJS= $(OUTDIR)png.o $(OUTDIR)pngerror.o $(OUTDIR)pnggccrd.o $(OUTDIR)pngget.o \
$(OUTDIR)pngmem.o $(OUTDIR)pngpread.o $(OUTDIR)pngread.o $(OUTDIR)pngrio.o $(OUTDIR)pngrtran.o \
$(OUTDIR)pngrutil.o $(OUTDIR)pngset.o $(OUTDIR)pngtest.o $(OUTDIR)pngtrans.o $(OUTDIR)pngvcrd.o \
$(OUTDIR)pngwio.o $(OUTDIR)pngwrite.o $(OUTDIR)pngwtran.o $(OUTDIR)pngwutil.o

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
$(OUTDIR)png.o : png.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngerror.o : pngerror.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pnggccrd.o : pnggccrd.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngget.o : pngget.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngmem.o : pngmem.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngpread.o : pngpread.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngread.o : pngread.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngrio.o : pngrio.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngrtran.o : pngrtran.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngrutil.o : pngrutil.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngset.o : pngset.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngtest.o : pngtest.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngtrans.o : pngtrans.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngvcrd.o : pngvcrd.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngwio.o : pngwio.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngwrite.o : pngwrite.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngwtran.o : pngwtran.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<
$(OUTDIR)pngwutil.o : pngwutil.c pngasmrd.h pngconf.h png.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(C) -c $(CFLAGS) -o $@ $<

clean:
	@if [ -d $(OUTDIR) ] ; then rm -f $(OUTDIR)*; rmdir $(OUTDIR) ; fi

