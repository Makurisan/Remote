CC = g++
LFLAGS = -fPIC -shared -Wall
CFLAGS = -fPIC -shared -Wall -I. -I../XPubModel -I../XPubAddOnMan -I../XPubPDF -I../XPubCnvFTag -D$(OSTYPEDEFINE) -D_THREAD_SAFE -D_REENTRANT -Wno-deprecated $(GLOBAL_DEFINES)
#-DPREVENT_LINK_WARNINGS

TARGET_NAME=libXPubCnvTPdf.so

ifndef CFG
	CFG=DEBUG
endif

-include ../make_defines

ifeq ($(CFG),DEBUG)
	CONFIG_INFO = Configuration is DEBUG
	LFLAGS += -g
	CFLAGS += -g -D_DEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS = -L../$(COMMONOUTDIR) -lXPubModel -lXPubAddOnMan -lXPubPDF
else
	CONFIG_INFO = Configuration is RELEASE
	LFLAGS += -O2
	CFLAGS += -O2 -DNDEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS = -L../$(COMMONOUTDIR) -lXPubModel -lXPubAddOnMan -lXPubPDF
endif



OBJS= $(OUTDIR)XPubCnvTPdf.o $(OUTDIR)ConvertWorkerTPdf.o

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
$(OUTDIR)XPubCnvTPdf.o : XPubCnvTPdf.cpp Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
# SAddOnManager
$(OUTDIR)ConvertWorkerTPdf.o : SAddOn/ConvertWorkerTPdf.cpp SAddOn/ConvertWorkerTPdf.h SAddOn/CnvTPdfExpImp.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<


clean:
	@if [ -d $(OUTDIR) ] ; then rm -f $(OUTDIR)*; rmdir $(OUTDIR) ; fi

