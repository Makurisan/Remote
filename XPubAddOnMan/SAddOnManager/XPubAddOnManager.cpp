#if defined(_LIB)

#include "../../XPubCnvFTag/SAddOn/ConvertWorkerFTag_Lib.h"
#include "../../XPubCnvTPdf/SAddOn/ConvertWorkerTPdf_Lib.h"
#include "../../XPubCnvTRtf/SAddOn/ConvertWorkerTRtf_Lib.h"

#include "../../XPubFrmDb/SAddOn/FormatWorkerDb_Lib.h"
#include "../../XPubFrmProp/SAddOn/FormatWorkerProp_Lib.h"
#include "../../XPubFrmTag/SAddOn/FormatWorkerTag_Lib.h"
#include "../../XPubFrmHtml/SAddOn/FormatWorkerHtml_Lib.h"
#include "../../XPubFrmPlain/SAddOn/FormatWorkerPlain_Lib.h"

#endif // defined(_LIB)

#if defined(LINUX_OS)

#include <dlfcn.h>

#elif defined(WIN32)

#elif defined(MAC_OS_MWE)

#else
#error "not implemented"
#endif

#include "SSTLdef/STLdef.h"
#include "SModelInd/ModelDef.h"
#include "Sxml/XPubMarkUp.h"

#include "STools/Synchronization.h"

#include "XPubAddOnManager.h"

/*
pthread_self = CFBundleGetFunctionPointerForName(bundle, CFSTR("pthread_self"));
*/

#if defined(MAC_OS_MWE)


void MessageBox(const char* message)
{
    SInt16      alertItemHit = 0;
    Str255 OutStr;
    CopyCStringToPascal(message, OutStr);

	StandardAlert(kAlertStopAlert, OutStr, NULL, NULL, &alertItemHit);
        	
}

void *bundle_open(const char *name);

void *bundle_open(const char *name)
{
  void *result = 0;
	
  xtstring BundleName( name );

  CFBundleRef mainBundle = CFBundleGetMainBundle();

 // get the PlugIns Directory

  CFURLRef pluginBundle = CFBundleCopyBuiltInPlugInsURL(mainBundle);
  
  char sPlugInName[500];
  
  CFArrayRef bundleArray = CFBundleCreateBundlesFromDirectory( kCFAllocatorDefault,
                   pluginBundle, NULL );
 
  if(bundleArray)
  {
    long bundleArraySize = CFArrayGetCount(bundleArray);

    for(long i = 0; i < bundleArraySize; i++)
    {
 			CFBundleRef currBundle = (CFBundleRef)CFArrayGetValueAtIndex(bundleArray, i);
	 	 
 			CFDictionaryRef bundleInfoDict = CFBundleGetInfoDictionary( currBundle );	
	 	  
 			CFStringRef cfBundleName;
 			cfBundleName = (CFStringRef)CFDictionaryGetValue( bundleInfoDict, CFSTR("InternalName"));
 			if(cfBundleName)
 			{
 				char sPlugInName[255];
				CFStringGetCString(cfBundleName, sPlugInName, 255, kCFStringEncodingUTF8);
		  
				if(!strcmp(name, sPlugInName))	
				{
          Boolean loaded = CFBundleLoadExecutable( currBundle );

          if ( loaded )
          {
           result = currBundle;
          }	
				}
 			}
    } 	 	
  }
  return result;			

}

void *getfunction(CFBundleRef bundle, const CFStringRef functionname)
{
  if(!bundle)
	return 0;

 // CFStringRef cfBundleName = CFStringCreateWithCString( NULL, functionname.c_str(), kCFStringEncodingASCII);

  void *pointer = CFBundleGetFunctionPointerForName(bundle, functionname);
  	 
  return pointer;
  
}

#endif





#if defined(LINUX_OS)

#define SHARED_PREFIX        _XT("lib")
#define SHARED_EXTENSION     _XT(".so")
#define XPUB_GET_F_ADDR      dlsym
#define XPUB_LOAD_SHARED(x)  dlopen(x, RTLD_NOW)

#elif defined(WIN32)

#define SHARED_PREFIX        _XT("")
#define SHARED_EXTENSION     _XT(".dll")
#define XPUB_GET_F_ADDR      ::GetProcAddress
#define XPUB_LOAD_SHARED(x)  ::LoadLibraryEx(x, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

#elif defined(MAC_OS_MWE)

#define SHARED_PREFIX        _XT("")
#define SHARED_EXTENSION     _XT("")
#define XPUB_GET_F_ADDR(x, y) getfunction((CFBundleRef)x, CFSTR(y))
#define XPUB_LOAD_SHARED(x)  bundle_open(x)

#else
#error "not implemented"
#endif

ADDONMODULE LoadAddOnLibrary(const xtstring& sName)
{
#if defined(_LIB)

  return (ADDONMODULE)-1;

#else // defined(_LIB)

  return XPUB_LOAD_SHARED(sName.c_str());

#endif // defined(_LIB)
}

#if !defined(_LIB)

extern char g_app[XPUB_MAX_PATH];

#endif // !defined(_LIB)

////////////////////////////////////////////////////////////////////////////////
// CFrmAddOnModule
////////////////////////////////////////////////////////////////////////////////
CFrmAddOnModule::CFrmAddOnModule(ADDONMODULE hAddOnModule,
                                 CXPubAddOnManager* pAddOnManager,
                                 TDataFormatForStoneAndTemplate  nModuleDataFormat)
{
  m_hAddOnModule = hAddOnModule;
  m_pAddOnManager = pAddOnManager;
  m_nModuleDataFormat = nModuleDataFormat;
  m_pFormatAddOn = 0;

  RegisterAddOnManager = 0;
  UnregisterAddOnManager = 0;
  GetFrmAddOnVersion = 0;
  CreateFormatWorker_ = 0;
  CreateFormatWorkerClone_ = 0;
  ReleaseFormatWorker_ = 0;
}

CFrmAddOnModule::~CFrmAddOnModule()
{
  if (m_pFormatAddOn)
    UnregisterAddOnManager(m_pFormatAddOn);

#if defined(_LIB)

  // nichts machen

#else // defined(_LIB)

#if defined(LINUX_OS)
  dlclose(m_hAddOnModule);
#elif defined(WIN32)
  ::FreeLibrary(m_hAddOnModule);
#elif defined(MAC_OS_MWE)
  CFBundleUnloadExecutable((CFBundleRef)m_hAddOnModule); 
#else
#error "not implemented"
#endif

#endif // defined(_LIB)
}

bool CFrmAddOnModule::InitAddOnModule()
{
#if defined(_LIB)

  if (m_nModuleDataFormat == tDataFormatST_Db)
  {
    RegisterAddOnManager = RegisterAddOnManager_LIB_FRMDB;
    UnregisterAddOnManager = UnregisterAddOnManager_LIB_FRMDB;
    GetFrmAddOnVersion = GetFrmAddOnVersion_LIB_FRMDB;
    CreateFormatWorker_ = CreateFormatWorker__LIB_FRMDB;
    CreateFormatWorkerClone_ = CreateFormatWorkerClone__LIB_FRMDB;
    ReleaseFormatWorker_ = ReleaseFormatWorker__LIB_FRMDB;

    m_pFormatAddOn = RegisterAddOnManager(m_pAddOnManager);
  }
  if (m_nModuleDataFormat == tDataFormatST_Property)
  {
    RegisterAddOnManager = RegisterAddOnManager_LIB_FRMPROP;
    UnregisterAddOnManager = UnregisterAddOnManager_LIB_FRMPROP;
    GetFrmAddOnVersion = GetFrmAddOnVersion_LIB_FRMPROP;
    CreateFormatWorker_ = CreateFormatWorker__LIB_FRMPROP;
    CreateFormatWorkerClone_ = CreateFormatWorkerClone__LIB_FRMPROP;
    ReleaseFormatWorker_ = ReleaseFormatWorker__LIB_FRMPROP;

    m_pFormatAddOn = RegisterAddOnManager(m_pAddOnManager);
  }
  if (m_nModuleDataFormat == tDataFormatST_Tag)
  {
    RegisterAddOnManager = RegisterAddOnManager_LIB_FRMTAG;
    UnregisterAddOnManager = UnregisterAddOnManager_LIB_FRMTAG;
    GetFrmAddOnVersion = GetFrmAddOnVersion_LIB_FRMTAG;
    CreateFormatWorker_ = CreateFormatWorker__LIB_FRMTAG;
    CreateFormatWorkerClone_ = CreateFormatWorkerClone__LIB_FRMTAG;
    ReleaseFormatWorker_ = ReleaseFormatWorker__LIB_FRMTAG;

    m_pFormatAddOn = RegisterAddOnManager(m_pAddOnManager);
  }
  if (m_nModuleDataFormat == tDataFormatST_Html)
  {
    RegisterAddOnManager = RegisterAddOnManager_LIB_FRMHTML;
    UnregisterAddOnManager = UnregisterAddOnManager_LIB_FRMHTML;
    GetFrmAddOnVersion = GetFrmAddOnVersion_LIB_FRMHTML;
    CreateFormatWorker_ = CreateFormatWorker__LIB_FRMHTML;
    CreateFormatWorkerClone_ = CreateFormatWorkerClone__LIB_FRMHTML;
    ReleaseFormatWorker_ = ReleaseFormatWorker__LIB_FRMHTML;

    m_pFormatAddOn = RegisterAddOnManager(m_pAddOnManager);
	}
  if (m_nModuleDataFormat == tDataFormatST_Plain)
  {
    RegisterAddOnManager = RegisterAddOnManager_LIB_FRMPLAIN;
    UnregisterAddOnManager = UnregisterAddOnManager_LIB_FRMPLAIN;
    GetFrmAddOnVersion = GetFrmAddOnVersion_LIB_FRMPLAIN;
    CreateFormatWorker_ = CreateFormatWorker__LIB_FRMPLAIN;
    CreateFormatWorkerClone_ = CreateFormatWorkerClone__LIB_FRMPLAIN;
    ReleaseFormatWorker_ = ReleaseFormatWorker__LIB_FRMPLAIN;

    m_pFormatAddOn = RegisterAddOnManager(m_pAddOnManager);
	}

#else // defined(_LIB)

  if (!m_hAddOnModule)
    return false;

  RegisterAddOnManager = (CFormatAddOn* (*)(CXPubAddOnManager*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("RegisterAddOnManager"));
  UnregisterAddOnManager = (bool (*)(CFormatAddOn*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("UnregisterAddOnManager"));
  GetFrmAddOnVersion = (int (*)())XPUB_GET_F_ADDR(m_hAddOnModule, _XT("GetFrmAddOnVersion"));
  CreateFormatWorker_ = (CFormatWorker* (*)(CFormatAddOn*, TWorkPaperType))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("CreateFormatWorker_"));
  CreateFormatWorkerClone_ = (CFormatWorker* (*)(CFormatAddOn*, CFormatWorker*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("CreateFormatWorkerClone_"));
  ReleaseFormatWorker_ = (bool (*)(CFormatAddOn*, CFormatWorker*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("ReleaseFormatWorker_"));

  if (!RegisterAddOnManager
      || !UnregisterAddOnManager
      || !GetFrmAddOnVersion
      || !CreateFormatWorker_
      || !CreateFormatWorkerClone_
      || !ReleaseFormatWorker_)
    return false;

  m_pFormatAddOn = RegisterAddOnManager(m_pAddOnManager);

#endif // defined(_LIB)

  return (m_pFormatAddOn != 0);
}

CFormatWorker* CFrmAddOnModule::CreateFormatWorker(TWorkPaperType nWorkPaperType)
{
  return CreateFormatWorker_(m_pFormatAddOn, nWorkPaperType);
}

CFormatWorker* CFrmAddOnModule::CreateFormatWorkerClone(CFormatWorker* pFormatWorker)
{
  return CreateFormatWorkerClone_(m_pFormatAddOn, pFormatWorker);
}

bool CFrmAddOnModule::ReleaseFormatWorker(CFormatWorker* pFormatWorker)
{
  return ReleaseFormatWorker_(m_pFormatAddOn, pFormatWorker);
}

bool CFrmAddOnModule::ReleaseAddOn()
{
  if (!m_pFormatAddOn)
    return false;
  UnregisterAddOnManager(m_pFormatAddOn);
  m_pFormatAddOn = 0;
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// CCnvFromAddOnModule
////////////////////////////////////////////////////////////////////////////////
CCnvFromAddOnModule::CCnvFromAddOnModule(ADDONMODULE hAddOnModule,
                                         CXPubAddOnManager* pAddOnManager,
                                         TDataFormatForStoneAndTemplate  nModuleDataFormat)
{
  m_hAddOnModule = hAddOnModule;
  m_pAddOnManager = pAddOnManager;
  m_nModuleDataFormat = nModuleDataFormat;
  m_pConvertAddOn = 0;

  RegisterAddOnManager = 0;
  UnregisterAddOnManager = 0;
  GetCnvFromAddOnVersion = 0;
  CreateConvertWorkerFrom_ = 0;
  CreateConvertWorkerFromClone_ = 0;
  ReleaseConvertWorkerFrom_ = 0;
}

CCnvFromAddOnModule::~CCnvFromAddOnModule()
{
  if (m_pConvertAddOn)
    UnregisterAddOnManager(m_pConvertAddOn);

#if defined(_LIB)

  // nichts machen

#else // defined(_LIB)

#if defined(LINUX_OS)
  dlclose(m_hAddOnModule);
#elif defined(WIN32)
  ::FreeLibrary(m_hAddOnModule);
#elif defined(MAC_OS_MWE)
  CFBundleUnloadExecutable((CFBundleRef)m_hAddOnModule); 
#else
#error "not implemented"
#endif

#endif // defined(_LIB)
}

bool CCnvFromAddOnModule::InitAddOnModule()
{
#if defined(_LIB)

  if (m_nModuleDataFormat == tDataFormatST_Tag)
  {
    RegisterAddOnManager = RegisterAddOnManager_LIB_FTAG;
    UnregisterAddOnManager = UnregisterAddOnManager_LIB_FTAG;
    GetCnvFromAddOnVersion = GetCnvFromAddOnVersion_LIB_FTAG;
    CreateConvertWorkerFrom_ = CreateConvertWorkerFrom__LIB_FTAG;
    CreateConvertWorkerFromClone_ = CreateConvertWorkerFromClone__LIB_FTAG;
    ReleaseConvertWorkerFrom_ = ReleaseConvertWorkerFrom__LIB_FTAG;

    m_pConvertAddOn = RegisterAddOnManager(m_pAddOnManager);
  }

#else // defined(_LIB)

  if (!m_hAddOnModule)
    return false;

  RegisterAddOnManager = (CConvertFromAddOn* (*)(CXPubAddOnManager*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("RegisterAddOnManager"));
  UnregisterAddOnManager = (bool (*)(CConvertFromAddOn*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("UnregisterAddOnManager"));
  GetCnvFromAddOnVersion = (int (*)())XPUB_GET_F_ADDR(m_hAddOnModule, _XT("GetCnvFromAddOnVersion"));
  CreateConvertWorkerFrom_ = (CConvertWorkerFrom* (*)(CConvertFromAddOn*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("CreateConvertWorkerFrom_"));
  CreateConvertWorkerFromClone_ = (CConvertWorkerFrom* (*)(CConvertFromAddOn*, CConvertWorkerFrom*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("CreateConvertWorkerFromClone_"));
  ReleaseConvertWorkerFrom_ = (bool (*)(CConvertFromAddOn*, CConvertWorkerFrom*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("ReleaseConvertWorkerFrom_"));

  if (!RegisterAddOnManager
      || !UnregisterAddOnManager
      || !GetCnvFromAddOnVersion
      || !CreateConvertWorkerFrom_
      || !CreateConvertWorkerFromClone_
      || !ReleaseConvertWorkerFrom_)
    return false;

  m_pConvertAddOn = RegisterAddOnManager(m_pAddOnManager);

#endif // defined(_LIB)

  return (m_pConvertAddOn != 0);
}

CConvertWorkerFrom* CCnvFromAddOnModule::CreateConvertWorkerFrom()
{
  return CreateConvertWorkerFrom_(m_pConvertAddOn);
}

CConvertWorkerFrom* CCnvFromAddOnModule::CreateConvertWorkerFromClone(CConvertWorkerFrom* pConvertWorker)
{
  return CreateConvertWorkerFromClone_(m_pConvertAddOn, pConvertWorker);
}

bool CCnvFromAddOnModule::ReleaseConvertWorkerFrom(CConvertWorkerFrom* pConvertWorker)
{
  return ReleaseConvertWorkerFrom_(m_pConvertAddOn, pConvertWorker);
}

bool CCnvFromAddOnModule::ReleaseAddOn()
{
  if (!m_pConvertAddOn)
    return false;
  UnregisterAddOnManager(m_pConvertAddOn);
  m_pConvertAddOn = 0;
  return true;
}

////////////////////////////////////////////////////////////////////////////////
// CCnvToAddOnModule
////////////////////////////////////////////////////////////////////////////////
CCnvToAddOnModule::CCnvToAddOnModule(ADDONMODULE hAddOnModule,
                                     CXPubAddOnManager* pAddOnManager,
                                     TDataFormatForStoneAndTemplate  nModuleDataFormat)
{
  m_hAddOnModule = hAddOnModule;
  m_pAddOnManager = pAddOnManager;
  m_nModuleDataFormat = nModuleDataFormat;
  m_pConvertAddOn = 0;

  RegisterAddOnManager = 0;
  UnregisterAddOnManager = 0;
  GetCnvToAddOnVersion = 0;
  CreateConvertWorkerTo_ = 0;
  CreateConvertWorkerToClone_ = 0;
  ReleaseConvertWorkerTo_ = 0;
}

CCnvToAddOnModule::~CCnvToAddOnModule()
{
  if (m_pConvertAddOn)
    UnregisterAddOnManager(m_pConvertAddOn);

#if defined(_LIB)

  // nichts machen

#else // defined(_LIB)

#if defined(LINUX_OS)
  dlclose(m_hAddOnModule);
#elif defined(WIN32)
  ::FreeLibrary(m_hAddOnModule);
#elif defined(MAC_OS_MWE)
  CFBundleUnloadExecutable((CFBundleRef)m_hAddOnModule); 
#else
#error "not implemented"
#endif

#endif // defined(_LIB)
}

bool CCnvToAddOnModule::InitAddOnModule()
{
#if defined(_LIB)

  if (m_nModuleDataFormat == tDataFormatST_Pdf)
  {
    RegisterAddOnManager = RegisterAddOnManager_LIB_TPDF;
    UnregisterAddOnManager = UnregisterAddOnManager_LIB_TPDF;
    GetCnvToAddOnVersion = GetCnvToAddOnVersion_LIB_TPDF;
    CreateConvertWorkerTo_ = CreateConvertWorkerTo__LIB_TPDF;
    CreateConvertWorkerToClone_ = CreateConvertWorkerToClone__LIB_TPDF;
    ReleaseConvertWorkerTo_ = ReleaseConvertWorkerTo__LIB_TPDF;

    m_pConvertAddOn = RegisterAddOnManager(m_pAddOnManager);
  }
  if (m_nModuleDataFormat == tDataFormatST_Rtf)
  {
    RegisterAddOnManager = RegisterAddOnManager_LIB_TRTF;
    UnregisterAddOnManager = UnregisterAddOnManager_LIB_TRTF;
    GetCnvToAddOnVersion = GetCnvToAddOnVersion_LIB_TRTF;
    CreateConvertWorkerTo_ = CreateConvertWorkerTo__LIB_TRTF;
    CreateConvertWorkerToClone_ = CreateConvertWorkerToClone__LIB_TRTF;
    ReleaseConvertWorkerTo_ = ReleaseConvertWorkerTo__LIB_TRTF;

    m_pConvertAddOn = RegisterAddOnManager(m_pAddOnManager);
  }

#else // defined(_LIB)

  if (!m_hAddOnModule)
    return false;

  RegisterAddOnManager = (CConvertToAddOn* (*)(CXPubAddOnManager*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("RegisterAddOnManager"));
  UnregisterAddOnManager = (bool (*)(CConvertToAddOn*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("UnregisterAddOnManager"));
  GetCnvToAddOnVersion = (int (*)())XPUB_GET_F_ADDR(m_hAddOnModule, _XT("GetCnvToAddOnVersion"));
  CreateConvertWorkerTo_ = (CConvertWorkerTo* (*)(CConvertToAddOn*, HWND))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("CreateConvertWorkerTo_"));
  CreateConvertWorkerToClone_ = (CConvertWorkerTo* (*)(CConvertToAddOn*, CConvertWorkerTo*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("CreateConvertWorkerToClone_"));
  ReleaseConvertWorkerTo_ = (bool (*)(CConvertToAddOn*, CConvertWorkerTo*))XPUB_GET_F_ADDR(m_hAddOnModule, _XT("ReleaseConvertWorkerTo_"));

  if (!RegisterAddOnManager
      || !UnregisterAddOnManager
      || !GetCnvToAddOnVersion
      || !CreateConvertWorkerTo_
      || !CreateConvertWorkerToClone_
      || !ReleaseConvertWorkerTo_)
    return false;


  m_pConvertAddOn = RegisterAddOnManager(m_pAddOnManager);

#endif // defined(_LIB)

  return (m_pConvertAddOn != 0);
}

CConvertWorkerTo* CCnvToAddOnModule::CreateConvertWorkerTo(HWND hWndParent)
{
  return CreateConvertWorkerTo_(m_pConvertAddOn, hWndParent);
}

CConvertWorkerTo* CCnvToAddOnModule::CreateConvertWorkerToClone(CConvertWorkerTo* pConvertWorker)
{
  return CreateConvertWorkerToClone_(m_pConvertAddOn, pConvertWorker);
}

bool CCnvToAddOnModule::ReleaseConvertWorkerTo(CConvertWorkerTo* pConvertWorker)
{
  return ReleaseConvertWorkerTo_(m_pConvertAddOn, pConvertWorker);
}

bool CCnvToAddOnModule::ReleaseAddOn()
{
  if (!m_pConvertAddOn)
    return false;
  UnregisterAddOnManager(m_pConvertAddOn);
  m_pConvertAddOn = 0;
  return true;
}















////////////////////////////////////////////////////////////////////////////////
// CXPubAddOnManager
////////////////////////////////////////////////////////////////////////////////
CXPubAddOnManager::CXPubAddOnManager(LPCXTCHAR pSyncSemaphoreName)
{
  if (pSyncSemaphoreName)
    m_sSyncSemaphoreName = pSyncSemaphoreName;
}

CXPubAddOnManager::~CXPubAddOnManager()
{
  ReleaseAllAddOns();
}

void CXPubAddOnManager::GetModulePath(xtstring& sPath)
{

#if !defined(_LIB)

#if defined(LINUX_OS)

#elif defined(WIN32)
  
  char cPath[_MAX_PATH];
  extern HINSTANCE g_hinst;

  ::GetModuleFileName(g_hinst, cPath, _MAX_PATH);
  sPath = cPath;
  size_t nPos1 = sPath.find_last_of(_XT("\\/"));
  if (nPos1 != xtstring::npos)
  {
    sPath.erase(nPos1);
    sPath += "\\";
  }
#elif defined(MAC_OS_MWE)

#else
#error "not implemented"
#endif

#endif // !defined(_LIB)
}

bool CXPubAddOnManager::InitManager()
{
  ADDONMODULE hModule;
  xtstring sPath;
  GetModulePath(sPath);

#if !defined(MAC_OS_MWE)

  // sync
  CSFSemaphore cSyncObject;
  if (m_sSyncSemaphoreName.size())
    cSyncObject.Open(m_sSyncSemaphoreName.c_str());
  if (cSyncObject.Created())
    cSyncObject.Wait();

#endif // !defined(MAC_OS_MWE)


#define ENV_VAR_MAX 32767 /* environment variable max is 32,767 bytes */
#define DLL_PATH_MAX 1024


#if defined(LINUX_OS)

  // Zur Zeit weiss ich nicht, was wir da machen sollen.
  // Auf jeden Fall hat kein Sinn mit PATH spielen.
  // Wenn, dann mit LD_LIBRARY_PATH.

#elif defined(MAC_OS_MWE)

#elif defined(WIN32)

#if !defined(_LIB)
  char current_path[ENV_VAR_MAX];
  long count = GetEnvironmentVariable("PATH", current_path, ENV_VAR_MAX);
  if (strstr(current_path,sPath.c_str()) == 0)
  {
    if (strlen(sPath.c_str()) + count + 1 < ENV_VAR_MAX - 1)
    {
      char new_path[ENV_VAR_MAX];

      sprintf(new_path, "%.*s;%.*s", DLL_PATH_MAX, sPath.c_str(),
              ENV_VAR_MAX - DLL_PATH_MAX - 1, current_path);
      SetEnvironmentVariable("PATH", new_path);
    }
  }
#endif // !defined(_LIB)

#else
#error "not implemented"
#endif

  xtstring sModule;
////////////////////////////////////////////////////////////////////////////////
// format addons
////////////////////////////////////////////////////////////////////////////////

  //////////////////////////
  // Html Frm AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubFrmHtml"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CFrmAddOnModule* pFrmModule = new CFrmAddOnModule(hModule, this, tDataFormatST_Html);
    if (pFrmModule)
    {
      if (pFrmModule->InitAddOnModule())
        m_cFrmAddOns.push_back(pFrmModule);
      else
        delete pFrmModule;
    }
  }
  //////////////////////////
  // Plain Frm AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubFrmPlain"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CFrmAddOnModule* pFrmModule = new CFrmAddOnModule(hModule, this, tDataFormatST_Plain);
    if (pFrmModule)
    {
      if (pFrmModule->InitAddOnModule())
        m_cFrmAddOns.push_back(pFrmModule);
      else
        delete pFrmModule;
    }
  }
  //////////////////////////
  // Rtf Frm AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubFrmRtf"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CFrmAddOnModule* pFrmModule = new CFrmAddOnModule(hModule, this, tDataFormatST_Rtf);
    if (pFrmModule)
    {
      if (pFrmModule->InitAddOnModule())
        m_cFrmAddOns.push_back(pFrmModule);
      else
        delete pFrmModule;
    }
  }
  //////////////////////////
  // Tag Frm AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubFrmTag"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CFrmAddOnModule* pFrmModule = new CFrmAddOnModule(hModule, this, tDataFormatST_Tag);
    if (pFrmModule)
    {
      if (pFrmModule->InitAddOnModule())
        m_cFrmAddOns.push_back(pFrmModule);
      else
        delete pFrmModule;
    }
  }
  //////////////////////////
  // Db Frm AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubFrmDb"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CFrmAddOnModule* pFrmModule = new CFrmAddOnModule(hModule, this, tDataFormatST_Db);
    if (pFrmModule)
    {
      if (pFrmModule->InitAddOnModule())
        m_cFrmAddOns.push_back(pFrmModule);
      else
        delete pFrmModule;
    }
  }
  //////////////////////////
  // Prop Frm AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubFrmProp"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CFrmAddOnModule* pFrmModule = new CFrmAddOnModule(hModule, this, tDataFormatST_Property);
    if (pFrmModule)
    {
      if (pFrmModule->InitAddOnModule())
        m_cFrmAddOns.push_back(pFrmModule);
      else
        delete pFrmModule;
    }
  }
  //////////////////////////
  // Xml Frm AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubFrmXml"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CFrmAddOnModule* pFrmModule = new CFrmAddOnModule(hModule, this, tDataFormatST_Xml);
    if (pFrmModule)
    {
      if (pFrmModule->InitAddOnModule())
        m_cFrmAddOns.push_back(pFrmModule);
      else
        delete pFrmModule;
    }
  }

////////////////////////////////////////////////////////////////////////////////
// convert from addons
////////////////////////////////////////////////////////////////////////////////

  //////////////////////////
  // Tag Cnv AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubCnvFTag"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CCnvFromAddOnModule* pCnvModule = new CCnvFromAddOnModule(hModule, this, tDataFormatST_Tag);
    if (pCnvModule)
    {
      if (pCnvModule->InitAddOnModule())
        m_cCnvFromAddOns.push_back(pCnvModule);
      else
        delete pCnvModule;
    }
  }
  //////////////////////////
  // Rtf Cnv AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubCnvFRtf"), SHARED_EXTENSION);
  hModule = 0;//LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CCnvFromAddOnModule* pCnvModule = new CCnvFromAddOnModule(hModule, this, tDataFormatST_Rtf);
    if (pCnvModule)
    {
      if (pCnvModule->InitAddOnModule())
        m_cCnvFromAddOns.push_back(pCnvModule);
      else
        delete pCnvModule;
    }
  }

////////////////////////////////////////////////////////////////////////////////
// convert to addons
////////////////////////////////////////////////////////////////////////////////

  //////////////////////////
  // Rtf Cnv AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubCnvTRtf"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CCnvToAddOnModule* pCnvModule = new CCnvToAddOnModule(hModule, this, tDataFormatST_Rtf);
    if (pCnvModule)
    {
      if (pCnvModule->InitAddOnModule())
        m_cCnvToAddOns.push_back(pCnvModule);
      else
        delete pCnvModule;
    }
  }

  //////////////////////////
  // Pdf Cnv AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubCnvTPdf"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CCnvToAddOnModule* pCnvModule = new CCnvToAddOnModule(hModule, this, tDataFormatST_Pdf);
    if (pCnvModule)
    {
      if (pCnvModule->InitAddOnModule())
        m_cCnvToAddOns.push_back(pCnvModule);
      else
        delete pCnvModule;
    }
  }

  //////////////////////////
  // Word Cnv AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubCnvTWord"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CCnvToAddOnModule* pCnvModule = new CCnvToAddOnModule(hModule, this, tDataFormatST_Word);
    if (pCnvModule)
    {
      if (pCnvModule->InitAddOnModule())
        m_cCnvToAddOns.push_back(pCnvModule);
      else
        delete pCnvModule;
    }
  }

  //////////////////////////
  // Outlook Cnv AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubCnvTOutlook"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CCnvToAddOnModule* pCnvModule = new CCnvToAddOnModule(hModule, this, tDataFormatST_Outlook);
    if (pCnvModule)
    {
      if (pCnvModule->InitAddOnModule())
        m_cCnvToAddOns.push_back(pCnvModule);
      else
        delete pCnvModule;
    }
  }

  //////////////////////////
  // InD Cnv AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubCnvTInD"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CCnvToAddOnModule* pCnvModule = new CCnvToAddOnModule(hModule, this, tDataFormatST_InDesign);
    if (pCnvModule)
    {
      if (pCnvModule->InitAddOnModule())
        m_cCnvToAddOns.push_back(pCnvModule);
      else
        delete pCnvModule;
    }
  }

  //////////////////////////
  // Fdf Cnv AddOn
  sModule.Format(_XT("%s%s%s%s"), sPath.c_str(), SHARED_PREFIX, _XT("XPubCnvTFdf"), SHARED_EXTENSION);
  hModule = LoadAddOnLibrary(sModule);
  if (hModule)
  {
    CCnvToAddOnModule* pCnvModule = new CCnvToAddOnModule(hModule, this, tDataFormatST_Fdf);
    if (pCnvModule)
    {
      if (pCnvModule->InitAddOnModule())
        m_cCnvToAddOns.push_back(pCnvModule);
      else
        delete pCnvModule;
    }
  }

#if defined(_DEBUG) && defined(_WIN32) && !defined(_LIB)
  xtstring sApp = g_app;
  size_t nPos = sApp.find(_XT("XPubAddOnMan.dll"));
  if (nPos != xtstring::npos)
    sApp.replace(nPos, sApp.size() - nPos, _XT("XPubAddOnMan.log"));
  else
    sApp += _XT("XPubAddOnMan.log");

  FILE* fp = fopen(sApp.c_str(), _XT("w"));
  if (fp)
  {
    {
      time_t now;
      time(&now);
        XTCHAR buff[100];
        buff[0] = 0;
        struct tm* ptmTemp = localtime(&now);
        if (ptmTemp)
          strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
      fprintf(fp, _XT("Created: %s\n"), buff);
    }

    char module[MAX_PATH];
    fprintf(fp, _XT("Format Worker AddOns\n"));
    for (CFrmAddOnsIterator it = m_cFrmAddOns.begin(); it != m_cFrmAddOns.end(); it++)
    {
      ::GetModuleFileName((*it)->GetAddOnModuleHandle(), module, MAX_PATH);
      fprintf(fp, _XT("\t%s\n"), module);
    }
    fprintf(fp, _XT("Convert Worker From AddOns\n"));
    for (CCnvFromAddOnsIterator it = m_cCnvFromAddOns.begin(); it != m_cCnvFromAddOns.end(); it++)
    {
      ::GetModuleFileName((*it)->GetAddOnModuleHandle(), module, MAX_PATH);
      fprintf(fp, _XT("\t%s\n"), module);
    }
    fprintf(fp, _XT("Convert Worker To AddOns\n"));
    for (CCnvToAddOnsIterator it = m_cCnvToAddOns.begin(); it != m_cCnvToAddOns.end(); it++)
    {
      ::GetModuleFileName((*it)->GetAddOnModuleHandle(), module, MAX_PATH);
      fprintf(fp, _XT("\t%s\n"), module);
    }
    fclose(fp);
  }

#endif // defined(_DEBUG) && defined(_WIN32) && !defined(_LIB)

#if defined(WIN32) && !defined(_LIB)
 // set back to the path before
  if(count)
    SetEnvironmentVariable("PATH", current_path);
#endif // defined(WIN32) && !defined(_LIB)

#if !defined(MAC_OS_MWE)

  // sync
  if (cSyncObject.Created())
    cSyncObject.Release();

#endif // !defined(MAC_OS_MWE)

  return true;
}

void CXPubAddOnManager::ReleaseAllAddOns()
{

#if !defined(MAC_OS_MWE)

  // sync
  CSFSemaphore cSyncObject;
  if (m_sSyncSemaphoreName.size())
    cSyncObject.Open(m_sSyncSemaphoreName.c_str());
  if (cSyncObject.Created())
    cSyncObject.Wait();

 #endif 

  {
    CFrmAddOnsIterator it = m_cFrmAddOns.begin();
    while (it != m_cFrmAddOns.end())
    {
      (*it)->ReleaseAddOn();
      delete (*it);
      it++;
    }
    m_cFrmAddOns.erase(m_cFrmAddOns.begin(), m_cFrmAddOns.end());
  }

  {
    CCnvFromAddOnsIterator it = m_cCnvFromAddOns.begin();
    while (it != m_cCnvFromAddOns.end())
    {
      (*it)->ReleaseAddOn();
      delete (*it);
      it++;
    }
    m_cCnvFromAddOns.erase(m_cCnvFromAddOns.begin(), m_cCnvFromAddOns.end());
  }

  {
    CCnvToAddOnsIterator it = m_cCnvToAddOns.begin();
    while (it != m_cCnvToAddOns.end())
    {
      (*it)->ReleaseAddOn();
      delete (*it);
      it++;
    }
    m_cCnvToAddOns.erase(m_cCnvToAddOns.begin(), m_cCnvToAddOns.end());
  }

#if !defined(MAC_OS_MWE)

  // sync
  if (cSyncObject.Created())
    cSyncObject.Release();

#endif  
}






////////////////////////
// Format Worker Add Ons
////////////////////////
bool CXPubAddOnManager::FormatWorkerAvailable(TDataFormatForStoneAndTemplate nWorkPaperFormat)
{
  CFrmAddOnsIterator it = m_cFrmAddOns.begin();
  while (it != m_cFrmAddOns.end())
  {
    if ((*it)->GetWorkPaperDataFormat() == nWorkPaperFormat)
      return true;
    it++;
  }
  return false;
}

CFormatWorker* CXPubAddOnManager::CreateFormatWorker(TDataFormatForStoneAndTemplate nWorkPaperFormat,
                                                     TWorkPaperType nWorkPaperType)
{
  CFrmAddOnsIterator it = m_cFrmAddOns.begin();
  while (it != m_cFrmAddOns.end())
  {
    if ((*it)->GetWorkPaperDataFormat() == nWorkPaperFormat)
      return (*it)->CreateFormatWorker(nWorkPaperType);
    it++;
  }
  return 0;
}

CFormatWorker* CXPubAddOnManager::CreateFormatWorkerClone(CFormatWorker* pFormatWorker)
{
  CFrmAddOnsIterator it = m_cFrmAddOns.begin();
  while (it != m_cFrmAddOns.end())
  {
    if (pFormatWorker && (*it)->GetWorkPaperDataFormat() == pFormatWorker->GetWorkPaperDataFormat())
      return (*it)->CreateFormatWorkerClone(pFormatWorker);
    it++;
  }
  return 0;
}

bool CXPubAddOnManager::ReleaseFormatWorker(CFormatWorker* pFormatWorker)
{
  CFrmAddOnsIterator it = m_cFrmAddOns.begin();
  while (it != m_cFrmAddOns.end())
  {
    if (pFormatWorker && (*it)->GetWorkPaperDataFormat() == pFormatWorker->GetWorkPaperDataFormat())
      return (*it)->ReleaseFormatWorker(pFormatWorker);
    it++;
  }

  return false;
}

////////////////////////
// Convert Worker From Add Ons
////////////////////////
bool CXPubAddOnManager::ConvertWorkerFromAvailable(TDataFormatForStoneAndTemplate nWorkPaperFormat)
{
  CCnvFromAddOnsIterator it = m_cCnvFromAddOns.begin();
  while (it != m_cCnvFromAddOns.end())
  {
    if ((*it)->GetWorkPaperDataFormat() == nWorkPaperFormat)
      return true;
    it++;
  }
  return false;
}

CConvertWorkerFrom* CXPubAddOnManager::CreateConvertWorkerFrom(TDataFormatForStoneAndTemplate nWorkPaperFormat)
{
  CCnvFromAddOnsIterator it = m_cCnvFromAddOns.begin();
  while (it != m_cCnvFromAddOns.end())
  {
    if ((*it)->GetWorkPaperDataFormat() == nWorkPaperFormat)
      return (*it)->CreateConvertWorkerFrom();
    it++;
  }
  return 0;
}

CConvertWorkerFrom* CXPubAddOnManager::CreateConvertWorkerFromClone(CConvertWorkerFrom* pConvertWorker)
{
  CCnvFromAddOnsIterator it = m_cCnvFromAddOns.begin();
  while (it != m_cCnvFromAddOns.end())
  {
    if (pConvertWorker && (*it)->GetWorkPaperDataFormat() == pConvertWorker->GetDataFormat())
      return (*it)->CreateConvertWorkerFromClone(pConvertWorker);
    it++;
  }
  return 0;
}

bool CXPubAddOnManager::ReleaseConvertWorkerFrom(CConvertWorkerFrom* pConvertWorker)
{
  CCnvFromAddOnsIterator it = m_cCnvFromAddOns.begin();
  while (it != m_cCnvFromAddOns.end())
  {
    if (pConvertWorker && (*it)->GetWorkPaperDataFormat() == pConvertWorker->GetDataFormat())
      return (*it)->ReleaseConvertWorkerFrom(pConvertWorker);
    it++;
  }

  return false;
}

////////////////////////
// Convert Worker To Add Ons
////////////////////////
bool CXPubAddOnManager::ConvertWorkerToAvailable(TDataFormatForStoneAndTemplate nWorkPaperFormat)
{
  CCnvToAddOnsIterator it = m_cCnvToAddOns.begin();
  while (it != m_cCnvToAddOns.end())
  {
    if ((*it)->GetWorkPaperDataFormat() == nWorkPaperFormat)
      return true;
    it++;
  }
  return false;
}

CConvertWorkerTo* CXPubAddOnManager::CreateConvertWorkerTo(TDataFormatForStoneAndTemplate nWorkPaperFormat, HWND hWndParent)
{
  CCnvToAddOnsIterator it = m_cCnvToAddOns.begin();
  while (it != m_cCnvToAddOns.end())
  {
    if ((*it)->GetWorkPaperDataFormat() == nWorkPaperFormat)
      return (*it)->CreateConvertWorkerTo(hWndParent);
    it++;
  }
  return 0;
}

CConvertWorkerTo* CXPubAddOnManager::CreateConvertWorkerToClone(CConvertWorkerTo* pConvertWorker)
{
  CCnvToAddOnsIterator it = m_cCnvToAddOns.begin();
  while (it != m_cCnvToAddOns.end())
  {
    if (pConvertWorker && (*it)->GetWorkPaperDataFormat() == pConvertWorker->GetDataFormat())
      return (*it)->CreateConvertWorkerToClone(pConvertWorker);
    it++;
  }
  return 0;
}

bool CXPubAddOnManager::ReleaseConvertWorkerTo(CConvertWorkerTo* pConvertWorker)
{
  CCnvToAddOnsIterator it = m_cCnvToAddOns.begin();
  while (it != m_cCnvToAddOns.end())
  {
    if (pConvertWorker && (*it)->GetWorkPaperDataFormat() == pConvertWorker->GetDataFormat())
      return (*it)->ReleaseConvertWorkerTo(pConvertWorker);
    it++;
  }

  return false;
}




CFormatWorker* CXPubAddOnManager::ReadWorkPaperFromMemory(const xtstring& sXMLMemoryContent)
{
  CFormatWorker* pRet = 0;

  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(sXMLMemoryContent.c_str()))
    return pRet;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == MAIN_TAG_WORKPAPERCONTENT)
  {
    if (cXMLDoc.FindChildElem(WORKPAPER_1STAGE))
    {
      // into 1. stage
      cXMLDoc.IntoElem();

      // pre set values
      TDataFormatForStoneAndTemplate nWorkPaperFormat = (TDataFormatForStoneAndTemplate)cXMLDoc.GetAttribLong(WORKPAPER_FORMAT);
      TWorkPaperType nWorkPaperType = (TWorkPaperType)cXMLDoc.GetAttribLong(WORKPAPER_TYPE);

      pRet = CreateFormatWorker(nWorkPaperFormat, nWorkPaperType);
      if (!pRet)
        return pRet;

      if (!pRet->ReadFromXMLMemoryContent(0, &cXMLDoc))
      {
        ReleaseFormatWorker(pRet);
        pRet = 0;
        return pRet;
      }

      // out of 1. stage
      cXMLDoc.OutOfElem();
    }
  }
  return pRet;
}
CFormatWorker* CXPubAddOnManager::ReadWorkPaperFromMemory(CXPubMarkUp* pXMLDoc)
{
  CFormatWorker* pRet = 0;
  if (pXMLDoc->FindChildElem(WORKPAPER_1STAGE))
  {
    // into 1. stage
    pXMLDoc->IntoElem();

    // pre set values
    TDataFormatForStoneAndTemplate nWorkPaperFormat = (TDataFormatForStoneAndTemplate)pXMLDoc->GetAttribLong(WORKPAPER_FORMAT);
    TWorkPaperType nWorkPaperType = (TWorkPaperType)pXMLDoc->GetAttribLong(WORKPAPER_TYPE);

    pRet = CreateFormatWorker(nWorkPaperFormat, nWorkPaperType);
    if (!pRet)
      return pRet;

    if (!pRet->ReadFromXMLMemoryContent(0, pXMLDoc))
    {
      ReleaseFormatWorker(pRet);
      pRet = 0;
      return pRet;
    }

    // out of 1. stage
    pXMLDoc->OutOfElem();
  }
  return pRet;
}
CFormatWorkerWrapper_Template* CXPubAddOnManager::CreateTemplateWrapperClass(CFormatWorkerWrapper_Root* pRoot,
                                                                             const xtstring& sXMLMemoryContent)
{
  CFormatWorkerWrapper_Template* pRet = 0;

  CXPubMarkUp cXMLDoc;
  if (!cXMLDoc.SetDoc(sXMLMemoryContent.c_str()))
    return pRet;

  cXMLDoc.ResetPos();
  if (cXMLDoc.FindElem() && cXMLDoc.GetTagName() == MAIN_TAG_WORKPAPERCONTENT)
  {
    if (cXMLDoc.FindChildElem(WORKPAPER_1STAGE))
    {
      // into 1. stage
      cXMLDoc.IntoElem();

      // pre set values
      TDataFormatForStoneAndTemplate nWorkPaperFormat = (TDataFormatForStoneAndTemplate)cXMLDoc.GetAttribLong(WORKPAPER_FORMAT);
      TWorkPaperType nWorkPaperType = (TWorkPaperType)cXMLDoc.GetAttribLong(WORKPAPER_TYPE);

      CFormatWorker* pFmt = CreateFormatWorker(nWorkPaperFormat, nWorkPaperType);
      if (!pFmt)
        return pRet;
      pRet = new CFormatWorkerWrapper_Template(pRoot, 0);
      if (!pRet)
      {
        ReleaseFormatWorker(pFmt);
        return pRet;
      }

      if (!pFmt->ReadFromXMLMemoryContent(pRet, &cXMLDoc))
      {
        ReleaseFormatWorker(pFmt);
        pRet->SetFormatWorker(0);
        delete pRet;
        pRet = 0;
        return pRet;
      }
      pRet->SetFormatWorker(pFmt);

      // out of 1. stage
      cXMLDoc.OutOfElem();
    }
  }
  return pRet;
}

