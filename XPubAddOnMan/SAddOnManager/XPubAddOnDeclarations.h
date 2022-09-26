#if !defined(_XPUBADDONDECLARATIONS_H_)
#define _XPUBADDONDECLARATIONS_H_

#include "AddOnManagerExpImp.h"

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32

class CProjectFileER;
class CDBDefinition;


////////////////////////////////////////////////////////////////////////////////
// Klasse CStateVarsChannel ist eine rein virtualle CallBack Klasse.
// Von dieser Klasse muss irgendwo eine Klasse abgeleitet sein
// und die Funktion implementiert sein.
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CChannelToExecutedEntity
{
public:
  virtual long GetSVLoopCount() = 0;
  virtual long GetSVCountTrue() = 0;
  virtual long GetSVCountFalse() = 0;
  virtual long GetSVOrder() = 0;
  virtual long GetSVMainCondition() = 0;
  virtual long GetSVInsertCount() = 0;
  virtual long GetSVCountDBRows() = 0;
  virtual long GetSVCountDBCols() = 0;
  virtual long GetSVActualDBRow() = 0;

  virtual long GetSVCreatedTemplates() = 0;

  virtual bool GetSVProductionExecution() = 0;
  virtual bool GetSVDynamicExecution() = 0;
  virtual bool GetSVDebugExecution() = 0;

};

////////////////////////////////////////////////////////////////////////////////
// Klasse CStateVarsChannel ist eine rein virtualle CallBack Klasse.
// Von dieser Klasse muss irgendwo eine Klasse abgeleitet sein
// und die Funktion implementiert sein.
////////////////////////////////////////////////////////////////////////////////
class XPUBADDONMANAGER_EXPORTIMPORT CChanelToExecutionModule
{
public:
  virtual const CProjectFileER* GetERModel(const xtstring& sERModelName) = 0;
  virtual const CDBDefinition* GetDBDefinition(const xtstring& sSymbolicDBName) = 0;
  virtual xtstring GetExecUser() = 0;
  virtual xtstring GetProjectFolder() = 0;
  virtual xtstring GetSessionID() = 0;
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_XPUBADDONDECLARATIONS_H_)

