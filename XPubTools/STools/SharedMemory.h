/* TIN *****************************************************************************
*
* HEADER: SharedMemory.h                         12.11.2001
*
* NOTE:   
*
***********************************************************************************/
#if !defined(_SHAREDMEMORY_H__INCLUDED_)
#define      _SHAREDMEMORY_H__INCLUDED_


#include "XPubToolsExpImp.h"
#include "types.h"

#if defined (LINUX_OS)

#include <sys/types.h>

#elif defined(WIN32)
#include "windows.h"
#include "stdlib.h"

#elif defined(MAC_OS_MWE)

#include <types.h>

typedef long HANDLE;
#define MAX_PATH 255

#else
#error "not implemented"
#endif

/* TIN *****************************************************************************
*
* CLASS: CSharedMemory
*
* NOTE:  A description was not provided.
*
***********************************************************************************/
class XPUBTOOLS_EXPORTIMPORT CSharedMemory  
{
public:
  CSharedMemory();
  virtual ~CSharedMemory();

public:
  bool Create(size_t nSize, LPCTSTR pszName);
  bool Open(LPCTSTR pszName);
  bool Delete();

  // map file
  bool Mapping();
  void UnMapping();

  bool Write(LPCVOID pBuf, size_t nBytesToWrite, size_t* pBytesWritten, bool bTerminate);
  bool Read(LPVOID pBuf, size_t nBytesToRead, size_t* pBytesRead);

  void Seek(size_t nOff){m_nOffset = nOff;};
  size_t GetSize(){return m_nSize;};

protected:
  // mutex 
  bool Lock();
  void UnLock();

  size_t        m_nSize;        // Memory block size
  HANDLE        m_hMapping;     // File mapping handle
  char          m_szName[MAX_PATH];  // Object name
  char          m_szMutexName[MAX_PATH];
  HANDLE        m_hMutex;       // handle to mutex for synchronizing reads and writes

  LPBYTE        m_pBuffer;
  size_t        m_nOffset;
  
};

#endif // !defined(_SHAREDMEMORY_H__INCLUDED_)
