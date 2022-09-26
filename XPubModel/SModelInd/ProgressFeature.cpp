#include "SSTLdef/STLdef.h"
#include "ProgressFeature.h"




CProgressObject::CProgressObject(size_t nCountOfIntervals)
{
  m_nCountOfIntervals = nCountOfIntervals;
  m_nCountOfHandledIntervals = 0;
}

void CProgressObject::Step()
{
  m_nCountOfHandledIntervals++;
//  assert(m_nCountOfHandledIntervals <= m_nCountOfIntervals);
  if (m_nCountOfHandledIntervals > m_nCountOfIntervals)
    m_nCountOfHandledIntervals = m_nCountOfIntervals;
}

double CProgressObject::GetFactorOfHandledIntervals()
{
  double dResult;
  dResult = m_nCountOfHandledIntervals;
  dResult /= m_nCountOfIntervals;
  return dResult;
}

double CProgressObject::GetFactorOfOneInterval()
{
  double dResult;
  dResult = 1.0;
  dResult /= m_nCountOfIntervals;
  return dResult;
}





CProgressFeature::CProgressFeature()
{
  m_nCountOfNotCreatedObjects = 0;
  m_nDeadLockCount = 0;
  m_nCountOfStepsInProgress = 0;
}

CProgressFeature::~CProgressFeature()
{
  for (CvoidVectorIterator it = m_cPObjects.begin(); it != m_cPObjects.end(); it++)
    delete ((CProgressObject*)(*it));
  m_cPObjects.erase(m_cPObjects.begin(), m_cPObjects.end());
}

void CProgressFeature::ShowProgressBar(bool bShow)
{
  for (CvoidVectorIterator it = m_cPObjects.begin(); it != m_cPObjects.end(); it++)
    delete ((CProgressObject*)(*it));
  m_cPObjects.erase(m_cPObjects.begin(), m_cPObjects.end());

  if (bShow)
  {
    m_nCountOfStepsInProgress = 0;
    OnStartOfProgress();
  }
  else
    OnEndOfProgress();
}

void CProgressFeature::CreateNewProgressObject(int nCountOfIntervals)
{
  if (m_nDeadLockCount)
  {
    m_nDeadLockCount++;
  }
  else
  {
    if (m_cPObjects.size() < MAX_DEEP_OF_PROGRESS_OBJECTS)
    {
      CProgressObject* pNewObject = new CProgressObject(nCountOfIntervals);
      if (!pNewObject)
        m_nDeadLockCount++;
      else
        m_cPObjects.push_back(pNewObject);
    }
    else
      m_nCountOfNotCreatedObjects++;
  }
}

void CProgressFeature::ReleaseProgressObject()
{
  if (m_nDeadLockCount)
  {
    m_nDeadLockCount--;
  }
  else
  {
    if (m_nCountOfNotCreatedObjects)
    {
      m_nCountOfNotCreatedObjects--;
    }
    else
    {
      if (m_cPObjects.size())
      {
        CvoidVectorReverseIterator it;
        it = m_cPObjects.rbegin();
        CProgressObject* pObject = (CProgressObject*)(*it);
        delete pObject;
        m_cPObjects.erase(m_cPObjects.end() - 1);
      }
    }
  }
}

void CProgressFeature::StepProgressObject()
{
  if (m_nDeadLockCount)
    return;
  if (m_cPObjects.size() >= MAX_DEEP_OF_PROGRESS_OBJECTS)
    return;

  CvoidVectorReverseIterator it;
  it = m_cPObjects.rbegin();
  CProgressObject* pObject = (CProgressObject*)(*it);
  pObject->Step();

  SetProgress();
}

void CProgressFeature::SetProgress()
{
  m_nCountOfStepsInProgress++;
  size_t nCount = m_cPObjects.size();

  double dOverAllFactor = 0.0;
  for (CvoidVectorReverseIterator it = m_cPObjects.rbegin(); it != m_cPObjects.rend(); it++)
  {
    CProgressObject* pObject = (CProgressObject*)(*it);
    dOverAllFactor = dOverAllFactor * pObject->GetFactorOfOneInterval()
                    + pObject->GetFactorOfHandledIntervals();
  }
  double dResult = 0.0;
  dResult = dOverAllFactor * PROGRESSBAR_COUNT_SIZE;
  long nPos;
  nPos = (long)dResult;
  OnSetProgressPos(nPos, m_nCountOfStepsInProgress);
}


