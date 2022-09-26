#if !defined(_XPUBADDONMANAGER_H_)
#define _XPUBADDONMANAGER_H_


#if defined(LINUX_OS)

typedef void* ADDONMODULE;

#elif defined(WIN32)

#include "windows.h"
typedef HMODULE ADDONMODULE;

#elif defined(MAC_OS_MWE)

typedef void* ADDONMODULE;

#else
#error "not implemented"
#endif


#include "XPubAddOnDeclarations.h"
#include "FormatWorker.h"
#include "ConvertWorker.h"
#include "FormatWorkerWrappers.h"

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


class CFrmAddOnModule;
class CCnvFromAddOnModule;
class CCnvToAddOnModule;
class CXPubAddOnManager;

typedef std::vector<CFrmAddOnModule*>     CFrmAddOns;
typedef CFrmAddOns::iterator              CFrmAddOnsIterator;
typedef CFrmAddOns::const_iterator        CFrmAddOnsConstIterator;
typedef CFrmAddOns::reverse_iterator      CFrmAddOnsReverseIterator;

typedef std::vector<CCnvFromAddOnModule*> CCnvFromAddOns;
typedef CCnvFromAddOns::iterator          CCnvFromAddOnsIterator;
typedef CCnvFromAddOns::const_iterator    CCnvFromAddOnsConstIterator;
typedef CCnvFromAddOns::reverse_iterator  CCnvFromAddOnsReverseIterator;

typedef std::vector<CCnvToAddOnModule*>   CCnvToAddOns;
typedef CCnvToAddOns::iterator            CCnvToAddOnsIterator;
typedef CCnvToAddOns::const_iterator      CCnvToAddOnsConstIterator;
typedef CCnvToAddOns::reverse_iterator    CCnvToAddOnsReverseIterator;

////////////////////////////////////////////////////////////////////////////////
// Klasse CFrmAddOnModule ist eine Wrapper Klasse fuer ein Format AddOn
// Fuer jedes gefundenes AddOn ist eine Instanz dieser Klasse angelegt
////////////////////////////////////////////////////////////////////////////////
class CFrmAddOnModule
{
public:
  CFrmAddOnModule(ADDONMODULE hAddOnModule,
                  CXPubAddOnManager* pAddOnManager,
                  TDataFormatForStoneAndTemplate  nModuleDataFormat);
  ~CFrmAddOnModule();
  bool InitAddOnModule();
  TDataFormatForStoneAndTemplate GetWorkPaperDataFormat(){return m_nModuleDataFormat;};

  CFormatWorker* CreateFormatWorker(TWorkPaperType nWorkPaperType);
  CFormatWorker* CreateFormatWorkerClone(CFormatWorker* pFormatWorker);
  bool ReleaseFormatWorker(CFormatWorker* pFormatWorker);

  ADDONMODULE GetAddOnModuleHandle(){return m_hAddOnModule;};

  bool ReleaseAddOn();

protected:
  ADDONMODULE                     m_hAddOnModule;
  TDataFormatForStoneAndTemplate  m_nModuleDataFormat;

private:
  CXPubAddOnManager*  m_pAddOnManager;
  CFormatAddOn*       m_pFormatAddOn;

  CFormatAddOn* (*RegisterAddOnManager)(CXPubAddOnManager* pManager);
  bool (*UnregisterAddOnManager)(CFormatAddOn* pFrmAddOn);
  int (*GetFrmAddOnVersion)();
  CFormatWorker* (*CreateFormatWorker_)(CFormatAddOn* pFrmAddOn, TWorkPaperType nWorkPaperType);
  CFormatWorker* (*CreateFormatWorkerClone_)(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker);
  bool (*ReleaseFormatWorker_)(CFormatAddOn* pFrmAddOn, CFormatWorker* pFormatWorker);
};

////////////////////////////////////////////////////////////////////////////////
// Klasse CCnvFromAddOnModule ist eine Wrapper Klasse fuer ein Convert From AddOn
// Fuer jedes gefundenes AddOn ist eine Instanz dieser Klasse angelegt
////////////////////////////////////////////////////////////////////////////////
class CCnvFromAddOnModule
{
public:
  CCnvFromAddOnModule(ADDONMODULE hAddOnModule,
                      CXPubAddOnManager* pAddOnManager,
                      TDataFormatForStoneAndTemplate  nModuleDataFormat);
  ~CCnvFromAddOnModule();
  bool InitAddOnModule();
  TDataFormatForStoneAndTemplate GetWorkPaperDataFormat(){return m_nModuleDataFormat;};

  CConvertWorkerFrom* CreateConvertWorkerFrom();
  CConvertWorkerFrom* CreateConvertWorkerFromClone(CConvertWorkerFrom* pConvertWorker);
  bool ReleaseConvertWorkerFrom(CConvertWorkerFrom* pConvertWorker);

  ADDONMODULE GetAddOnModuleHandle(){return m_hAddOnModule;};

  bool ReleaseAddOn();

protected:
  ADDONMODULE                     m_hAddOnModule;
  TDataFormatForStoneAndTemplate  m_nModuleDataFormat;

private:
  CXPubAddOnManager*  m_pAddOnManager;
  CConvertFromAddOn*  m_pConvertAddOn;

  CConvertFromAddOn* (*RegisterAddOnManager)(CXPubAddOnManager* pManager);
  bool (*UnregisterAddOnManager)(CConvertFromAddOn* pCnvAddOn);
  int (*GetCnvFromAddOnVersion)();
  CConvertWorkerFrom* (*CreateConvertWorkerFrom_)(CConvertFromAddOn* pCnvAddOn);
  CConvertWorkerFrom* (*CreateConvertWorkerFromClone_)(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker);
  bool (*ReleaseConvertWorkerFrom_)(CConvertFromAddOn* pCnvAddOn, CConvertWorkerFrom* pConvertWorker);
};

////////////////////////////////////////////////////////////////////////////////
// Klasse CCnvToAddOnModule ist eine Wrapper Klasse fuer ein Convert To AddOn
// Fuer jedes gefundenes AddOn ist eine Instanz dieser Klasse angelegt
////////////////////////////////////////////////////////////////////////////////
class CCnvToAddOnModule
{
public:
  CCnvToAddOnModule(ADDONMODULE hAddOnModule,
                    CXPubAddOnManager* pAddOnManager,
                    TDataFormatForStoneAndTemplate  nModuleDataFormat);
  ~CCnvToAddOnModule();
  bool InitAddOnModule();
  TDataFormatForStoneAndTemplate GetWorkPaperDataFormat(){return m_nModuleDataFormat;};

  CConvertWorkerTo* CreateConvertWorkerTo(HWND hWndParent);
  CConvertWorkerTo* CreateConvertWorkerToClone(CConvertWorkerTo* pConvertWorker);
  bool ReleaseConvertWorkerTo(CConvertWorkerTo* pConvertWorker);

  ADDONMODULE GetAddOnModuleHandle(){return m_hAddOnModule;};

  bool ReleaseAddOn();

protected:
  ADDONMODULE                     m_hAddOnModule;
  TDataFormatForStoneAndTemplate  m_nModuleDataFormat;

private:
  CXPubAddOnManager*  m_pAddOnManager;
  CConvertToAddOn*    m_pConvertAddOn;

  CConvertToAddOn* (*RegisterAddOnManager)(CXPubAddOnManager* pManager);
  bool (*UnregisterAddOnManager)(CConvertToAddOn* pCnvAddOn);
  int (*GetCnvToAddOnVersion)();
  CConvertWorkerTo* (*CreateConvertWorkerTo_)(CConvertToAddOn* pCnvAddOn, HWND hWndParent);
  CConvertWorkerTo* (*CreateConvertWorkerToClone_)(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker);
  bool (*ReleaseConvertWorkerTo_)(CConvertToAddOn* pCnvAddOn, CConvertWorkerTo* pConvertWorker);
};







class XPUBADDONMANAGER_EXPORTIMPORT CXPubAddOnManager
{
public:
  CXPubAddOnManager(LPCXTCHAR pSyncSemaphoreName);
  ~CXPubAddOnManager();
  bool InitManager();
  void ReleaseAllAddOns();

  // Format Worker Add Ons
  bool FormatWorkerAvailable(TDataFormatForStoneAndTemplate nWorkPaperFormat);
  CFormatWorker* CreateFormatWorker(TDataFormatForStoneAndTemplate nWorkPaperFormat, TWorkPaperType nWorkPaperType);
  CFormatWorker* CreateFormatWorkerClone(CFormatWorker* pFormatWorker);
  bool ReleaseFormatWorker(CFormatWorker* pFormatWorker);

  // Convert Worker Add Ons
  bool ConvertWorkerFromAvailable(TDataFormatForStoneAndTemplate nWorkPaperFormat);
  CConvertWorkerFrom* CreateConvertWorkerFrom(TDataFormatForStoneAndTemplate nWorkPaperFormat);
  CConvertWorkerFrom* CreateConvertWorkerFromClone(CConvertWorkerFrom* pConvertWorker);
  bool ReleaseConvertWorkerFrom(CConvertWorkerFrom* pConvertWorker);

  bool ConvertWorkerToAvailable(TDataFormatForStoneAndTemplate nWorkPaperFormat);
  CConvertWorkerTo* CreateConvertWorkerTo(TDataFormatForStoneAndTemplate nWorkPaperFormat, HWND hWndParent);
  CConvertWorkerTo* CreateConvertWorkerToClone(CConvertWorkerTo* pConvertWorker);
  bool ReleaseConvertWorkerTo(CConvertWorkerTo* pConvertWorker);

  CFormatWorker* ReadWorkPaperFromMemory(const xtstring& sXMLMemoryContent);
  CFormatWorker* ReadWorkPaperFromMemory(CXPubMarkUp* pXMLDoc);
  CFormatWorkerWrapper_Template* CreateTemplateWrapperClass(CFormatWorkerWrapper_Root* pRoot, const xtstring& sXMLMemoryContent);

protected:
  void GetModulePath(xtstring& sPath);
  // Format Worker Add Ons
  // Convert Worker Add Ons

private:
  xtstring m_sSyncSemaphoreName;
  // Format Worker Add Ons
  CFrmAddOns      m_cFrmAddOns;
  // Convert Worker Add Ons
  CCnvFromAddOns  m_cCnvFromAddOns;
  CCnvToAddOns    m_cCnvToAddOns;
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_XPUBADDONMANAGER_H_)

