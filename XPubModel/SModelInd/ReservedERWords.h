#if !defined(_RESERVEDERWORDS_H_)
#define _RESERVEDERWORDS_H_


#include "ModelExpImp.h"


#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


typedef struct _ResSQLKeyword
{
  XTCHAR* pKeyword;
}
ResSQLKeyword;



class XPUBMODEL_EXPORTIMPORT CReservedERWords
{
public:
  CReservedERWords();
  ~CReservedERWords();

  bool IsReservedWord(const xtstring& sWord);
  bool IsReservedWord(LPCXTCHAR pWord);

private:
  static ResSQLKeyword m_arrReservedKeywords[];
};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // !defined(_RESERVEDERWORDS_H_)

