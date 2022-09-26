CC = g++
LFLAGS = -fPIC -shared -Wall
CFLAGS = -fPIC -shared -Wall -I. -I../XPubModel -I../XPubAddOnMan -I../XPubMagic -D$(OSTYPEDEFINE) -D_THREAD_SAFE -D_REENTRANT -Wno-deprecated $(GLOBAL_DEFINES)
#-DPREVENT_LINK_WARNINGS

TARGET_NAME=libXPubCnvFTag.so

ifndef CFG
	CFG=DEBUG
endif

-include ../make_defines

ifeq ($(CFG),DEBUG)
	CONFIG_INFO = Configuration is DEBUG
	LFLAGS += -g
	CFLAGS += -g -D_DEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS = -L../$(COMMONOUTDIR) -lXPubModel -lXPubAddOnMan -lXPubMagic
else
	CONFIG_INFO = Configuration is RELEASE
	LFLAGS += -O2
	CFLAGS += -O2 -DNDEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS = -L../$(COMMONOUTDIR) -lXPubModel -lXPubAddOnMan -lXPubMagic
endif



OBJS= $(OUTDIR)XPubCnvFTag.o $(OUTDIR)ConvertWorkerFTag.o

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
$(OUTDIR)XPubCnvFTag.o : XPubCnvFTag.cpp Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
# SAddOnManager
$(OUTDIR)ConvertWorkerFTag.o : SAddOn/ConvertWorkerFTag.cpp SAddOn/ConvertWorkerFTag.h SAddOn/CnvFTagExpImp.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<


clean:
	@if [ -d $(OUTDIR) ] ; then rm -f $(OUTDIR)*; rmdir $(OUTDIR) ; fi

