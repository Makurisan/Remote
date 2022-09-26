/* TIN *****************************************************************************
*
*  FILE:         ThreadSynch.cpp
*
*  DESCRIPTION: 
*
***********************************************************************************/
#include <assert.h>
#include "Synchronization.h"


////////////////////////////////////////////////////////////////////////////////////
//                   CSFSemaphore class implementation     
////////////////////////////////////////////////////////////////////////////////////

CSFSemaphore::CSFSemaphore()
{
  semaphore = 0;
}

CSFSemaphore::CSFSemaphore(UINT nAvailable, UINT nMax, LPCTSTR lpszName /*=0*/)
{
  semaphore = 0;
  Create(nAvailable, nMax, lpszName);
}

CSFSemaphore::~CSFSemaphore()
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  if (Created())
  {
    sem_destroy(semaphore);
    delete semaphore;
  }
#elif defined(WIN32)
  if (Created())
    ::CloseHandle(semaphore);
#else
#error "not implemented"
#endif
  semaphore = 0;
}

bool CSFSemaphore::Create(UINT nAvailable, UINT nMax, LPCTSTR lpszName/*=0*/)
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  nMax = nMax; // not used
  if (Created())
  {
    sem_destroy(semaphore);
    delete semaphore;
  }
  semaphore = new sem_t;
  return sem_init(semaphore, 0, nAvailable);
#elif defined(WIN32)

  SECURITY_DESCRIPTOR sd;
  InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
  SetSecurityDescriptorDacl(&sd, TRUE, 0, FALSE);
  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = &sd;
  sa.bInheritHandle = FALSE;

  semaphore = ::CreateSemaphore(&sa, nAvailable, nMax, lpszName);
  return (semaphore != 0);
#else
#error "not implemented"
#endif
}

bool CSFSemaphore::Open(LPCTSTR lpszName)
{
#if defined(LINUX_OS)
  return false;
#elif defined(WIN32)
  assert(semaphore == 0);
  semaphore = ::OpenSemaphore(SEMAPHORE_ALL_ACCESS/*SYNCHRONIZE*/, TRUE, lpszName);

  if (!semaphore)
    return false;

  return true;

#elif defined(MAC_OS_MWE)  
  return true;
#else
#error "not implemented"
#endif
}

void CSFSemaphore::Release()
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  sem_post(semaphore);
#elif defined(WIN32)
  BOOL b = ::ReleaseSemaphore(semaphore, 1, NULL);
  DWORD dw = GetLastError();
#else
#error "not implemented"
#endif
}

void CSFSemaphore::Release(int nHowMany)
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  while (nHowMany-- > 0)
   sem_post(semaphore);
#elif defined(WIN32)
  BOOL b = ::ReleaseSemaphore(semaphore, nHowMany, NULL);
  DWORD dw = GetLastError();
#else
#error "not implemented"
#endif
}

bool CSFSemaphore::Wait(int nMillSec /*=-1*/)
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  // Da ist noch nicht nMillSec beruecksichtigt.
  sem_wait(semaphore);
  return true;
#elif defined(WIN32)
  if (nMillSec == -1)
    nMillSec = INFINITE;
  return (WAIT_OBJECT_0 == ::WaitForSingleObject(semaphore, nMillSec));
#else
#error "not implemented"
#endif
}








////////////////////////////////////////////////////////////////////////////////////
//                       CSFMutexLock class implementation     
////////////////////////////////////////////////////////////////////////////////////


CSFMutexLock::CSFMutexLock()
{                            // pthread_mutexattr_t
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  pthread_mutex_init(&structure_lock, /*USYNC_THREAD*/0);
  pthread_mutex_init(&lock, /*USYNC_THREAD*/0);
#elif defined(WIN32)
  ::InitializeCriticalSection(&lock);
#else
#error "not implemented"
#endif
}

CSFMutexLock::~CSFMutexLock()
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  pthread_mutex_destroy(&structure_lock);
  pthread_mutex_destroy(&lock);
#elif defined(WIN32)
  ::DeleteCriticalSection(&lock);
#else
#error "not implemented"
#endif
}

void CSFMutexLock::Lock()
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  pthread_mutex_lock(&structure_lock);
  if (owner == pthread_self())
  { // this thread already owns the lock
    count++;
    pthread_mutex_unlock(&structure_lock);
  }
  else
  {
    // try to obtain the mutex.  if we cannot do so immediately, release
    // the structure lock and wait on the mutex.
    if (pthread_mutex_trylock(&lock) != 0)
    {
       pthread_mutex_unlock(&structure_lock);
       pthread_mutex_lock(&lock);
       pthread_mutex_lock(&structure_lock);
       owner = pthread_self();
     }
     pthread_mutex_unlock(&structure_lock);
  }
#elif defined(WIN32)
  ::EnterCriticalSection(&lock);
#else
#error "not implemented"
#endif
}

void CSFMutexLock::Unlock()
{

#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  pthread_mutex_lock(&structure_lock);
  if ((owner == pthread_self()) && (--count == 0))
    pthread_mutex_unlock(&lock);
  pthread_mutex_unlock(&structure_lock);
#elif defined(WIN32)
  ::LeaveCriticalSection(&lock);
#else
#error "not implemented"
#endif
}




////////////////////////////////////////////////////////////////////////////////////
//                       CSFEvent class implementation     
////////////////////////////////////////////////////////////////////////////////////


CSFEvent::CSFEvent()
{
  event = 0;
}

CSFEvent::CSFEvent(bool bInitState, LPCTSTR lpszName)
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  event = new pthread_cond_t;
  pthread_cond_init(event, 0);
  pthread_mutex_init(&dummy_mutex, 0);
#elif defined(WIN32)
  event = ::CreateEvent(NULL, bInitState, FALSE, lpszName);
#else
#error "not implemented"
#endif
}

CSFEvent::~CSFEvent()
{
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  if (event)
    pthread_cond_destroy(event);
#elif defined(WIN32)
  ::CloseHandle(event);
#else
#error "not implemented"
#endif
}

bool CSFEvent::Open(LPCTSTR lpszName)
{
  assert(event == 0);
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  return false;
#elif defined(WIN32)
  event = ::OpenEvent(EVENT_ALL_ACCESS, TRUE, lpszName);

  if (!event)
    return false;
  return true;
#else
#error "not implemented"
#endif
}

void CSFEvent::Signal()
{
  assert(event != 0);
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  if (event)
    pthread_cond_broadcast(event);
#elif defined(WIN32)
  //::PulseEvent(event);
  ::SetEvent(event);
#else
#error "not implemented"
#endif
}

bool CSFEvent::Wait(int nMillSec /*=-1 */)
{
  assert(event != 0);
#if defined(LINUX_OS) || defined(MAC_OS_MWE)  
  if (event)
  {
    pthread_mutex_lock(&dummy_mutex);
    pthread_cond_wait(event, &dummy_mutex);
    pthread_mutex_unlock(&dummy_mutex);
    return true;
  }
  return false;
#elif defined(WIN32)
  if (nMillSec == -1)
    nMillSec = INFINITE;
  if (::WaitForSingleObject(event, nMillSec) == WAIT_TIMEOUT)
    return false;
  return true;
#else
#error "not implemented"
#endif
}

