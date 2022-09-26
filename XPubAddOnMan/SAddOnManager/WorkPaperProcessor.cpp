#include "WorkPaperProcessor.h"








////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// base
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CPropProcessor::CPropProcessor(CWPP_Settings* pSettings,
                               CFormatWorkerWrapper* pFormatWorkerWrapper)
{
  m_pSettings = pSettings;
  m_pFormatWorkerWrapper = pFormatWorkerWrapper;
  ScanPropStones();
}
CPropProcessor::~CPropProcessor()
{
}
bool CPropProcessor::ScanPropStones()
{
  if (!m_pFormatWorkerWrapper)
  {
    m_pSettings->AddError(_XT("m_pFormatWorkerWrapper ist NULL in CPropProcessor::ScanPropStones!"));
    return false;
  }

  for (size_t nI = 0; nI < m_pFormatWorkerWrapper->CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper* pChild = m_pFormatWorkerWrapper->GetStoneWrapper(nI);
    if (!pChild)
      continue;
    if (pChild->GetFormatWorkerType() != tDataFormatST_Property)
      continue;
    CFormatWorker* pFormatWorker = pChild->GetFormatWorker();
    if (!pFormatWorker)
      continue;
    xtstring sPropStone = pFormatWorker->GetWorkPaperContent();
    CPropStoneInd cProp((CSyncWPNotifiersGroup*)0);
    if (cProp.ReadXMLContent(sPropStone))
      AddPropertyGroup(&cProp.GetProperties());
  }
  return true;
}







////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// stone
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool CStoneInsertProcessor::InsertStone(CStoneProcessor* pProcessor)
{
  if (!pProcessor)
  {
    m_pSettings->AddError(_XT("pProcessor ist NULL in CStoneInsertProcessor::InsertStone!"));
    return false;
  }

  CFormatWorkerWrapper* pFWToInsert = pProcessor->GetFormatWorkerWrapper();
  if (!pFWToInsert)
  {
    m_pSettings->AddError(_XT("pFWToInsert ist NULL in CStoneInsertProcessor::InsertStone!"));
    return false;
  }
  CFormatWorkerWrapper* pMyFW = m_pProcessor->GetFormatWorkerWrapper();
  if (!pMyFW)
  {
    m_pSettings->AddError(_XT("pMyFW ist NULL in CStoneInsertProcessor::InsertStone!"));
    return false;
  }

  if (!pFWToInsert->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in pFWToInsert ist NULL in CStoneInsertProcessor::InsertStone!"));
    return false;
  }
  if (!pMyFW->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in pMyFW ist NULL in CStoneInsertProcessor::InsertStone!"));
    return false;
  }

  pFWToInsert->GetFormatWorker()->SetWorkPaperProcessingMode(tProcessingMode_Immediately);

  int nCountOfNotRealised;
  int nCountOfRealised;
  int nIncrementStateInsertCount;
  try
  {
    pMyFW->GetFormatWorker()->InsertWorkPaper(pFWToInsert->GetFormatWorker(), nCountOfNotRealised, nCountOfRealised, nIncrementStateInsertCount);
  }
  catch (...)
  {
    m_pSettings->AddError(_XT("Exception in CStoneInsertProcessor::InsertStone bei InsertWorkPaper!"));
    return false;
  }

  return true;
}
bool CStoneInsertProcessor::FinishWorkWithFields()
{
  if (!m_pProcessor->GetFormatWorkerWrapper())
  {
    m_pSettings->AddError(_XT("FormatWorkerWrapper ist NULL in CStoneInsertProcessor::FinishWorkWithFields!"));
    return false;
  }

  CFormatWorkerWrapper* pMyFW = m_pProcessor->GetFormatWorkerWrapper();
  if (!pMyFW)
  {
    m_pSettings->AddError(_XT("pMyFW ist NULL in CStoneInsertProcessor::FinishWorkWithFields!"));
    return false;
  }
  if (!pMyFW->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in pMyFW ist NULL in CStoneInsertProcessor::FinishWorkWithFields!"));
    return false;
  }

  bool bRet = true;

  /////////////////////////////////
  // Original Key Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->GetFormatWorker()->CountOfKeys(); nI++)
  {
    CXPubVariant cResult;
    CMPModelKeyInd_ForFrm* pKey = pMyFW->GetFormatWorker()->GetKey(nI);
    if (!pKey)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pKey, cResult);
    // Informationen holen
    xtstring sFieldName = pKey->CMPModelKeyInd::GetName();
    xtstring sKey = pKey->GetKeyResult();
    xtstring sTagProperty = pKey->GetTagPropertyResult();
    if (((bool)cResult) != true)
      // nicht darstellen
      sKey.clear();
    try
    {
      // in Baustein ersetzen
      bRet &= pMyFW->GetFormatWorker()->ReplaceTextField(sFieldName, sKey);
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceTextField (KEY original) in CStoneInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }
  ////////////////////////////////
  // Wrapper Key Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->CountOfFieldWrappers(); nI++)
  {
    CFormatWorkerWrapper_Field* pField = pMyFW->GetFieldWrapper(nI);
    if (!pField)
      continue;
    if (!pField->IsKey())
      continue;
    CXPubVariant cResult;
    CMPModelKeyInd_ForFrm* pKey = pField->GetKey();
    if (!pKey)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pKey, cResult);
    // Informationen holen
    xtstring sFieldName = pKey->CMPModelKeyInd::GetName();
    xtstring sKey = pKey->GetKeyResult();
    if (pField->ElementIDDefined() && m_pSettings->m_bUseElementID_Field)
      sKey = pField->GetElementIDForReplace_Front();
    xtstring sTagProperty = pKey->GetTagPropertyResult();
    if (((bool)cResult) != true)
      // nicht darstellen
      sKey.clear();
    try
    {
      // in Baustein ersetzen
      bRet &= pMyFW->GetFormatWorker()->ReplaceTextField(sFieldName, sKey);
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceTextField (KEY wrapper) in CStoneInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }


  //////////////////////////////////
  // Original Text Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->GetFormatWorker()->CountOfTexts(); nI++)
  {
    CXPubVariant cResult;
    CMPModelTextInd_ForFrm* pText = pMyFW->GetFormatWorker()->GetText(nI);
    if (!pText)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pText, cResult);
    // Informationen holen
    xtstring sFieldName = pText->CMPModelTextInd::GetName();
    xtstring sContent = pText->GetContentResult();
    xtstring sTagProperty = pText->GetTagPropertyResult();
    if (((bool)cResult) != true)
      // nicht darstellen
      sContent.clear();
    try
    {
      // in Baustein ersetzen
      bRet &= pMyFW->GetFormatWorker()->ReplaceTextField(sFieldName, sContent);
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceTextField (TEXT original) in CStoneInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }
  //////////////////////////////////
  // Wrapper Text Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->CountOfFieldWrappers(); nI++)
  {
    CFormatWorkerWrapper_Field* pField = pMyFW->GetFieldWrapper(nI);
    if (!pField)
      continue;
    if (!pField->IsText())
      continue;
    CXPubVariant cResult;
    CMPModelTextInd_ForFrm* pText = pField->GetText();
    if (!pText)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pText, cResult);
    // Informationen holen
    xtstring sFieldName = pText->CMPModelTextInd::GetName();
    xtstring sContent = pText->GetContentResult();
    if (pField->ElementIDDefined() && m_pSettings->m_bUseElementID_Field)
      sContent = pField->GetElementIDForReplace_Front();
    xtstring sTagProperty = pText->GetTagPropertyResult();
    if (((bool)cResult) != true)
      // nicht darstellen
      sContent.clear();
    try
    {
      // in Baustein ersetzen
      bRet &= pMyFW->GetFormatWorker()->ReplaceTextField(sFieldName, sContent);
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceTextField (TEXT wrapper) in CStoneInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }


  //////////////////////////////////
  // Original Area Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->GetFormatWorker()->CountOfAreas(); nI++)
  {
    CXPubVariant cResult;
    CMPModelAreaInd_ForFrm* pArea = pMyFW->GetFormatWorker()->GetArea(nI);
    if (!pArea)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pArea, cResult);
    // Informationen holen
    xtstring sFieldName = pArea->CMPModelAreaInd::GetName();
    try
    {
      // in Baustein loeschen
      bRet &= pMyFW->GetFormatWorker()->ReplaceAreaField(sFieldName, !((bool)cResult));
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceAreaField (AREA original) in CStoneInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }
  //////////////////////////////////
  // Wrapper Area Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->CountOfFieldWrappers(); nI++)
  {
    CFormatWorkerWrapper_Field* pField = pMyFW->GetFieldWrapper(nI);
    if (!pField)
      continue;
    if (!pField->IsArea())
      continue;
    CXPubVariant cResult;
    CMPModelAreaInd_ForFrm* pArea = pField->GetArea();
    if (!pArea)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pArea, cResult);
    // Informationen holen
    xtstring sFieldName = pArea->CMPModelAreaInd::GetName();
    try
    {
      // in Baustein loeschen
      bRet &= pMyFW->GetFormatWorker()->ReplaceAreaField(sFieldName, !((bool)cResult));
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceAreaField (AREA wrapper) in CStoneInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }


  // XPub Konstanten entfernen
  CxtstringVector arrConstants;
  pMyFW->GetFormatWorker()->GetAllConstants(arrConstants);
  try
  {
    pMyFW->GetFormatWorker()->CleanUpWorkPaper(arrConstants);
  }
  catch (...)
  {
    m_pSettings->AddError(_XT("Exception bei CleanUpWorkPaper in CStoneInsertProcessor::FinishWorkWithFields!"));
    return false;
  }
  return bRet;
}


CStoneProcessor::CStoneProcessor(CWPP_Settings* pSettings,
                                 CTemplateProcessor* pTemplateProcessor,
                                 CWorkPaperProcessor* pWorkPaperProcessor,
                                 CFormatWorkerWrapper* pFormatWorkerWrapper)
                :CPropProcessor(pSettings,
                                pFormatWorkerWrapper)
{
  m_pInsertProcessor = 0;

  m_pStoneProcessor = 0;
  m_pTemplateProcessor = pTemplateProcessor;
  m_pWorkPaperProcessor = pWorkPaperProcessor;

  // save WorkPaper Content
  if (m_pFormatWorkerWrapper && m_pFormatWorkerWrapper->GetFormatWorker())
  {
    m_sWorkPaperContent = m_pFormatWorkerWrapper->GetFormatWorker()->GetWorkPaperContent();
  }
}
CStoneProcessor::CStoneProcessor(CWPP_Settings* pSettings,
                                 CStoneProcessor* pStoneProcessor,
                                 CWorkPaperProcessor* pWorkPaperProcessor,
                                 CFormatWorkerWrapper* pFormatWorkerWrapper)
                :CPropProcessor(pSettings,
                                pFormatWorkerWrapper)
{
  m_pInsertProcessor = 0;

  m_pStoneProcessor = pStoneProcessor;
  m_pTemplateProcessor = 0;
  m_pWorkPaperProcessor = pWorkPaperProcessor;

  // save WorkPaper Content
  if (m_pFormatWorkerWrapper && m_pFormatWorkerWrapper->GetFormatWorker())
  {
    m_sWorkPaperContent = m_pFormatWorkerWrapper->GetFormatWorker()->GetWorkPaperContent();
  }
}
CStoneProcessor::~CStoneProcessor()
{
  if (m_pInsertProcessor)
    delete m_pInsertProcessor;

  // restore WorkPaper Content
  if (m_pFormatWorkerWrapper && m_pFormatWorkerWrapper->GetFormatWorker())
  {
    m_pFormatWorkerWrapper->GetFormatWorker()->SetWorkPaperContent(m_sWorkPaperContent);
  }
}
bool CStoneProcessor::ProcessStone()
{
//  if (!m_pTemplateProcessor && !m_pStoneProcessor)
//    return false;
  if (!m_pFormatWorkerWrapper)
  {
    m_pSettings->AddError(_XT("m_pFormatWorkerWrapper ist NULL in CStoneProcessor::ProcessStone!"));
    return false;
  }
  if (!m_pFormatWorkerWrapper->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in m_pFormatWorkerWrapper ist NULL in CStoneProcessor::ProcessStone!"));
    return false;
  }


  if (!OnStartProcessStone())
  {
    m_pSettings->AddError(_XT("OnStartProcessStone failed in CStoneProcessor::ProcessStone!"));
    return false;
  }

  // wir verarbeiten zuerst eingefuegte FormatWorker - stones
  for (size_t nI = 0; nI < m_pFormatWorkerWrapper->CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper* pChild = m_pFormatWorkerWrapper->GetStoneWrapper(nI);
    if (!pChild)
      continue;
//    if (pChild->GetWorkPaperType() == tWorkPaperType_Stone)
    {
      if (pChild->GetFormatWorkerType() == tDataFormatST_Property)
        continue;

      CStoneProcessor* pProcessor = m_pWorkPaperProcessor->CreateStoneProcessor(this, pChild);
      if (!pProcessor)
      {
        m_pSettings->AddError(_XT("CStoneProcessor nicht erzeugt in CStoneProcessor::ProcessStone!"));
        return false;
      }
      if (!pProcessor->ProcessStone())
      {
        m_pSettings->AddError(_XT("ProcessStone failed in CStoneProcessor::ProcessStone!"));
        delete pProcessor;
        return false;
      }
      delete pProcessor;
    }
  }

  // jetzt beenden wir diesem Baustein
  if (GetInsertProcessor())
    GetInsertProcessor()->FinishWorkWithFields();

  // jetzt setzen wir die Grenzen
  if (m_pFormatWorkerWrapper->ElementIDDefined() && m_pSettings->m_bUseElementID_Stone)
  {
    try
    {
      m_pFormatWorkerWrapper->GetFormatWorker()->InsertText_Front(m_pFormatWorkerWrapper->GetElementIDForReplace_Front());
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception in CStoneProcessor::ProcessStone bei InsertText_Front!"));
      return false;
    }
    try
    {
      m_pFormatWorkerWrapper->GetFormatWorker()->InsertText_Tail(m_pFormatWorkerWrapper->GetElementIDForReplace_Tail());
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception in CStoneProcessor::ProcessStone bei InsertText_Tail!"));
      return false;
    }
  }

  // jetzt einfuegen wir diesem Baustein
  if (m_pTemplateProcessor && m_pTemplateProcessor->GetInsertProcessor())
    m_pTemplateProcessor->GetInsertProcessor()->InsertStone(this);
  if (m_pStoneProcessor && m_pStoneProcessor->GetInsertProcessor())
    m_pStoneProcessor->GetInsertProcessor()->InsertStone(this);

  if (!OnEndProcessStone())
  {
    m_pSettings->AddError(_XT("OnEndProcessStone failed in CStoneProcessor::ProcessStone!"));
    return false;
  }

  return true;
}
bool CStoneProcessor::OnStartProcessStone()
{
  CPropStoneIndesignNewDocProps* pProp = 0;
  pProp = GetNextGroup(pProp);
  while (pProp)
  {
    CXPubVariant cVar;
    pProp->GetColumnsGutter(cVar);
    pProp->GetColumnsNumber(cVar);
    pProp = GetNextGroup(pProp);
  }

  m_pInsertProcessor = new CStoneInsertProcessor(m_pSettings, this);
  return true;
}
bool CStoneProcessor::OnEndProcessStone()
{
  return true;
}








////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// template
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool CTemplateInsertProcessor::InsertStone(CStoneProcessor* pProcessor)
{
  if (!pProcessor)
  {
    m_pSettings->AddError(_XT("pProcessor ist NULL in CTemplateInsertProcessor::InsertStone!"));
    return false;
  }

  CFormatWorkerWrapper* pFWToInsert = pProcessor->GetFormatWorkerWrapper();
  if (!pFWToInsert)
  {
    m_pSettings->AddError(_XT("pFWToInsert ist NULL in CTemplateInsertProcessor::InsertStone!"));
    return false;
  }
  CFormatWorkerWrapper* pMyFW = m_pProcessor->GetFormatWorkerWrapper();
  if (!pMyFW)
  {
    m_pSettings->AddError(_XT("pMyFW ist NULL in CTemplateInsertProcessor::InsertStone!"));
    return false;
  }

  if (!pFWToInsert->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in pFWToInsert ist NULL in CTemplateInsertProcessor::InsertStone!"));
    return false;
  }
  if (!pMyFW->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in pMyFW ist NULL in CTemplateInsertProcessor::InsertStone!"));
    return false;
  }

  pFWToInsert->GetFormatWorker()->SetWorkPaperProcessingMode(tProcessingMode_Immediately);

  int nCountOfNotRealised;
  int nCountOfRealised;
  int nIncrementStateInsertCount;
  try
  {
    pMyFW->GetFormatWorker()->InsertWorkPaper(pFWToInsert->GetFormatWorker(), nCountOfNotRealised, nCountOfRealised, nIncrementStateInsertCount);
  }
  catch (...)
  {
    m_pSettings->AddError(_XT("Exception in CTemplateInsertProcessor::InsertStone bei InsertWorkPaper!"));
    return false;
  }

  return true;
}
bool CTemplateInsertProcessor::InsertTemplate(CTemplateProcessor* pProcessor)
{
  if (!pProcessor)
  {
    m_pSettings->AddError(_XT("pProcessor ist NULL in CTemplateInsertProcessor::InsertTemplate!"));
    return false;
  }

  CFormatWorkerWrapper* pFWToInsert = pProcessor->GetFormatWorkerWrapper();
  if (!pFWToInsert)
  {
    m_pSettings->AddError(_XT("pFWToInsert ist NULL in CTemplateInsertProcessor::InsertTemplate!"));
    return false;
  }
  CFormatWorkerWrapper* pMyFW = m_pProcessor->GetFormatWorkerWrapper();
  if (!pMyFW)
  {
    m_pSettings->AddError(_XT("pMyFW ist NULL in CTemplateInsertProcessor::InsertTemplate!"));
    return false;
  }

  if (!pFWToInsert->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in pFWToInsert ist NULL in CTemplateInsertProcessor::InsertTemplate!"));
    return false;
  }
  if (!pMyFW->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in pMyFW ist NULL in CTemplateInsertProcessor::InsertTemplate!"));
    return false;
  }

  pFWToInsert->GetFormatWorker()->SetWorkPaperProcessingMode(tProcessingMode_Immediately);

  int nCountOfNotRealised;
  int nCountOfRealised;
  int nIncrementStateInsertCount;
  try
  {
    pMyFW->GetFormatWorker()->InsertWorkPaper(pFWToInsert->GetFormatWorker(), nCountOfNotRealised, nCountOfRealised, nIncrementStateInsertCount);
  }
  catch (...)
  {
    m_pSettings->AddError(_XT("Exception in CTemplateInsertProcessor::InsertTemplate bei InsertWorkPaper!"));
    return false;
  }

  return true;
}
bool CTemplateInsertProcessor::FinishWorkWithFields()
{
  if (!m_pProcessor->GetFormatWorkerWrapper())
  {
    m_pSettings->AddError(_XT("FormatWorkerWrapper ist NULL in CTemplateInsertProcessor::FinishWorkWithFields!"));
    return false;
  }

  CFormatWorkerWrapper* pMyFW = m_pProcessor->GetFormatWorkerWrapper();
  if (!pMyFW)
  {
    m_pSettings->AddError(_XT("pMyFW ist NULL in CTemplateInsertProcessor::FinishWorkWithFields!"));
    return false;
  }
  if (!pMyFW->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in pMyFW ist NULL in CTemplateInsertProcessor::FinishWorkWithFields!"));
    return false;
  }

  bool bRet = true;

  /////////////////////////////////
  // Original Key Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->GetFormatWorker()->CountOfKeys(); nI++)
  {
    CXPubVariant cResult;
    CMPModelKeyInd_ForFrm* pKey = pMyFW->GetFormatWorker()->GetKey(nI);
    if (!pKey)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pKey, cResult);
    // Informationen holen
    xtstring sFieldName = pKey->CMPModelKeyInd::GetName();
    xtstring sKey = pKey->GetKeyResult();
    xtstring sTagProperty = pKey->GetTagPropertyResult();
    if (((bool)cResult) != true)
      // nicht darstellen
      sKey.clear();
    try
    {
      // in Baustein ersetzen
      bRet &= pMyFW->GetFormatWorker()->ReplaceTextField(sFieldName, sKey);
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceTextField (KEY original) in CTemplateInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }
  ////////////////////////////////
  // Wrapper Key Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->CountOfFieldWrappers(); nI++)
  {
    CFormatWorkerWrapper_Field* pField = pMyFW->GetFieldWrapper(nI);
    if (!pField)
      continue;
    if (!pField->IsKey())
      continue;
    CXPubVariant cResult;
    CMPModelKeyInd_ForFrm* pKey = pField->GetKey();
    if (!pKey)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pKey, cResult);
    // Informationen holen
    xtstring sFieldName = pKey->CMPModelKeyInd::GetName();
    xtstring sKey = pKey->GetKeyResult();
    if (pField->ElementIDDefined() && m_pSettings->m_bUseElementID_Field)
      sKey = pField->GetElementIDForReplace_Front();
    xtstring sTagProperty = pKey->GetTagPropertyResult();
    if (((bool)cResult) != true)
      // nicht darstellen
      sKey.clear();
    try
    {
      // in Baustein ersetzen
      bRet &= pMyFW->GetFormatWorker()->ReplaceTextField(sFieldName, sKey);
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceTextField (KEY wrapper) in CTemplateInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }


  //////////////////////////////////
  // Original Text Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->GetFormatWorker()->CountOfTexts(); nI++)
  {
    CXPubVariant cResult;
    CMPModelTextInd_ForFrm* pText = pMyFW->GetFormatWorker()->GetText(nI);
    if (!pText)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pText, cResult);
    // Informationen holen
    xtstring sFieldName = pText->CMPModelTextInd::GetName();
    xtstring sContent = pText->GetContentResult();
    xtstring sTagProperty = pText->GetTagPropertyResult();
    if (((bool)cResult) != true)
      // nicht darstellen
      sContent.clear();
    try
    {
      // in Baustein ersetzen
      bRet &= pMyFW->GetFormatWorker()->ReplaceTextField(sFieldName, sContent);
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceTextField (TEXT original) in CTemplateInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }
  //////////////////////////////////
  // Wrapper Text Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->CountOfFieldWrappers(); nI++)
  {
    CFormatWorkerWrapper_Field* pField = pMyFW->GetFieldWrapper(nI);
    if (!pField)
      continue;
    if (!pField->IsText())
      continue;
    CXPubVariant cResult;
    CMPModelTextInd_ForFrm* pText = pField->GetText();
    if (!pText)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pText, cResult);
    // Informationen holen
    xtstring sFieldName = pText->CMPModelTextInd::GetName();
    xtstring sContent = pText->GetContentResult();
    if (pField->ElementIDDefined() && m_pSettings->m_bUseElementID_Field)
      sContent = pField->GetElementIDForReplace_Front();
    xtstring sTagProperty = pText->GetTagPropertyResult();
    if (((bool)cResult) != true)
      // nicht darstellen
      sContent.clear();
    try
    {
      // in Baustein ersetzen
      bRet &= pMyFW->GetFormatWorker()->ReplaceTextField(sFieldName, sContent);
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceTextField (TEXT wrapper) in CTemplateInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }


  //////////////////////////////////
  // Original Area Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->GetFormatWorker()->CountOfAreas(); nI++)
  {
    CXPubVariant cResult;
    CMPModelAreaInd_ForFrm* pArea = pMyFW->GetFormatWorker()->GetArea(nI);
    if (!pArea)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pArea, cResult);
    // Informationen holen
    xtstring sFieldName = pArea->CMPModelAreaInd::GetName();
    try
    {
      // in Baustein loeschen
      bRet &= pMyFW->GetFormatWorker()->ReplaceAreaField(sFieldName, !((bool)cResult));
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceAreaField (AREA original) in CTemplateInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }
  //////////////////////////////////
  // Wrapper Area Felder bearbeiten
  for (size_t nI = 0; nI < pMyFW->CountOfFieldWrappers(); nI++)
  {
    CFormatWorkerWrapper_Field* pField = pMyFW->GetFieldWrapper(nI);
    if (!pField)
      continue;
    if (!pField->IsArea())
      continue;
    CXPubVariant cResult;
    CMPModelAreaInd_ForFrm* pArea = pField->GetArea();
    if (!pArea)
      continue;
    // Gueltigkeit auswerten
    bool bRet = pMyFW->GetFormatWorker()->ExecuteVisibility(pArea, cResult);
    // Informationen holen
    xtstring sFieldName = pArea->CMPModelAreaInd::GetName();
    try
    {
      // in Baustein loeschen
      bRet &= pMyFW->GetFormatWorker()->ReplaceAreaField(sFieldName, !((bool)cResult));
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception bei ReplaceAreaField (AREA wrapper) in CTemplateInsertProcessor::FinishWorkWithFields!"));
      bRet = false;
    }
  }


  // XPub Konstanten entfernen
  CxtstringVector arrConstants;
  pMyFW->GetFormatWorker()->GetAllConstants(arrConstants);
  try
  {
    pMyFW->GetFormatWorker()->CleanUpWorkPaper(arrConstants);
  }
  catch (...)
  {
    m_pSettings->AddError(_XT("Exception bei CleanUpWorkPaper in CTemplateInsertProcessor::FinishWorkWithFields!"));
    return false;
  }
  return bRet;
}


CTemplateProcessor::CTemplateProcessor(CWPP_Settings* pSettings,
                                       CTemplateProcessor* pTemplateProcessor,
                                       CWorkPaperProcessor* pWorkPaperProcessor,
                                       CFormatWorkerWrapper* pFormatWorkerWrapper)
                   :CPropProcessor(pSettings,
                                   pFormatWorkerWrapper)
{
  m_pInsertProcessor = 0;

  m_pTemplateProcessor = pTemplateProcessor;
  m_pWorkPaperProcessor = pWorkPaperProcessor;

  // save WorkPaper content
  if (m_pFormatWorkerWrapper && m_pFormatWorkerWrapper->GetFormatWorker())
  {
    m_sWorkPaperContent = m_pFormatWorkerWrapper->GetFormatWorker()->GetWorkPaperContent();
  }
}
CTemplateProcessor::CTemplateProcessor(CWPP_Settings* pSettings,
                                       CWorkPaperProcessor* pWorkPaperProcessor,
                                       CFormatWorkerWrapper* pFormatWorkerWrapper)
                   :CPropProcessor(pSettings,
                                   pFormatWorkerWrapper)
{
  m_pInsertProcessor = 0;

  m_pTemplateProcessor = 0;
  m_pWorkPaperProcessor = pWorkPaperProcessor;

  // save WorkPaper content
  if (m_pFormatWorkerWrapper && m_pFormatWorkerWrapper->GetFormatWorker())
  {
    m_sWorkPaperContent = m_pFormatWorkerWrapper->GetFormatWorker()->GetWorkPaperContent();
  }
}
CTemplateProcessor::~CTemplateProcessor()
{
  if (m_pInsertProcessor)
    delete m_pInsertProcessor;

  // restore WorkPaper Content
  if (m_pFormatWorkerWrapper && m_pFormatWorkerWrapper->GetFormatWorker())
  {
    m_pFormatWorkerWrapper->GetFormatWorker()->SetWorkPaperContent(m_sWorkPaperContent);
  }
}
bool CTemplateProcessor::ProcessTemplate()
{
  if (!m_pFormatWorkerWrapper)
  {
    m_pSettings->AddError(_XT("m_pFormatWorkerWrapper ist NULL in CTemplateProcessor::ProcessTemplate!"));
    return false;
  }
  if (!m_pFormatWorkerWrapper->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in m_pFormatWorkerWrapper ist NULL in CTemplateProcessor::ProcessTemplate!"));
    return false;
  }


  if (!OnStartProcessTemplate())
  {
    m_pSettings->AddError(_XT("OnStartProcessTemplate failed in CTemplateProcessor::ProcessTemplate!"));
    return false;
  }

  // wir verarbeiten zuerst eingefuegte FormatWorker - stones
  for (size_t nI = 0; nI < m_pFormatWorkerWrapper->CountOfStoneWrappers(); nI++)
  {
    CFormatWorkerWrapper* pChild = m_pFormatWorkerWrapper->GetStoneWrapper(nI);
    if (!pChild)
      continue;
//    if (pChild->GetWorkPaperType() == tWorkPaperType_Stone)
    {
      if (pChild->GetFormatWorkerType() == tDataFormatST_Property)
        continue;

      CStoneProcessor* pProcessor = m_pWorkPaperProcessor->CreateStoneProcessor(this, pChild);
      if (!pProcessor)
      {
        m_pSettings->AddError(_XT("CStoneProcessor nicht erzeugt in CTemplateProcessor::ProcessTemplate!"));
        return false;
      }
      if (!pProcessor->ProcessStone())
      {
        m_pSettings->AddError(_XT("ProcessStone failed in CTemplateProcessor::ProcessTemplate!"));
        delete pProcessor;
        return false;
      }
      delete pProcessor;
    }
  }
  // wir verarbeiten zuerst eingefuegte FormatWorker - templates
  for (size_t nI = 0; nI < m_pFormatWorkerWrapper->CountOfTemplateWrappers(); nI++)
  {
    CFormatWorkerWrapper* pChild = m_pFormatWorkerWrapper->GetTemplateWrapper(nI);
    if (!pChild)
      continue;
//    if (pChild->GetWorkPaperType() == tWorkPaperType_Template)
    {
      CTemplateProcessor* pProcessor = m_pWorkPaperProcessor->CreateTemplateProcessor(this, pChild);
      if (!pProcessor)
      {
        m_pSettings->AddError(_XT("CTemplateProcessor nicht erzeugt in CTemplateProcessor::ProcessTemplate!"));
        return false;
      }
      if (!pProcessor->ProcessTemplate())
      {
        m_pSettings->AddError(_XT("ProcessTemplate failed in CTemplateProcessor::ProcessTemplate!"));
        delete pProcessor;
        return false;
      }
      delete pProcessor;
    }
  }

  // jetzt beenden wir diese Vorlage
  if (GetInsertProcessor())
    GetInsertProcessor()->FinishWorkWithFields();

  // jetzt setzen wir die Grenzen
  if (m_pFormatWorkerWrapper->ElementIDDefined() && m_pSettings->m_bUseElementID_Template)
  {
    try
    {
      m_pFormatWorkerWrapper->GetFormatWorker()->InsertText_Front(m_pFormatWorkerWrapper->GetElementIDForReplace_Front());
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception in CTemplateProcessor::ProcessTemplate bei InsertText_Front!"));
      return false;
    }
    try
    {
      m_pFormatWorkerWrapper->GetFormatWorker()->InsertText_Tail(m_pFormatWorkerWrapper->GetElementIDForReplace_Tail());
    }
    catch (...)
    {
      m_pSettings->AddError(_XT("Exception in CTemplateProcessor::ProcessTemplate bei InsertText_Tail!"));
      return false;
    }
  }

  // jetzt einfuegen wir diese Vorlage
  if (m_pTemplateProcessor && m_pTemplateProcessor->GetInsertProcessor())
    m_pTemplateProcessor->GetInsertProcessor()->InsertTemplate(this);

  if (!OnEndProcessTemplate())
  {
    m_pSettings->AddError(_XT("OnEndProcessTemplate failed in CTemplateProcessor::ProcessTemplate!"));
    return false;
  }

  return true;
}
bool CTemplateProcessor::OnStartProcessTemplate()
{
  CPropStoneIndesignNewDocProps* pProp = 0;
  pProp = GetNextGroup(pProp);
  while (pProp)
  {
    CXPubVariant cVar;
    pProp->GetColumnsGutter(cVar);
    pProp->GetColumnsNumber(cVar);
    pProp = GetNextGroup(pProp);
  }
  m_pInsertProcessor = new CTemplateInsertProcessor(m_pSettings, this);
  return true;
}
bool CTemplateProcessor::OnEndProcessTemplate()
{
  // In dieser Version machen wir nicht abspeichern.
  /*
  CFormatWorkerWrapper* pMyFW = GetFormatWorkerWrapper();
  if (!pMyFW)
  {
    m_pSettings->AddError(_XT("pMyFW ist NULL in CTemplateProcessor::OnEndProcessTemplate!"));
    return false;
  }
  if (!pMyFW->GetFormatWorker())
  {
    m_pSettings->AddError(_XT("FormatWorker in pMyFW ist NULL in CTemplateProcessor::OnEndProcessTemplate!"));
    return false;
  }
  if (!m_pTemplateProcessor)
  {
    // top vorlage
    CGeneratedFiles cGeneratedFiles;
    try
    {
      pMyFW->GetFormatWorker()->SaveWorkPaperToFile(cGeneratedFiles);
    }
    catch (...)
    {
      return false;
    }
  }
  */

  return true;
}











////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// main processor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
CWorkPaperProcessor::CWorkPaperProcessor()
{
  m_pAddOnManager = 0;
  m_pTemplate = 0;
  m_pStone = 0;
}

CWorkPaperProcessor::~CWorkPaperProcessor()
{
}

bool CWorkPaperProcessor::ProcessGeneratedWorkPaper(xtstring& sResult)
{
  bool bRet = false;
  if (m_pTemplate)
  {
    CTemplateProcessor* pTemplateProcessor = CreateTemplateProcessor(m_pTemplate);
    if (pTemplateProcessor)
    {
      if (pTemplateProcessor->ProcessTemplate())
      {
        sResult = m_pTemplate->GetFormatWorker()->GetWorkPaperContent();
        bRet =  true;
      }
      else
        m_cSettings.AddError(_XT("ProcessTemplate failed!"));
      delete pTemplateProcessor;
    }
    else
      m_cSettings.AddError(_XT("CreateTemplateProcessor failed!"));
  }
  else if (m_pStone)
  {
    CStoneProcessor* pStoneProcessor = CreateStoneProcessor((CStoneProcessor*)0, m_pStone);
    if (pStoneProcessor)
    {
      if (pStoneProcessor->ProcessStone())
      {
        sResult = m_pStone->GetFormatWorker()->GetWorkPaperContent();
        bRet = true;
      }
      else
        m_cSettings.AddError(_XT("ProcessStone failed!"));
      delete pStoneProcessor;
    }
    else
      m_cSettings.AddError(_XT("CreateStoneProcessor failed!"));
  }
  else
  {
    m_cSettings.AddError(_XT("Es wurde kein WorkPaperWrapper übergeben!"));
  }

  return bRet;
}

void CWorkPaperProcessor::SetUseElementID(bool bUseElementID_Field,
                                          bool bUseElementID_Stone,
                                          bool bUseElementID_Template)
{
  m_cSettings.m_bUseElementID_Field = bUseElementID_Field;
  m_cSettings.m_bUseElementID_Stone = bUseElementID_Stone;
  m_cSettings.m_bUseElementID_Template = bUseElementID_Template;
}

CTemplateProcessor* CWorkPaperProcessor::CreateTemplateProcessor(CFormatWorkerWrapper* pFormatWorkerWrapper)
{
  return new CTemplateProcessor(&m_cSettings, this, pFormatWorkerWrapper);
}

CTemplateProcessor* CWorkPaperProcessor::CreateTemplateProcessor(CTemplateProcessor* pTemplateProcessor,
                                                                 CFormatWorkerWrapper* pFormatWorkerWrapper)
{
  return new CTemplateProcessor(&m_cSettings, pTemplateProcessor, this, pFormatWorkerWrapper);
}

CStoneProcessor* CWorkPaperProcessor::CreateStoneProcessor(CTemplateProcessor* pTemplateProcessor,
                                                           CFormatWorkerWrapper* pFormatWorkerWrapper)
{
  return new CStoneProcessor(&m_cSettings, pTemplateProcessor, this, pFormatWorkerWrapper);
}

CStoneProcessor* CWorkPaperProcessor::CreateStoneProcessor(CStoneProcessor* pStoneProcessor,
                                                           CFormatWorkerWrapper* pFormatWorkerWrapper)
{
  return new CStoneProcessor(&m_cSettings, pStoneProcessor, this, pFormatWorkerWrapper);
}
