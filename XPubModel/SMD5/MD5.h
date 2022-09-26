#if !defined(_MD5_H_)
#define _MD5_H_


#include "SModelInd/ModelExpImp.h"
#include "SSTLdef/STLdef.h"

#if defined(LINUX_OS) || defined(MAC_OS_MWE)
typedef long UINT4;
#elif defined(WIN32)
#include "windows.h"
typedef DWORD UINT4;
#else
#error "not implemented"
#endif


typedef unsigned char*  MD5_POINTER;


/* MD5 context. */
typedef struct
{
  UINT4 state[4];             /* state (ABCD) */
  UINT4 count[2];             /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];   /* input buffer */
} MD5_CTX;

class XPUBMODEL_EXPORTIMPORT CMD5
{
public:
  CMD5();
  ~CMD5();

  void Reset();

  void NextBufferForMD5(unsigned char* input, unsigned int inputLen);
  xtstring GetMD5Sum();

  bool NextFileForMD5(const xtstring& sFileName);

protected:
  void MD5Init();

  void MD5_memcpy(MD5_POINTER output, MD5_POINTER input, unsigned int len);
  void MD5_memset(MD5_POINTER output, int value, unsigned int len);
  void Encode(unsigned char *output, UINT4 *input, unsigned int len);
  void Decode(UINT4 *output, unsigned char *input, unsigned int len);

  void MD5Update(unsigned char* input, unsigned int inputLen);
  void MD5Transform(UINT4 state[4], unsigned char block[64]);
  void MD5Final(unsigned char digest[16]);
private:
  MD5_CTX m_md5ctx;
};


#endif // !defined(_MD5_H_)
