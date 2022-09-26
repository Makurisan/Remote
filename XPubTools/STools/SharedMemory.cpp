// SharedMemory.cpp: implementation of the CSharedMemory class.
//
//////////////////////////////////////////////////////////////////////

#include "SharedMemory.h"
#include "assert.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSharedMemory::CSharedMemory()
{
  m_hMutex    = 0;
  m_hMapping  = 0;

  m_pBuffer   = 0;
  m_nOffset   = 0;
  m_nSize     = 0;
}


CSharedMemory::~CSharedMemory()
{
  Delete();
}


bool CSharedMemory::Create(size_t nSize, LPCTSTR pszName)
{
  // If the following ASSERT fires, you're calling Create on a shared memory object
  // that is already initialized.
  assert(m_hMapping == 0);

  SECURITY_DESCRIPTOR sd;
  InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
  SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = &sd;
  sa.bInheritHandle = FALSE;

  // Create a file mapping object.
  HANDLE hMapping = ::CreateFileMapping((HANDLE)INVALID_HANDLE_VALUE,
                                        &sa,//NULL,
                                        PAGE_READWRITE /*SETCOM*/,
                                        0,
                                        nSize + sizeof(size_t), pszName);

  if (hMapping == NULL || GetLastError() == ERROR_ALREADY_EXISTS)
    return false;

  // Store pertinent data for later.
  strcpy(m_szName, pszName);
  strcpy(m_szMutexName, pszName);
  m_hMapping = hMapping;
  m_nSize    = nSize;

  // buff: |size|..................
  void* pMem = ::MapViewOfFile(hMapping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
  if(!pMem)
     return false;
  *((size_t*)pMem) = m_nSize;
  ::UnmapViewOfFile( pMem );

  return true;
}


bool CSharedMemory::Open(LPCTSTR pszName)
{
  HANDLE hMapping = ::OpenFileMapping(FILE_MAP_WRITE | FILE_MAP_READ, FALSE, pszName);

  if (hMapping == NULL)// || GetLastError() == ERROR_ALREADY_EXISTS)
  {
    DWORD dw = GetLastError();
    return false;
  }

  // Store pertinent data for later.
  strcpy(m_szName, pszName);
  m_hMapping = hMapping;

  // buff: |size|..................
  void* pMem = ::MapViewOfFile(m_hMapping, FILE_MAP_WRITE|FILE_MAP_READ, 0, 0, 0);
  if(!pMem)
     return FALSE;
 ::CopyMemory(&m_nSize, pMem, sizeof(size_t));
 ::UnmapViewOfFile( pMem );

  return true;
}


bool CSharedMemory::Mapping()
{
  assert(m_hMapping != NULL);

  void* pMem = ::MapViewOfFile(m_hMapping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
  if (!pMem)
    return false;

  m_pBuffer = (PBYTE)pMem + sizeof(size_t);
  Seek(0);

  return true;
}


void CSharedMemory::UnMapping()
{
  if (m_pBuffer)
    ::UnmapViewOfFile(m_pBuffer - sizeof(size_t));
  m_pBuffer = NULL;
}




bool CSharedMemory::Delete()
{
  m_nSize     = 0;
  m_szName[0] = '\0';

  if (m_hMutex)
   UnLock();

  if (m_hMapping)
    ::CloseHandle(m_hMapping);
  m_hMapping = NULL;

  return TRUE;
}



bool CSharedMemory::Write(LPCVOID pBuf, size_t nBytesToWrite, size_t* pBytesWritten, bool bTerminate)
{
  assert(m_hMapping != NULL);

  if (!m_pBuffer || m_nOffset >= m_nSize)
  {
    if (pBytesWritten)
      *pBytesWritten = 0;
    return false;
  }

  size_t nCount = min(nBytesToWrite, m_nSize - m_nOffset);
  ::CopyMemory(m_pBuffer + m_nOffset, pBuf, nCount);

//  if (!m_nOffset)
//    *((LPBYTE)m_pBuffer + nCount++) = '\0';
  if (bTerminate)
    *((LPBYTE)m_pBuffer + m_nOffset + nCount++) = '\0';
  m_nOffset += nCount;

  if (pBytesWritten)
    *pBytesWritten = nCount;

  return true;
}



bool CSharedMemory::Read(LPVOID pBuf, size_t nBytesToRead, size_t* pBytesRead)
{
  assert(m_hMapping != NULL);

  if (!m_pBuffer || m_nOffset >= m_nSize)
  {
    if (pBytesRead)
      *pBytesRead = 0;
    return false;
  }

  size_t nCount = min(nBytesToRead, m_nSize - m_nOffset);
  ::CopyMemory(pBuf, m_pBuffer + m_nOffset, nCount);

//  if (!m_nOffset)
//    nCount = strlen((LPCTSTR)pBuf) + 1;
  m_nOffset += nCount;

  if (pBytesRead)
    *pBytesRead = nCount;

  return true;
}



bool CSharedMemory::Lock()
{
  assert(m_hMutex == NULL);
  m_hMutex = ::CreateMutex(NULL, FALSE, m_szMutexName);

  if (m_hMutex == NULL)
   return false;

  return true;
}


void CSharedMemory::UnLock()
{
  if (m_hMutex)
    ::ReleaseMutex(m_hMutex);
}
