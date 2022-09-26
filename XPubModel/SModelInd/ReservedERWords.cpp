#include <assert.h>
#include "SSTLdef/STLdef.h"
#include "ReservedERWords.h"




ResSQLKeyword CReservedERWords::m_arrReservedKeywords[] =
{
  _XT("AFTER"), _XT("ALL"), _XT("AND"), _XT("ANY"), _XT("AS"), _XT("ASC"), _XT("ATTRIBUTE"), _XT("AVG"),
  _XT("BEFORE"), _XT("BETWEEN"), _XT("BLOB"), _XT("BOOL"), _XT("BOOLEAN"), _XT("BY"), _XT("CHAR"),
  _XT("CHARACTER"), _XT("CHARINDEX"), _XT("CHILDREN"), _XT("CONTAINS"), _XT("COUNT"),
  _XT("COUNTER"), _XT("CREATE"), _XT("CURRENCY"), _XT("DATE"), _XT("DATETIME"), _XT("SQLDELETE"),
  _XT("DESC"), _XT("DISTINCT"), _XT("DOCUMENT"), _XT("DOUBLE"), _XT("DROP"), _XT("EXISTS"),
  _XT("FALSE"), _XT("FIRST"), _XT("FLOAT"), _XT("FOR"), _XT("FROM"), _XT("GROUP"), _XT("HAVING"),
  _XT("IN"), _XT("INDEX"), _XT("INSERT"), _XT("INT"), _XT("INTEGER"), _XT("INTO"), _XT("IS"), _XT("KEY"),
  _XT("LAST"), _XT("LEFT")
};

 



CReservedERWords::CReservedERWords()
{
}

CReservedERWords::~CReservedERWords()
{
}


bool CReservedERWords::IsReservedWord(const xtstring& sWord)
{
  size_t nIndex = 0;
  while (m_arrReservedKeywords[nIndex].pKeyword)
  {
    if (sWord.CompareNoCase(m_arrReservedKeywords[nIndex].pKeyword) == 0)
      return true;
    nIndex++;
  }
  return false;
}

bool CReservedERWords::IsReservedWord(LPCXTCHAR pWord)
{
  if (!pWord)
    return true;
  return IsReservedWord(xtstring(pWord));
}
