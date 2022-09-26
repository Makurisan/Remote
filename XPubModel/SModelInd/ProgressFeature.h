#if !defined(_PROGRESSFEATURE_H_)
#define _PROGRESSFEATURE_H_


#include "ModelExpImp.h"

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32


#define PROGRESSBAR_SIZE        1000
#define PROGRESSBAR_COUNT_SIZE  1000.0

#define MAX_DEEP_OF_PROGRESS_OBJECTS  100

class CProgressObject
{
public:
  CProgressObject(size_t nCountOfIntervals);
  void Step();

  double GetFactorOfHandledIntervals();
  double GetFactorOfOneInterval();

protected:
  size_t m_nCountOfIntervals;
  size_t m_nCountOfHandledIntervals;
};

class XPUBMODEL_EXPORTIMPORT CProgressFeature
{
protected:
  CProgressFeature();
  virtual ~CProgressFeature();
private:
  CvoidVector   m_cPObjects;
  size_t        m_nCountOfNotCreatedObjects;
  size_t        m_nDeadLockCount;
  size_t        m_nCountOfStepsInProgress;
public:
  void ShowProgressBar(bool bShow);

  void SetProgress();

  void CreateNewProgressObject(int nCountOfIntervals);
  void ReleaseProgressObject();
  void StepProgressObject();

  virtual void OnStartOfProgress() = 0;
  virtual void OnEndOfProgress() = 0;
  virtual void OnSetProgressPos(long nPercentPos, long nAbsolutePos) = 0;
};

#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32


#endif // _PROGRESSFEATURE_H_
