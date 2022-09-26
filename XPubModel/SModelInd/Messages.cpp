#include <assert.h>
#include "../SSTLdef/STLdef.h"
#include "Messages.h"



CExecutionMessageList::CExecutionMessageList()
{
}

CExecutionMessageList::~CExecutionMessageList()
{
  RemoveAllMessages();
}

bool CExecutionMessageList::AddMessage(CExecutionMessage* pMessage)
{
  CExecutionMessage* pNewMessage;
  pNewMessage = new CExecutionMessage(*pMessage);
//  *pNewMessage = *pMessage;

  m_vMessages.push_back(pNewMessage);
  return true;
}

bool CExecutionMessageList::RemoveAllMessages()
{
  for (CExecutionMessagesVectorIterator it = m_vMessages.begin();
        it != m_vMessages.end();
        it++)
  {
    CExecutionMessage* pMsg = *it;
    delete pMsg;
  }
  m_vMessages.erase(m_vMessages.begin(), m_vMessages.end());
  return true;
}

size_t CExecutionMessageList::GetCountOfMessages() const
{
  return m_vMessages.size();
}

CExecutionMessage* CExecutionMessageList::GetLastMessage()
{
  assert(m_vMessages.size());
  if (!m_vMessages.size())
    return NULL;
  return m_vMessages[m_vMessages.size() - 1];
}

CExecutionMessage* CExecutionMessageList::Get(size_t nIndex) const
{
  assert(m_vMessages.size() > nIndex);
  if (m_vMessages.size() <= nIndex)
    return NULL;
  return m_vMessages[nIndex];
}

void CExecutionMessage::Clear()
{
  m_sModule.clear();
  m_sEntity.clear();
  m_sWorkPaper.clear();
  m_sMsgKeyword.clear();
  m_sMsgText.clear();
  m_sMsgDescription.clear();
}
