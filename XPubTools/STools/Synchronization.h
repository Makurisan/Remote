/* TIN *****************************************************************************
*
*  HEADER:       ThreadSynch.h
*
*  DESCRIPTION:  implementations in a CThreadSemaphore
*                                     CMutexLock  
*                                     CThreadEvent  
* 
***********************************************************************************/


#if !defined(_SYNCHRONIZATION__INCLUDED_)
#define      _SYNCHRONIZATION__INCLUDED_


#include "XPubToolsExpImp.h"
#include "types.h"


#if defined(LINUX_OS)
 
 #include <pthread.h>
 #include <semaphore.h>
 #define SEMAPHORE_T    sem_t*
 #define MUTEX_T        pthread_mutex_t
 #define THREAD_T       pthread_t
 #define EVENT_T        pthread_cond_t*

#elif defined(WIN32)
 
 #include <windows.h>
 #define SEMAPHORE_T    HANDLE 
 #define MUTEX_T        CRITICAL_SECTION
 #define THREAD_T       HANDLE
 #define EVENT_T        HANDLE

#elif defined(MAC_OS_MWE)

 #define __dead2 
 #define __pure2 
 
 #include <sys/unistd.h>
 #include <pthread.h>
 #include <semaphore.h>
 
 #define SEMAPHORE_T    sem_t*
 #define MUTEX_T        pthread_mutex_t
 #define THREAD_T       pthread_t
 #define EVENT_T        pthread_cond_t*


#else
#error "not implemented"
#endif


/* TIN *****************************************************************************
*
*  CLASS:       CSFSemaphore
*
*  DESCRIPTION: This class working with semaphore
*
***********************************************************************************/

class XPUBTOOLS_EXPORTIMPORT CSFSemaphore
{
public:
  CSFSemaphore();
  CSFSemaphore(UINT nAvailable, UINT nMax, LPCTSTR lpszName = 0);
  virtual ~CSFSemaphore();

  bool Created(){return (semaphore != 0);};
public:
  bool Create(UINT nAvailable, UINT nMax, LPCTSTR lpszName = 0);
  
  bool Open(LPCTSTR lpszName);
  void Release();
  void Release(int how_many);
  bool Wait(int nMillSec = -1);

private:
  SEMAPHORE_T semaphore;
};


/* TIN *****************************************************************************
*
*  CLASS:       CSFMutexLock
*
*  DESCRIPTION: This class working with mutex primitive is non-recursive 
*
***********************************************************************************/
class XPUBTOOLS_EXPORTIMPORT CSFMutexLock
{
public:
  CSFMutexLock();
  virtual ~CSFMutexLock();

public:
  void Lock();
  void Unlock();

private:
  MUTEX_T         structure_lock; // structure access lock
  MUTEX_T         lock;           // mutex
  THREAD_T        owner;          // owner of mutex
  UINT            count;          // # of times owner locked mutex
};




/* TIN *****************************************************************************
*
*  CLASS:       CThreadEvent
*
*  DESCRIPTION: The following is the implementation of the Event class
*
***********************************************************************************/
class XPUBTOOLS_EXPORTIMPORT CSFEvent
{
public:  
  CSFEvent(); // for open existing
  CSFEvent(bool bInitState, LPCTSTR lpszName = 0);
  
  virtual ~CSFEvent();

public:
  bool Open(LPCTSTR lpName);         // object name

  void Signal();
  bool Wait(int nMillSec = -1);

  EVENT_T GetEvent(){return event;};

// members
private:
  EVENT_T event;
  // in order to wait on a condition variable you must have a locked
  // mutex, which we don't really need.
  MUTEX_T dummy_mutex;

};










/*
// The following is the implementation of the Gate class on Solaris 2.x.
class CThreadEvent
{

public:
  CThreadEvent();
  {
    mutex_init(&structure_lock, USYNC_PROCESS, (void*)0);
    cond_init(&wakeup, USYNC_PROCESS, (void*)0);
    closed = 1;
  }

  ~CThreadEvent();
  {
    mutex_destroy(&structure_lock);
    cond_destroy(&wakeup);
  }

  void Set();
  {
    mutex_lock(&structure_lock);
    closed = 0;
    cond_broadcast(&wakeup);
    mutex_unlock(&structure_lock);
  }

  void Close();
  {
    mutex_lock(&structure_lock);
    closed = 1;
    mutex_unlock(&structure_lock);
  }

  // temporarily open the gate, allowing any waiting threads to
  // proceed
  void Release();
  {
    cond_broadcast(&wakeup);
  }

  void Wait();
  {
    mutex_lock(&structure_lock);
    if ( closed )
      cond_wait(&wakeup, &structure_lock);
    mutex_unlock(&structure_lock);
  }

private:
  mutex_t structure_lock;
  cond_t  wakeup;
  int     closed;

};

Win32 (Windows NT and Windows 95) Gate Class Implementation
class Gate {
private:
  HANDLE gate;

public:
  Gate(void) {
    gate = CreateEvent((void*), TRUE, FALSE, (void*)0);
  }

  ~Gate(void) {
    CloseHandle(gate);
  }

  void Open(void) {
    SetEvent(gate);
  };

  void Close(void) {
    ResetEvent(gate);
  }

  // temporarily open the gate, allowing any waiting threads to
  // proceed
  void Release(void) {
    PulseEvent(gate);
  }

  void Wait(void) {
    WaitForSingleObject(gate, INFINITE);
  }
};
*/


#endif
