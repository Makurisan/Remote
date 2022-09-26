CC = g++
LFLAGS = -fPIC -shared -Wall
CFLAGS = -fPIC -shared -Wall -I. -D$(OSTYPEDEFINE) -D_THREAD_SAFE -D_REENTRANT -Wno-deprecated $(GLOBAL_DEFINES)
#-DPREVENT_LINK_WARNINGS

TARGET_NAME=libXPubTools.so

ifndef CFG
	CFG=DEBUG
endif

-include ../make_defines

ifeq ($(CFG),DEBUG)
	CONFIG_INFO = Configuration is DEBUG
	LFLAGS += -g
	CFLAGS += -g -D_DEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS = -L../$(COMMONOUTDIR)
else
	CONFIG_INFO = Configuration is RELEASE
	LFLAGS += -O2
	CFLAGS += -O2 -DNDEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS = -L../$(COMMONOUTDIR)
endif



OBJS= $(OUTDIR)XPubTools.o $(OUTDIR)Synchronization.o $(OUTDIR)GlobalSettings.o
#$(OUTDIR)SharedMemory.o

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
	$(CC) $(LFLAGS) -v -o $@ $(OBJS) $(XPUBLIBS)
	cp $(OUTDIR)$(TARGET_NAME) ../$(COMMONOUTDIR)
###############################################
# main
$(OUTDIR)XPubTools.o : XPubTools.cpp Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
# STools
$(OUTDIR)GlobalSettings.o : STools/GlobalSettings.cpp STools/GlobalSettings.h STools/XPubToolsExpImp.h STools/types.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)SharedMemory.o : STools/SharedMemory.cpp STools/SharedMemory.h STools/XPubToolsExpImp.h STools/types.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)Synchronization.o : STools/Synchronization.cpp STools/Synchronization.h STools/XPubToolsExpImp.h STools/types.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<



clean:
	@if [ -d $(OUTDIR) ] ; then rm -f $(OUTDIR)*; rmdir $(OUTDIR) ; fi

