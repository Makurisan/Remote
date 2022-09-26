// ModelInd.h: interface for the CModelBaseInd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MODELIND_H_)
#define _MODELIND_H_


#include "ModelExpImp.h"
#include "ModelDef.h"


#include <vector>


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32






class CModelBaseInd;


class XPUBMODEL_EXPORTIMPORT CModelBaseInd
{
public:
  typedef enum TTypeOfModel
  {
    tMPModelFirstObject = 0,
    tMPModel = 1,
    tMPModelConstant,
    tMPModelKey,
    tMPModelText,
    tMPModelLink,
    tMPModelGraphic,
    tMPModelArea,
    tMPModelLastObject,
    tERModelFirstObject = 100,
    tERModelField,
    tERModelParam,
    tERModelRelation,
    tERModelTable,
    tERFreeSelectTables,
    tERModelLastObject,
    tIFSEntityFirstObject = 200,
    tIFSEntityField,
    tIFSEntityConnection,
    tIFSEntity,
    tIFSEntityHitName,
    tIFSEntityAssignment,
    tIFSEntityVariable,
    tIFSEntityPreProcess,
    tIFSEntityPostProcess,
    tIFSEntityOneCondition,
    tIFSEntityConditionVariations,
    tIFSEntityParameters,
    tIFSEntitySQLCommand,
    tIFSEntitySQLCommandVariations,
    tIFSEntityGlobalERTable,
    tIFSEntityLastObject,
    tDBStoneTemplateFirstObject = 300,
    tDBStoneField,
    tDBStone,
    tDBTemplateTable,
    tDBTemplate,
    tDBStoneTemplateLastObject,
    tPropStoneTemplateFirstObject = 400,
    tPropStoneProperties,
    tPropStone,
    tPropStoneTemplateLastObject,
  };
  TTypeOfModel GetTypeOfModel(){return m_nTypeOfModel;};

  // check name functions
  // ret val 'false' -> die sName gar nicht zulassen
  // ret val 'true'  -> die sNewName benutzen / kann sein, dass das zu sName identisch ist
  static bool CheckDefaultName(const xtstring& sName, xtstring& sNewName, const xtstring& sRestrictedChars, LPCXTCHAR* pReplaceTokens);
  static bool CheckModuleName(const xtstring& sName, xtstring& sNewName);
  static bool CheckIFSEntityName(const xtstring& sName, xtstring& sNewName);
  static bool CheckERTableName(const xtstring& sName, xtstring& sNewName);
  static bool CheckERFieldName(const xtstring& sName, xtstring& sNewName);
  static bool CheckERParamName(const xtstring& sName, xtstring& sNewName);
  static bool CheckERRelationName(const xtstring& sName, xtstring& sNewName);
  static bool CheckVariableName(const xtstring& sName, xtstring& sNewName);

  // zur Zeit nur bei Vorlagen und Bausteinen benutzt
  bool PropertiesChanged(){return m_bPropChanged;};
  void ClearPropertiesChanged(){m_bPropChanged = false;};
  void SetPropertiesChanged(){m_bPropChanged = true;};

  static xtstring GetFormatedTime(time_t time, bool bWithSeconds);
  static bool     SetFormatedTime(time_t* pTime, const xtstring& sTime);

protected:
  CModelBaseInd(TTypeOfModel nType);
  virtual ~CModelBaseInd();

  void SetTypeOfModel(TTypeOfModel nTypeOfModel){m_nTypeOfModel = nTypeOfModel;};

private:
  TTypeOfModel  m_nTypeOfModel;

  // zur Zeit nur bei Vorlagen und Bausteinen benutzt
  bool m_bPropChanged;
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_MODELIND_H_)
