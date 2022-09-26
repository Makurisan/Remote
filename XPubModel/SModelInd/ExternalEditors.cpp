#include <assert.h>
#include "../SSTLdef/STLdef.h"
#include "../Sxml/XPubMarkUp.h"
#include "../Sxml/XMLTagNames.h"
#include "ExternalEditors.h"



#define DEFAULT_DB_PORT_MYSQL       3306










CExternalEditors::CExternalEditors()
{
}
CExternalEditors::~CExternalEditors()
{
  ResetAll();
}

void CExternalEditors::ResetAll()
{
  SetEditorForPlain(_XT(""));
  SetEditorForHTML(_XT(""));
  SetEditorForRTF(_XT(""));
  SetEditorForTAG(_XT(""));
  SetEditorForDB(_XT(""));
  SetEditorForProp(_XT(""));
  SetEditorForXML(_XT(""));
}

xtstring CExternalEditors::GetExternalEditorForDataFormat(TDataFormatForStoneAndTemplate nDataFormat)
{
  xtstring sExternalEditor;
  switch (nDataFormat)
  {
  case tDataFormatST_Plain:
    sExternalEditor = GetEditorForPlain();
    break;
  case tDataFormatST_Html:
    sExternalEditor = GetEditorForHTML();
    break;
  case tDataFormatST_Rtf:
    sExternalEditor = GetEditorForRTF();
    break;
  case tDataFormatST_Tag:
    sExternalEditor = GetEditorForTAG();
    break;
  case tDataFormatST_Db:
    sExternalEditor = GetEditorForDB();
    break;
  case tDataFormatST_Property:
  case tDataFormatST_Pdf:
  case tDataFormatST_Word:
  case tDataFormatST_Outlook:
    break;
  case tDataFormatST_Xml:
    sExternalEditor = GetEditorForXML();
  default:
    break;
  }
  return sExternalEditor;
}

xtstring CExternalEditors::GetExternalEditorForDataFormat(TSaveTemplateInFormat nDataFormat)
{
  xtstring sExternalEditor;
  switch (nDataFormat)
  {
  case tSaveTemplateInFormat_Native:
    break;
  case tSaveTemplateInFormat_Plain:
    sExternalEditor = GetEditorForPlain();
    break;
  case tSaveTemplateInFormat_Html:
    sExternalEditor = GetEditorForHTML();
    break;
  case tSaveTemplateInFormat_Rtf:
    sExternalEditor = GetEditorForRTF();
    break;
  case tSaveTemplateInFormat_Tag:
    sExternalEditor = GetEditorForTAG();
    break;
  case tSaveTemplateInFormat_Db:
    sExternalEditor = GetEditorForDB();
    break;
  case tSaveTemplateInFormat_Pdf:
  case tSaveTemplateInFormat_Ter:
  case tSaveTemplateInFormat_Word:
  case tSaveTemplateInFormat_WordXML:
  case tSaveTemplateInFormat_Excel:
  case tSaveTemplateInFormat_ExcelXML:
  case tSaveTemplateInFormat_Outlook:
  case tSaveTemplateInFormat_Mapi:
  case tSaveTemplateInFormat_ExecLogFile:
  case tSaveTemplateInFormat_Indesign:
  case tSaveTemplateInFormat_Fdf:
  case tSaveTemplateInFormat_IndesignBook:
  case tSaveTemplateInFormat_IndesignLib:
  case tSaveTemplateInFormat_IndesignTemplate:
  case tSaveTemplateInFormat_IndesignContent:
  default:
    break;
  }
  return sExternalEditor;
}

bool CExternalEditors::ReadXMLDoc(CXPubMarkUp* pXMLDoc)
{
  ResetAll();
  // XMLDoc irgendwo
  if (pXMLDoc->FindChildElem(XMLTAG_EXTERNALEDITORS))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ExternalEditors>

    SetEditorForPlain (pXMLDoc->GetAttrib(XMLTAG_EDITORFOR_PLAIN));
    SetEditorForHTML  (pXMLDoc->GetAttrib(XMLTAG_EDITORFOR_HTML));
    SetEditorForRTF   (pXMLDoc->GetAttrib(XMLTAG_EDITORFOR_RTF));
    SetEditorForTAG   (pXMLDoc->GetAttrib(XMLTAG_EDITORFOR_TAG));
    SetEditorForDB    (pXMLDoc->GetAttrib(XMLTAG_EDITORFOR_DB));
    SetEditorForProp  (pXMLDoc->GetAttrib(XMLTAG_EDITORFOR_PROP));
    SetEditorForXML   (pXMLDoc->GetAttrib(XMLTAG_EDITORFOR_XML));

    pXMLDoc->OutOfElem();
    // XMLDoc irgendwo
  }
  return true;
}

bool CExternalEditors::SaveXMLDoc(CXPubMarkUp* pXMLDoc)
{
  // XMLDoc irgendwo
  if (pXMLDoc->AddChildElem(XMLTAG_EXTERNALEDITORS, _XT("")))
  {
    pXMLDoc->IntoElem();
    // XMLDoc in <ExternalEditors>

    pXMLDoc->SetAttrib(XMLTAG_EDITORFOR_PLAIN,  GetEditorForPlain().c_str());
    pXMLDoc->SetAttrib(XMLTAG_EDITORFOR_HTML,   GetEditorForHTML().c_str());
    pXMLDoc->SetAttrib(XMLTAG_EDITORFOR_RTF,    GetEditorForRTF().c_str());
    pXMLDoc->SetAttrib(XMLTAG_EDITORFOR_TAG,    GetEditorForTAG().c_str());
    pXMLDoc->SetAttrib(XMLTAG_EDITORFOR_DB,     GetEditorForDB().c_str());
    pXMLDoc->SetAttrib(XMLTAG_EDITORFOR_PROP,   GetEditorForProp().c_str());
    pXMLDoc->SetAttrib(XMLTAG_EDITORFOR_XML,    GetEditorForXML().c_str());

    pXMLDoc->OutOfElem();
    // XMLDoc irgendwo
  }
  return true;
}

bool CExternalEditors::CopyFrom(CExternalEditors* pEditors,
                                bool& bEditorsChanged)
{
  if (!pEditors)
    return false;

  bEditorsChanged = false;

  if (GetEditorForPlain() != pEditors->GetEditorForPlain())
    bEditorsChanged = true;
  else if (GetEditorForHTML() != pEditors->GetEditorForHTML())
    bEditorsChanged = true;
  else if (GetEditorForRTF() != pEditors->GetEditorForRTF())
    bEditorsChanged = true;
  else if (GetEditorForTAG() != pEditors->GetEditorForTAG())
    bEditorsChanged = true;
  else if (GetEditorForDB() != pEditors->GetEditorForDB())
    bEditorsChanged = true;
  else if (GetEditorForProp() != pEditors->GetEditorForProp())
    bEditorsChanged = true;
  else if (GetEditorForXML() != pEditors->GetEditorForXML())
    bEditorsChanged = true;

  return AssignFrom(pEditors);
}

bool CExternalEditors::AssignFrom(CExternalEditors* pEditors)
{
  if (!pEditors)
    return false;

  SetEditorForPlain(pEditors->GetEditorForPlain());
  SetEditorForHTML(pEditors->GetEditorForHTML());
  SetEditorForRTF(pEditors->GetEditorForRTF());
  SetEditorForTAG(pEditors->GetEditorForTAG());
  SetEditorForDB(pEditors->GetEditorForDB());
  SetEditorForProp(pEditors->GetEditorForProp());
  SetEditorForXML(pEditors->GetEditorForXML());
  return true;
}

