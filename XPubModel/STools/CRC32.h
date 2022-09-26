#if !defined(_CRC32_H_)
#define _CRC32_H_

#include "SModelInd/ModelExpImp.h"



class XPUBMODEL_EXPORTIMPORT CCRC32
{
public:
  CCRC32();
  ~CCRC32();

  void Reset();
  void Compute(unsigned char *pData, unsigned long uSize);
  unsigned long GetCRC32();

protected:
  unsigned long m_nCRC32;
};


#endif // !defined(_CRC32_H_)
