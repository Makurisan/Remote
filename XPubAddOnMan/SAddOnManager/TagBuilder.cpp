#include "TagBuilder.h"


CTagBuilder::CTagBuilder(CConvertWorkerProgress* pConvertProgress)
{
  m_pTo = 0;
  m_pFrom = 0;
  m_pConvertProgress = pConvertProgress;
  m_pAddOnManager = 0;
  m_pKeywords = 0;
  m_bRestrictFeatures = false;
}

CTagBuilder::~CTagBuilder(void)
{
}

void CTagBuilder::SetProgress(CConvertWorkerProgress* pConvertProgress)
{
  m_pConvertProgress = pConvertProgress;
}

void CTagBuilder::Detach()
{
  m_pAddOnManager = 0;
}

bool CTagBuilder::Attach(CXPubAddOnManager* pAddOnManager/*=0*/)
{
  if (!pAddOnManager)
    return false;
  m_pAddOnManager = pAddOnManager;
  return true;
} 

bool CTagBuilder::Set(const xtstring& sInput)
{
  if (m_pFrom == NULL || m_pTo == NULL)
    return false;

  m_pFrom->SetRestrictFeatures(m_bRestrictFeatures);
  m_pFrom->ConvertAndAdd(sInput, _XT(""), tCopyType_ActualPosition);
  return true;
}

bool CTagBuilder::Open(BuildFormats eFormat)
{
  if (!m_pAddOnManager)
    return false;

  if (eFormat == RTF)
    m_eDataFormat = tDataFormatST_Rtf;
  else if (eFormat == PDF)
    m_eDataFormat = tDataFormatST_Pdf;
  else if (eFormat == INDESIGN)
    m_eDataFormat = tDataFormatST_InDesign;
  else if (eFormat == FDF)
    m_eDataFormat = tDataFormatST_Fdf;

 // create convert worker from
  m_pFrom = m_pAddOnManager->CreateConvertWorkerFrom(tDataFormatST_Tag);
  if (!m_pFrom)
  {
    //AfxMessageBox(_T("Convert AddOn 'From Tag' not create!"));
    return false;
  }
  m_pFrom->SetKeywords(m_pKeywords);
  m_pFrom->SetConvertWorkerProgress(m_pConvertProgress);
  // create convert worker to
  if (!m_pAddOnManager->ConvertWorkerToAvailable(m_eDataFormat))
  {
    //AfxMessageBox(_T("Convert AddOn 'To Rtf' not available!"));
    return false;
  }
  m_pTo = m_pAddOnManager->CreateConvertWorkerTo(m_eDataFormat, ::GetDesktopWindow());
  if (!m_pTo)
  {
    //AfxMessageBox(_T("Convert AddOn 'To Rtf' not create!"));
    return false;
  }

  if(m_sRootTemplatePath.size())
    m_pFrom->SetRootTemplatePath(m_sRootTemplatePath);

  if(m_sRootPicturePath.size())
    m_pFrom->SetRootPicturePath(m_sRootPicturePath);

  // rtf
  m_pFrom->AttachConvertTo(m_pTo);

  return true;
}

bool CTagBuilder::Close()
{
  if (!m_pAddOnManager)
    return false;

  if (m_pFrom)
  {
    m_pFrom->DetachConvertTo();
    m_pAddOnManager->ReleaseConvertWorkerFrom(m_pFrom);
  }

  if (m_pTo)
  {
    m_pAddOnManager->ReleaseConvertWorkerTo(m_pTo);
  }

  m_pTo = 0;
  m_pFrom = 0;

  return true;
}

bool CTagBuilder::Get(xtstring& sFormatData)
{
  char *pFormatData=NULL;
  int lFormatSize=0;

  if (m_eDataFormat == tDataFormatST_Rtf)
  {
    sFormatData = m_pTo->GetDataStream(m_eDataFormat);
    return true;
  }

  if (Get(pFormatData, lFormatSize))
  {
    sFormatData = pFormatData;
    delete []pFormatData;
    return true;
  }
  return false;
}	

bool CTagBuilder::Get(char*& pFormatData, int& lFormatSize)
{
  if (m_pFrom == 0 || m_pTo == 0)
    return false;

  if (m_pTo->IsDataFormatAvailable(m_eDataFormat))
  {
    lFormatSize = 0;
    m_pTo->GetDataStream(m_eDataFormat, pFormatData, lFormatSize);
    if (lFormatSize)
    {
      pFormatData = new char[lFormatSize + 1];
      pFormatData[lFormatSize] = 0;
      lFormatSize += 1;
      if (!m_pTo->GetDataStream(m_eDataFormat, pFormatData, lFormatSize))
      {
        delete []pFormatData;
        pFormatData = NULL;
        lFormatSize = 0;
        return false;
      }
    }
    else
      return false;
  }
  return true;
}

bool CTagBuilder::Save(const xtstring& sFileName)
{
  if (m_pFrom == 0 || m_pTo == 0)
    return false;

  if (m_pTo->SaveDataStream(sFileName))
    return true;
 
  return false;

}

bool CTagBuilder::Copy()
{
#if defined(WIN32)
  xtstring sData(_XT(""));
  if (Get(sData))
  {
    if (OpenClipboard(::GetDesktopWindow()))
    {
      HGLOBAL clipbuffer;
      char * buffer;
      EmptyClipboard();
      clipbuffer = GlobalAlloc(GMEM_DDESHARE, sData.length()+1);
      buffer = (char*)GlobalLock(clipbuffer);
      strcpy(buffer, LPCSTR(sData.c_str()));
      GlobalUnlock(clipbuffer);
      SetClipboardData(RegisterClipboardFormat("Rich Text Format"),clipbuffer);
      CloseClipboard();
      return true;
    }
  }
#endif // WIN32
  return false;
}

bool CTagBuilder::SetPassword(const xtstring& sOwner,
                              const xtstring& sUser,
                              int lPermissions)
{
  if (!m_pTo)
    return false;

  if (m_pTo->GetFormaterType() == PDF_Formater || m_pTo->GetFormaterType() == FDF_Formater)
    return m_pTo->SetPassword(sOwner, sUser, lPermissions);
  else
    return false;
}

bool CTagBuilder::GetDocumentStrings(CxtstringVector& vStringArray)
{
  if (!m_pTo)
    return false;

  if (m_pTo->GetFormaterType() == PDF_Formater)
  {
    m_pTo->GetDocumentStrings(vStringArray);
  }  
  return false;

}

bool CTagBuilder::SetDocumentProperty(const TDocumentProperty nTypeOfProperty, const xtstring& sValue)
{
  if (m_pTo && !sValue.empty())
    return m_pTo->SetDocumentInfo(nTypeOfProperty, sValue);
  return true;
}

