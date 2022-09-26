#if !defined(_WORKPAPERPROCESSOR_H_)
#define _WORKPAPERPROCESSOR_H_


#include "AddOnManagerExpImp.h"

// base STL defines
#include "SSTLdef/STLdef.h"

// XML support
#include "Sxml/MarkupSTL.h"
#include "Sxml/XMLTagNames.h"

// model {data} classes
#include "SModelInd/ModelDef.h"
#include "SModelInd/ModelInd.h"
#include "SModelInd/ERModelInd.h"
#include "SModelInd/IFSEntityInd.h"
#include "SModelInd/MPModelFieldsInd.h"
#include "SModelInd/MPModelERModelInd.h"
#include "SModelInd/MPModelIFSInd.h"
#include "SModelInd/MPModelStoneInd.h"
#include "SModelInd/MPModelTemplateInd.h"
#include "SModelInd/MPModelPythonInd.h"
#include "SModelInd/Constants.h"
#include "SModelInd/DBDefinitions.h"
#include "SModelInd/ProjectFilesInd.h"
#include "SModelInd/ProgressFeature.h"
#include "SSyncEng/SyncEngine.h"

// addon
#include "XPubAddOnManager.h"

#pragma  warning(disable:4251)

class CWorkPaperProcessor;
class CTemplateProcessor;
class CStoneProcessor;


class XPUBADDONMANAGER_EXPORTIMPORT CWPP_ErrorClass
{
public:
  CWPP_ErrorClass(){};
  ~CWPP_ErrorClass(){};

  size_t CountOfErrors(){return m_cErrors.size();};
  xtstring GetError(size_t nI)
  {
    if (nI < 0 || nI >= CountOfErrors())
      return _XT("");
    return m_cErrors[nI];
  }
  void AddError(const xtstring& sError){m_cErrors.push_back(sError);};

  void ResetErrors(){m_cErrors.clear();};

protected:
  CxtstringVector m_cErrors;
};

class XPUBADDONMANAGER_EXPORTIMPORT CWPP_Settings : public CWPP_ErrorClass
{
public:
  CWPP_Settings(){m_bUseElementID_Field = true; m_bUseElementID_Stone = true; m_bUseElementID_Template = true;};

  bool m_bUseElementID_Field;
  bool m_bUseElementID_Stone;
  bool m_bUseElementID_Template;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// base
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CPropProcessor : public CPropStoneContainer
{
  // Diese Klasse ist von CPropStoneContainer abgeleitet
  // und in Constructor ist die Funktion ScanPropStones aufgerufen.
  // Die Funktion ScanPropStones sucht in allen eingefuegten FormatWorker
  // die Property Bausteine und diese sind in Container eingefuegt.
  // Dadurch in jede abgeleitete Klasse kann man diese Bausteine
  // ueber CPropStoneContainer Funktionen abgreifen.
public:
  CFormatWorkerWrapper* GetFormatWorkerWrapper(){return m_pFormatWorkerWrapper;};

protected:
  CPropProcessor(CWPP_Settings* pSettings, CFormatWorkerWrapper* pFormatWorkerWrapper);
  ~CPropProcessor();

  CFormatWorkerWrapper* m_pFormatWorkerWrapper;

private:
  bool ScanPropStones();

protected:
  CWPP_Settings* m_pSettings;
};






////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// stone
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CStoneInsertProcessor
{
  // Diese Klasse ist in CStoneProcessor erzeugt und dient dazu,
  // dass alle Child FormatWorker nach CStoneProcessor ueber diese Klasse
  // eingefuegt sind.
  // Ausserdem in dieser Klasse sind alle Text Felder und Area Felder
  // in Baustein eingefuegt.
public:
  CStoneInsertProcessor(CWPP_Settings* pSettings, CStoneProcessor* pProcessor)
    {m_pSettings = pSettings; m_pProcessor = pProcessor;};
  ~CStoneInsertProcessor(){};

  // Ueber diese Funktion sind die FormatWorker eingefuegt
  virtual bool InsertStone(CStoneProcessor* pProcessor);
  // In dieser Funktion ist Arbeit mit den Textfelder / Areafelder / XPub Konstanten beendet.
  // Das bedeutet, Felder sind nach Baustein eingefuegt und XPub Konstanten entfernt.
  // Da ist gerade die Stelle, wo man die Felder auch nach anderem Ziel einfuegen kann.
  virtual bool FinishWorkWithFields();
protected:
  // Klasse CStoneProcessor, die diese Klasse erzeugt hat
  CStoneProcessor*  m_pProcessor;
  CWPP_Settings* m_pSettings;
};

class XPUBADDONMANAGER_EXPORTIMPORT CStoneProcessor : public CPropProcessor
{
  // Diese Klasse ist fuer jedem FormatWorker (Typ Baustein) ueber CWorkPaperProcessor erzeugt.
public:
  CStoneProcessor(CWPP_Settings* pSettings, CTemplateProcessor* pTemplateProcessor, CWorkPaperProcessor* pWorkPaperProcessor, CFormatWorkerWrapper* pFormatWorkerWrapper);
  CStoneProcessor(CWPP_Settings* pSettings, CStoneProcessor* pStoneProcessor, CWorkPaperProcessor* pWorkPaperProcessor, CFormatWorkerWrapper* pFormatWorkerWrapper);
  virtual ~CStoneProcessor();

  // CStoneInsertProcessor ist Klasse, ueber welcher sind Bausteine
  // nach FormatWorker eingefuegt, der in dieser Klasse abgelegt ist (m_pFormatWorker in basis Klasse).
  // m_pInsertProcessor ist in OnStartProcessStone erzeugt.
  // In dem Moment sind alle PropBausteine schon vorhanden und man kann entscheiden,
  // welchem InsertProcessor instanziere ich.
  CStoneInsertProcessor* GetInsertProcessor(){return m_pInsertProcessor;};

  // In dieser Funktion sind alle FormatWorker verarbeitet,
  // die in Liste des CFormatWorker (m_pFormatWorker in basis Klasse) abgelegt sind.
  // Am Ende der Funktion ist FormatWorker nach Ziel eingefuegt.
  virtual bool ProcessStone();
protected:
  // Am Anfang der Funktion ProcessStone aufgerufen. Da muss man mindestens m_pInsertProcessor erzeugen.
  virtual bool OnStartProcessStone();
  // Am Ende der Funktion ProcessStone aufgerufen.
  virtual bool OnEndProcessStone();

protected:
  // Diesem Member muss man in OnStartProcessTemplate erzeugen.
  // Alle Objekte die nach diesem Baustein eingefuegt werden, fragen nach diesem Member.
  CStoneInsertProcessor* m_pInsertProcessor;
protected:
  CStoneProcessor*      m_pStoneProcessor;      // for CStoneProcessor created
  CTemplateProcessor*   m_pTemplateProcessor;   // for CTemplateProcessor created
  CWorkPaperProcessor*  m_pWorkPaperProcessor;

  xtstring  m_sWorkPaperContent;
};






////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// template
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CTemplateInsertProcessor
{
  // Diese Klasse ist in CTemplateProcessor erzeugt und dient dazu,
  // dass alle Child FormatWorker nach CTemplateProcessor ueber diese Klasse
  // eingefuegt sind.
  // Ausserdem in dieser Klasse sind alle Text Felder und Area Felder
  // in Baustein eingefuegt.
public:
  CTemplateInsertProcessor(CWPP_Settings* pSettings, CTemplateProcessor* pProcessor)
    {m_pSettings = pSettings; m_pProcessor = pProcessor;};
  ~CTemplateInsertProcessor(){};

  // Ueber diese Funktionen sind die FormatWorker eingefuegt.
  virtual bool InsertStone(CStoneProcessor* pProcessor);
  virtual bool InsertTemplate(CTemplateProcessor* pProcessor);
  // In dieser Funktion ist Arbeit mit den Textfelder / Areafelder / XPub Konstanten beendet.
  // Das bedeutet, Felder sind nach Vorlage eingefuegt und XPub Konstanten entfernt.
  // Da ist gerade die Stelle, wo man die Felder auch nach anderem Ziel einfuegen kann.
  virtual bool FinishWorkWithFields();
protected:
  // Klasse CTemplateProcessor, die diese Klasse erzeugt hat
  CTemplateProcessor* m_pProcessor;
  CWPP_Settings* m_pSettings;
};

class XPUBADDONMANAGER_EXPORTIMPORT CTemplateProcessor : public CPropProcessor
{
  // Diese Klasse ist fuer jedem FormatWorker (Typ Vorlage) ueber CWorkPaperProcessor erzeugt.
public:
  CTemplateProcessor(CWPP_Settings* pSettings, CTemplateProcessor* pTemplateProcessor, CWorkPaperProcessor* pWorkPaperProcessor, CFormatWorkerWrapper* pFormatWorkerWrapper);
  CTemplateProcessor(CWPP_Settings* pSettings, CWorkPaperProcessor* pWorkPaperProcessor, CFormatWorkerWrapper* pFormatWorkerWrapper);
  virtual ~CTemplateProcessor();

  // CTemplateInsertProcessor ist Klasse, ueber welcher sind Bausteine/Vorlagen
  // nach FormatWorker eingefuegt, der in dieser Klasse abgelegt ist (m_pFormatWorker in basis Klasse).
  // m_pInsertProcessor ist in OnStartProcessStone erzeugt.
  // In dem Moment sind alle PropBausteine schon vorhanden und man kann entscheiden,
  // welchem InsertProcessor instanziere ich.
  CTemplateInsertProcessor* GetInsertProcessor(){return m_pInsertProcessor;};

  // In dieser Funktion sind alle FormatWorker verarbeitet,
  // die in Liste des CFormatWorker (m_pFormatWorker in basis Klasse) abgelegt sind.
  // Am Ende der Funktion ist FormatWorker nach Ziel eingefuegt.
  virtual bool ProcessTemplate();
protected:
  // Am Anfang der Funktion ProcessStone aufgerufen. Da muss man mindestens m_pInsertProcessor erzeugen.
  virtual bool OnStartProcessTemplate();
  // Am Ende der Funktion ProcessTemplate aufgerufen.
  virtual bool OnEndProcessTemplate();

protected:
  // Diesem Member muss man in OnStartProcessTemplate erzeugen.
  // Alle Objekte die nach diese Vorlage eingefuegt werden, fragen nach diesem Member.
  CTemplateInsertProcessor* m_pInsertProcessor;
protected:
  CTemplateProcessor*   m_pTemplateProcessor;   // for CTemplateProcessor created
  CWorkPaperProcessor*  m_pWorkPaperProcessor;

  xtstring  m_sWorkPaperContent;
};






////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// main processor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CWorkPaperProcessor
{
  friend class CStoneProcessor;
  friend class CTemplateProcessor;
public:
  CWorkPaperProcessor();
  virtual ~CWorkPaperProcessor();

  void SetGeneratedTemplate(CFormatWorkerWrapper_Template* pTemplate){m_pTemplate = pTemplate;};
  void SetGeneratedStone(CFormatWorkerWrapper_Stone* pStone){m_pStone = pStone;};
  void SetAddOnManager(CXPubAddOnManager* pAddOnManager){m_pAddOnManager = pAddOnManager;};

  void SetUseElementID(bool bUseElementID_Field, bool bUseElementID_Stone, bool bUseElementID_Template);

// Ergebnis mit ConvertWorker verarbeiten
  bool ProcessGeneratedWorkPaper(xtstring& sResult);

  CWPP_ErrorClass* GetErrors(){return &m_cSettings;};

protected:
  virtual CTemplateProcessor* CreateTemplateProcessor(CFormatWorkerWrapper* pFormatWorker);
  virtual CTemplateProcessor* CreateTemplateProcessor(CTemplateProcessor* pTemplateProcessor, CFormatWorkerWrapper* pFormatWorker);
  virtual CStoneProcessor* CreateStoneProcessor(CTemplateProcessor* pTemplateProcessor, CFormatWorkerWrapper* pFormatWorker);
  virtual CStoneProcessor* CreateStoneProcessor(CStoneProcessor* pStoneProcessor, CFormatWorkerWrapper* pFormatWorker);

protected:
  CWPP_Settings                   m_cSettings;
  CFormatWorkerWrapper_Template*  m_pTemplate;
  CFormatWorkerWrapper_Stone*     m_pStone;
  CXPubAddOnManager*              m_pAddOnManager;
};


#endif // _WORKPAPERPROCESSOR_H_
