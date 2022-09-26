#if !defined(_CONSTANTS_H_)
#define _CONSTANTS_H_


#include "ModelExpImp.h"


class CXPubMarkUp;


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32




class XPUBMODEL_EXPORTIMPORT CProjectConstant
{
public:
  CProjectConstant();
  ~CProjectConstant();

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc) const;

  void operator =(const CProjectConstant* pProjConstant);

  xtstring GetName(){return m_sName;};
  void SetName(const xtstring& sName){m_sName = sName;};
  CConditionOrAssignment&         GetAssignment(){return m_caAssignment;};
  const CConditionOrAssignment&   GetAssignment_Const() const {return m_caAssignment;};
  CConditionOrAssignment*         GetAssignment_Ptr(){return &m_caAssignment;};

protected:
  xtstring                m_sName;
  CConditionOrAssignment  m_caAssignment;
};


typedef map<xtstring, CProjectConstant*>  CConstantMap;
typedef CConstantMap::iterator            CConstantMapIterarot;
typedef CConstantMap::const_iterator      CConstantMapConstIterarot;
typedef CConstantMap::reverse_iterator    CConstantMapReverseIterarot;



class XPUBMODEL_EXPORTIMPORT CProjectConstantArray
{
public:
  CProjectConstantArray();
  CProjectConstantArray(LPCXTCHAR pArrayName);
  ~CProjectConstantArray();

  void RemoveAll();
  bool CopyFrom(const CProjectConstantArray* pArray);

  size_t  CountOfConstants();
  bool    GetConstant(const xtstring& sName, CConditionOrAssignment& caAssignment) const;
  bool    GetConstant(size_t nIndex, xtstring& sName, CConditionOrAssignment& caAssignment) const;
  bool    SetConstant(const xtstring& sName, const CConditionOrAssignment& caAssignment);
  bool    SetConstant(size_t nIndex, const CConditionOrAssignment& caAssignment);
  bool    AddConstant(const xtstring& sName, const CConditionOrAssignment& caAssignment, size_t& nPos);
  bool    RemoveConstant(size_t nIndex);
  bool    RemoveConstant(const xtstring& sName);

  bool ReadXMLDoc(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDoc(CXPubMarkUp* pXMLDoc) const;

  bool  GetXMLDoc(xtstring& sXML) const;
  bool  SetXMLDoc(const xtstring& sXML, bool bAnyConstants);

protected:
  bool ReadXMLDocRemainder(CXPubMarkUp* pXMLDoc);
  bool SaveXMLDocRemainder(CXPubMarkUp* pXMLDoc) const;

  xtstring  m_sArrayName;
  CConstantMap  m_cConstants;
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_CONSTANTS_H_)

