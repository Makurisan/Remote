CC = g++
LFLAGS = -fPIC -shared -Wall
CFLAGS = -fPIC -shared -Wall -I. -I../XPubTools -I/usr/include/python2.3 -D$(OSTYPEDEFINE) -D_THREAD_SAFE -D_REENTRANT -Wno-deprecated $(GLOBAL_DEFINES)
#-DPREVENT_LINK_WARNINGS

TARGET_NAME=libXPubModel.so

ifndef CFG
	CFG=DEBUG
endif

-include ../make_defines

ifeq ($(CFG),DEBUG)
	CONFIG_INFO = Configuration is DEBUG
	LFLAGS += -g
	CFLAGS += -g -D_DEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS = -L../$(COMMONOUTDIR) -lXPubTools
ifndef NO_DBMYSQL
	EXTERNLIBS = -lpython2.3
endif
else
	CONFIG_INFO = Configuration is RELEASE
	LFLAGS += -O2
	CFLAGS += -O2 -DNDEBUG
	OUTDIR = $(COMMONOUTDIR)/
	XPUBLIBS = -L../$(COMMONOUTDIR) -lXPubTools
ifndef NO_DBMYSQL
	EXTERNLIBS = -lpython2.3
endif
endif


OBJS= $(OUTDIR)XPubModel.o $(OUTDIR)STLdef.o \
$(OUTDIR)SyncEngineBase.o $(OUTDIR)SyncGroups.o $(OUTDIR)SyncEngine.o \
$(OUTDIR)ProjectCompare.o $(OUTDIR)Utils.o \
$(OUTDIR)MarkupSTL.o \
$(OUTDIR)Constants.o $(OUTDIR)DBDefinitions.o $(OUTDIR)DBStoneInd.o $(OUTDIR)DBStoneInd_Sync.o \
$(OUTDIR)DBTemplateInd.o $(OUTDIR)DBTemplateInd_Sync.o $(OUTDIR)DTNSupport.o \
$(OUTDIR)ERModelInd.o $(OUTDIR)ExternalEditors.o $(OUTDIR)FormatDateTime.o $(OUTDIR)FormatNumber.o \
$(OUTDIR)IFSEntityInd.o $(OUTDIR)IFSEntityInd_Sync.o \
$(OUTDIR)Messages.o $(OUTDIR)ModelDef.o $(OUTDIR)ModelInd.o $(OUTDIR)MPModelERModelInd.o $(OUTDIR)MPModelERModelInd_Sync.o \
$(OUTDIR)MPModelFieldsInd.o $(OUTDIR)MPModelFieldsInd_Sync.o $(OUTDIR)MPModelIFSInd.o $(OUTDIR)MPModelIFSInd_Sync.o \
$(OUTDIR)MPModelPythonInd.o $(OUTDIR)MPModelStoneInd.o $(OUTDIR)MPModelStoneInd_Sync.o \
$(OUTDIR)MPModelTemplateInd.o $(OUTDIR)MPModelTemplateInd_Sync.o \
$(OUTDIR)OpenedModules.o $(OUTDIR)ProgressFeature.o $(OUTDIR)ProjectFiles_Base.o $(OUTDIR)ProjectFiles_ER.o $(OUTDIR)ProjectFiles_IFS.o \
$(OUTDIR)ProjectFilesInd.o $(OUTDIR)ProjectFiles_Python.o $(OUTDIR)ProjectFiles_Stone.o $(OUTDIR)ProjectFiles_Template.o \
$(OUTDIR)PropStoneInd.o $(OUTDIR)PropStoneInd_Sync.o $(OUTDIR)ReservedERWords.o $(OUTDIR)XMLModelClasses.o \
$(OUTDIR)XPubVariant.o $(OUTDIR)ZoomFactor.o \
$(OUTDIR)MD5.o $(OUTDIR)Expression.o $(OUTDIR)HtmlStoneImage.o $(OUTDIR)PythonInterface.o $(OUTDIR)XPubPythonWrapper.o

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
	$(CC) $(LFLAGS) -v -o $@ $(OBJS) $(XPUBLIBS) $(EXTERNLIBS)
	cp $(OUTDIR)$(TARGET_NAME) ../$(COMMONOUTDIR)
###############################################
# main
$(OUTDIR)XPubModel.o : XPubModel.cpp Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
# SSTLdef
$(OUTDIR)STLdef.o : SSTLdef/STLdef.cpp SSTLdef/STLdef.h SOpSys/charwchar.h SModelInd/ModelExpImp.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
# SSyncEng
$(OUTDIR)SyncEngineBase.o : SSyncEng/SyncEngineBase.cpp SSyncEng/SyncEngineBase.h SSyncEng/SyncEngine.h SSyncEng/SyncGroups.h SModelInd/ModelExpImp.h SSTLdef/STLdef.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)SyncGroups.o : SSyncEng/SyncGroups.cpp SSyncEng/SyncGroups.h SSyncEng/SyncEngineBase.h SSyncEng/SyncEngine.h Sxml/MarkupSTL.h SModelInd/ModelInd.h SModelInd/ERModelInd.h SModelInd/IFSEntityInd.h SModelInd/ModelExpImp.h SSTLdef/STLdef.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)SyncEngine.o : SSyncEng/SyncEngine.cpp SSyncEng/SyncEngine.h SSyncEng/SyncEngineBase.h SSyncEng/SyncGroups.h SModelInd/ModelExpImp.h SSTLdef/STLdef.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
# STools
$(OUTDIR)ProjectCompare.o : STools/ProjectCompare.cpp STools/ProjectCompare.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)Utils.o : STools/Utils.cpp STools/Utils.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
# Sxml
$(OUTDIR)MarkupSTL.o : Sxml/MarkupSTL.cpp Sxml/MarkupSTL.h SModelInd/ModelExpImp.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
# SModelInd
$(OUTDIR)Constants.o : SModelInd/Constants.cpp SModelInd/Constants.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)DBDefinitions.o : SModelInd/DBDefinitions.cpp SModelInd/DBDefinitions.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)DBStoneInd.o : SModelInd/DBStoneInd.cpp SModelInd/DBStoneInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)DBStoneInd_Sync.o : SModelInd/DBStoneInd_Sync.cpp SModelInd/DBStoneInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)DBTemplateInd.o : SModelInd/DBTemplateInd.cpp SModelInd/DBTemplateInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)DBTemplateInd_Sync.o : SModelInd/DBTemplateInd_Sync.cpp SModelInd/DBTemplateInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)DTNSupport.o : SModelInd/DTNSupport.cpp SModelInd/DTNSupport.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ERModelInd.o : SModelInd/ERModelInd.cpp SModelInd/ERModelInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ExternalEditors.o :  SModelInd/ExternalEditors.cpp SModelInd/ExternalEditors.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)FormatDateTime.o :  SModelInd/FormatDateTime.cpp SModelInd/FormatDateTime.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)FormatNumber.o :  SModelInd/FormatNumber.cpp SModelInd/FormatNumber.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)IFSEntityInd.o :  SModelInd/IFSEntityInd.cpp SModelInd/IFSEntityInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)IFSEntityInd_Sync.o :  SModelInd/IFSEntityInd_Sync.cpp SModelInd/IFSEntityInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)Messages.o : SModelInd/Messages.cpp SModelInd/Messages.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ModelDef.o : SModelInd/ModelDef.cpp SModelInd/ModelDef.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ModelInd.o : SModelInd/ModelInd.cpp SModelInd/ModelInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelERModelInd.o : SModelInd/MPModelERModelInd.cpp SModelInd/MPModelERModelInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelERModelInd_Sync.o : SModelInd/MPModelERModelInd_Sync.cpp SModelInd/MPModelERModelInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelFieldsInd.o : SModelInd/MPModelFieldsInd.cpp SModelInd/MPModelFieldsInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelFieldsInd_Sync.o : SModelInd/MPModelFieldsInd_Sync.cpp SModelInd/MPModelFieldsInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelIFSInd.o : SModelInd/MPModelIFSInd.cpp SModelInd/MPModelIFSInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelIFSInd_Sync.o : SModelInd/MPModelIFSInd_Sync.cpp SModelInd/MPModelIFSInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelPythonInd.o : SModelInd/MPModelPythonInd.cpp SModelInd/MPModelPythonInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelStoneInd.o : SModelInd/MPModelStoneInd.cpp SModelInd/MPModelStoneInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelStoneInd_Sync.o : SModelInd/MPModelStoneInd_Sync.cpp SModelInd/MPModelStoneInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelTemplateInd.o : SModelInd/MPModelTemplateInd.cpp SModelInd/MPModelTemplateInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)MPModelTemplateInd_Sync.o : SModelInd/MPModelTemplateInd_Sync.cpp SModelInd/MPModelTemplateInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)OpenedModules.o : SModelInd/OpenedModules.cpp SModelInd/OpenedModules.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ProgressFeature.o : SModelInd/ProgressFeature.cpp SModelInd/ProgressFeature.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ProjectFiles_Base.o : SModelInd/ProjectFiles_Base.cpp SModelInd/ProjectFiles_Base.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ProjectFiles_ER.o : SModelInd/ProjectFiles_ER.cpp SModelInd/ProjectFiles_ER.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ProjectFiles_IFS.o : SModelInd/ProjectFiles_IFS.cpp SModelInd/ProjectFiles_IFS.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ProjectFilesInd.o : SModelInd/ProjectFilesInd.cpp SModelInd/ProjectFilesInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ProjectFiles_Python.o : SModelInd/ProjectFiles_Python.cpp SModelInd/ProjectFiles_Python.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ProjectFiles_Stone.o : SModelInd/ProjectFiles_Stone.cpp SModelInd/ProjectFiles_Stone.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ProjectFiles_Template.o : SModelInd/ProjectFiles_Template.cpp SModelInd/ProjectFiles_Template.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)PropStoneInd.o : SModelInd/PropStoneInd.cpp SModelInd/PropStoneInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)PropStoneInd_Sync.o : SModelInd/PropStoneInd_Sync.cpp SModelInd/PropStoneInd.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ReservedERWords.o : SModelInd/ReservedERWords.cpp SModelInd/ReservedERWords.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)XMLModelClasses.o : SModelInd/XMLModelClasses.cpp SModelInd/XMLModelClasses.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)XPubVariant.o : SModelInd/XPubVariant.cpp SModelInd/XPubVariant.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)ZoomFactor.o : SModelInd/ZoomFactor.cpp SModelInd/ZoomFactor.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
#SMD5
$(OUTDIR)MD5.o : SMD5/MD5.cpp SMD5/MD5.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
###############################################
# SExpression
$(OUTDIR)Expression.o : SExpression/Expression.cpp SExpression/Expression.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)HtmlStoneImage.o : SExpression/HtmlStoneImage.cpp SExpression/HtmlStoneImage.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)PythonInterface.o : SExpression/PythonInterface.cpp SExpression/PythonInterface.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<
$(OUTDIR)XPubPythonWrapper.o : SExpression/XPubPythonWrapper.cpp SExpression/XPubPythonWrapper.h Makefile
	@echo
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	@echo ++ Compiling $<
	@echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	$(CC) -c $(CFLAGS) -o $@ $<



clean:
	@if [ -d $(OUTDIR) ] ; then rm -f $(OUTDIR)*; rmdir $(OUTDIR) ; fi

