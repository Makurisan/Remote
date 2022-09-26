// diese zwei muss man vor #if !defined(xxx) eintragen
#include "ModelExpImp.h"
#include "ModelDef.h"

#if !defined(_MESSAGES_H_)
#define _MESSAGES_H_



#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


typedef enum TSourceOfMessage
{
  tMessageSourceProjectLoad = 1,
  tMessageSourceProjectFileER,
  tMessageSourceProjectFileIFS,
  tMessageSourceProjectFileHTMLStone,
  tMessageSourceProjectFileHTMLTemp,
  tMessageSourceProjectFilePython,

  tMessageSourceExecution,
  tMessageSourceExecutionDataPool,
  tMessageSourceExecutionCreateEntity,
  tMessageSourceExecutionEntity,
  tMessageSourceExecutionOfSubIFS,
  tMessageSourceExecutionGlobalTable,
  tMessageSourceExecutionEntityInformation,
  tMessageSourceEntityAssignments,
  tMessageSourceEntityAssignment_Valid,
  tMessageSourceEntityAssignment_NotValid,
  tMessageSourceEntityAssignment_Error,
  tMessageSourceWorkPaperLoad,
  tMessageSourceWorkPaperCondition,
  tMessageSourceWorkPaperFields,
  tMessageSourceWorkPaperHit,
  tMessageSourceWorkPaperInsertTextField_Valid,
  tMessageSourceWorkPaperInsertTextField_NotValid,
  tMessageSourceWorkPaperInsertTextField_Error,
  tMessageSourceWorkPaperInsertKeyField_Valid,
  tMessageSourceWorkPaperInsertKeyField_NotValid,
  tMessageSourceWorkPaperInsertKeyField_Error,
  tMessageSourceWorkPaperInsertLinkField_Valid,
  tMessageSourceWorkPaperInsertLinkField_NotValid,
  tMessageSourceWorkPaperInsertLinkField_Error,
  tMessageSourceWorkPaperInsertGraphicField_Valid,
  tMessageSourceWorkPaperInsertGraphicField_NotValid,
  tMessageSourceWorkPaperInsertGraphicField_Error,
  tMessageSourceWorkPaperInsertAreaField_Valid,
  tMessageSourceWorkPaperInsertAreaField_NotValid,
  tMessageSourceWorkPaperInsertAreaField_Error,
  tMessageSourceExecutionWorkPaper,
  tMessageSourceExecutionProperty,
  tMessageSourceExecutionPreProcessPrepare,
  tMessageSourceExecutionPostProcessPrepare,
  tMessageSourceExecutionPreProcessExecute,
  tMessageSourceExecutionPostProcessExecute,
  tMessageSourceExecutionPreProcessItem,
  tMessageSourceExecutionPostProcessItem
};


class XPUBMODEL_EXPORTIMPORT CExecutionMessage
{
public:
  typedef enum TTypeOfMessage
  {
    tMessageFatalError,
    tMessageError,
    tMessageWarning,
    tMessageInfo,
  };

  CExecutionMessage(CExecutionMessage* pMessage)
  {
     if (pMessage)
    {
      m_nTypeOfMessage = pMessage->m_nTypeOfMessage;
      m_nSourceOfMessage = pMessage->m_nSourceOfMessage;
      m_nNumber = pMessage->m_nNumber;
      m_sModule = pMessage->m_sModule;
      m_sEntity = pMessage->m_sEntity;
      m_sWorkPaper = pMessage->m_sWorkPaper;
      m_sMsgKeyword = pMessage->m_sMsgKeyword;
      m_sMsgText = pMessage->m_sMsgText;
      m_sMsgDescription = pMessage->m_sMsgDescription;
    }
    else
    {
      Clear();
    }
  };
  CExecutionMessage(TTypeOfMessage nTypeOfMessage, TSourceOfMessage nSourceOfMessage)
  {
    m_nTypeOfMessage = nTypeOfMessage;
    m_nSourceOfMessage = nSourceOfMessage;
    m_nNumber = 0;
    Clear();
  };

  void Clear();
  TTypeOfMessage GetTypeOfMessage(){return m_nTypeOfMessage;};
  void SetTypeOfMessage(TTypeOfMessage nTypeOfMessage){m_nTypeOfMessage = nTypeOfMessage;};
  TSourceOfMessage GetMessageSource(){return m_nSourceOfMessage;};
  void SetMessageSource(TSourceOfMessage nSourceOfMessage){m_nSourceOfMessage = nSourceOfMessage;};

  bool MessageDescriptionExist(){return (m_sMsgDescription.size() != 0);}
  xtstring GetPosition()
  {
    xtstring sRet(m_sModule);
    if (sRet.size() && m_sEntity.size())
      sRet += _XT(" / ");
    sRet += m_sEntity;
    if (sRet.size() && m_sWorkPaper.size())
      sRet += _XT(" / ");
    sRet += m_sWorkPaper;
    return sRet;
  }


  int m_nNumber;
  xtstring m_sModule;           // 3. Spalte in GUI
  xtstring m_sEntity;           // 3. Spalte in GUI
  xtstring m_sWorkPaper;        // 3. Spalte in GUI
  xtstring m_sMsgKeyword;       // 1. Spalte in GUI
  xtstring m_sMsgText;          // 2. Spalte in GUI
  xtstring m_sMsgDescription;

protected:
  TTypeOfMessage    m_nTypeOfMessage;
  TSourceOfMessage  m_nSourceOfMessage;
};


typedef std::vector<CExecutionMessage*> CExecutionMessagesVector;
typedef CExecutionMessagesVector::iterator CExecutionMessagesVectorIterator;
typedef CExecutionMessagesVector::reverse_iterator CExecutionMessagesVectorReverseIterator;

class XPUBMODEL_EXPORTIMPORT CExecutionMessageList
{
public:
  CExecutionMessageList();
  ~CExecutionMessageList();

  bool AddMessage(CExecutionMessage* pMessage);
  bool RemoveAllMessages();
  size_t GetCountOfMessages() const;
  CExecutionMessage* GetLastMessage();
  CExecutionMessage* Get(size_t nIndex) const;
protected:
  CExecutionMessagesVector m_vMessages;
};

#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_MESSAGES_H_)
