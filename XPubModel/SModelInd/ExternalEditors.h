#if !defined(_EXTERNALEDITORS_H_)
#define _EXTERNALEDITORS_H_


#include "ModelExpImp.h"
#include "ModelDef.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32

class CXPubMarkUp;

#define STANDALONE_EXTERNALEDITOR       _XT("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\r\n<StandAloneExternalEditor></StandAloneExternalEditor>\r\n")
#define STANDALONE_EXTERNALEDITOR_TAG   _XT("StandAloneExternalEditor")

#define XMLTAG_EXTERNALEDITORS                            _XT("ExternalEditors")
#define XMLTAG_EDITORFOR_PLAIN                            _XT("EditorForPlain")
#define XMLTAG_EDITORFOR_HTML                             _XT("EditorForHTML")
#define XMLTAG_EDITORFOR_RTF                              _XT("EditorForRTF")
#define XMLTAG_EDITORFOR_TAG                              _XT("EditorForTAG")
#define XMLTAG_EDITORFOR_DB                               _XT("EditorForDB")
#define XMLTAG_EDITORFOR_PROP                             _XT("EditorForProp")
#define XMLTAG_EDITORFOR_XML                              _XT("EditorForXML")



class XPUBMODEL_EXPORTIMPORT CExternalEditors
{
public:
  CExternalEditors();
  ~CExternalEditors();

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc);

  bool CopyFrom(CExternalEditors* pEditors, bool& bEditorsChanged);
  bool AssignFrom(CExternalEditors* pEditors);

  void ResetAll();

  xtstring GetExternalEditorForDataFormat(TDataFormatForStoneAndTemplate nDataFormat);
  xtstring GetExternalEditorForDataFormat(TSaveTemplateInFormat nDataFormat);

  xtstring GetEditorForPlain(){return m_sEditorForPlain;};
  void SetEditorForPlain(const xtstring& sEditorForPlain){m_sEditorForPlain = sEditorForPlain;};
  xtstring GetEditorForHTML(){return m_sEditorForHTML;};
  void SetEditorForHTML(const xtstring& sEditorForHTML){m_sEditorForHTML = sEditorForHTML;};
  xtstring GetEditorForRTF(){return m_sEditorForRTF;}
  void SetEditorForRTF(const xtstring& sEditorForRTF){m_sEditorForRTF = sEditorForRTF;};
  xtstring GetEditorForTAG(){return m_sEditorForTAG;};
  void SetEditorForTAG(const xtstring& sEditorForTAG){m_sEditorForTAG = sEditorForTAG;};
  xtstring GetEditorForDB(){return m_sEditorForDB;};
  void SetEditorForDB(const xtstring& sEditorForDB){m_sEditorForDB = sEditorForDB;};
  xtstring GetEditorForProp(){return m_sEditorForProp;};
  void SetEditorForProp(const xtstring& sEditorForProp){m_sEditorForProp = sEditorForProp;};
  xtstring GetEditorForXML(){return m_sEditorForXML;};
  void SetEditorForXML(const xtstring& sEditorForXML){m_sEditorForXML = sEditorForXML;};

protected:
  xtstring  m_sEditorForPlain;
  xtstring  m_sEditorForHTML;
  xtstring  m_sEditorForRTF;
  xtstring  m_sEditorForTAG;
  xtstring  m_sEditorForDB;
  xtstring  m_sEditorForProp;
  xtstring  m_sEditorForXML;
};

#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_EXTERNALEDITORS_H_)
