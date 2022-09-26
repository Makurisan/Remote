CC = g++
LFLAGS = -fPIC -shared -Wall
CFLAGS = -fPIC -shared -Wall -I. -I../XPubModel -I../XPubTools -D$(OSTYPEDEFINE) -D_THREAD_SAFE -D_REENTRANT -Wno-deprecated $(GLOBAL_DEFINES)
#-DPREVENT_LINK_WARNINGS

TARGET_NAME=libXPubAddOnMan.so

ifndef CFG
	CFG=DEBUG
endif

-include ../make_defines

ifeq ($(CFG),DEBUG)
	CONFIG_INFO = Configuration is DEBUG
	LFLAGS += -g
	CFLAGS += -g -D_DEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS = -L../$(COMMONOUTDIR) -lXPubModel -lXPubTools
else
	CONFIG_INFO = Configuration is RELEASE
	LFLAGS += -O2
	CFLAGS += -O2 -DNDEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS = -L../$(COMMONOUTDIR) -lXPubModel -lXPubTools
endif



OBJS= $(OUTDIR)XPubAddOnMan.o $(OUTDIR)XPubAddOnManager.o $(OUTDIR)FormatWorker.o $(OUTDIR)ConvertWorker.o $(OUTDIR)TagBuilder.o

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
$(OUTDIR)XPubAddOnMan.o : XPubAddOnMan.cpp Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
# SAddOnManager
$(OUTDIR)XPubAddOnManager.o : SAddOnManager/XPubAddOnManager.cpp SAddOnManager/XPubAddOnManager.h SAddOnManager/AddOnManagerExpImp.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)FormatWorker.o : SAddOnManager/FormatWorker.cpp SAddOnManager/FormatWorker.h SAddOnManager/AddOnManagerExpImp.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ConvertWorker.o : SAddOnManager/ConvertWorker.cpp SAddOnManager/ConvertWorker.h SAddOnManager/AddOnManagerExpImp.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)TagBuilder.o : SAddOnManager/TagBuilder.cpp SAddOnManager/TagBuilder.h SAddOnManager/AddOnManagerExpImp.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<


clean:
	@if [ -d $(OUTDIR) ] ; then rm -f $(OUTDIR)*; rmdir $(OUTDIR) ; fi

