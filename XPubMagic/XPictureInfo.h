#if !defined(_XPICTUREINFO_H_)
#define _XPICTUREINFO_H_


#include "MagicExpImp.h"

#if !defined(MAC_OS_MWE)

// Wegen XPubMagic.vcproj und XPubMagicLib.vcproj
// muessen wir ein bisschen zaubern
#if !defined(_LIB)
#define _LIB
#define _LIB_INTERNAL_DEFINED
#endif // !defined(_LIB)
#include <Magick++.h>
#if defined(_LIB_INTERNAL_DEFINED)
#undef _LIB
#endif // defined(_LIB_INTERNAL_DEFINED)

#endif // !defined(MAC_OS_MWE)


#include "SSTLdef/STLdef.h"


class XPUBMAGIC_EXPORTIMPORT CXPubPictureInfo
{
public:
  CXPubPictureInfo(const xtstring& sFilePath);
  ~CXPubPictureInfo();

  int baseColumns();
  int baseRows();
  double xResolution();
  double yResolution();
  int depth();
  int colorSpace();
protected:
  xtstring m_sFilePath;
  Magick::Image* m_image;
};

#endif // _XPICTUREINFO_H_
