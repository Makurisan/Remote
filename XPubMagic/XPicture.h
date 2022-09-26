#if !defined(_XPICTURE_H_)
#define _XPICTURE_H_


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

#define IMAGE_XPICTURE				_XT("XPubPicture")	// Python Importname

#if !defined(MAC_OS_MWE)
 typedef std::list<Magick::Image> ImageList;
#endif

typedef std::list<xtstring> ImageStringList;


class XPUBMAGIC_EXPORTIMPORT CXPubPicture
{
public:
	CXPubPicture(void);
	~CXPubPicture(void);

public:
	bool BuildThumbnailSmall(const xtstring& sFile, int nSize, int nBorderWidth, unsigned long clrBorder);
	bool BuildSmallFileName(const xtstring& sInFile, xtstring& sOutFile);
	bool Rotate(const xtstring& sFileName, int nDegree);
	bool Resize(const xtstring& sInFileName, const xtstring& sOutFileName, int nPercent, int nXRes=96, int nYRes=96);
	bool Resize(const xtstring& sInFileName, const xtstring& sOutFileName, int nWidth, int nHeight, int nXRes=96, int nYRes=96);
	bool Copy(const xtstring& sInFile, xtstring& sOutFile, xtstring& sPictureType, int nSize=0);
	bool Album(const xtstring& sFileName, int nSize=0);
	bool GetImageList(const xtstring& sFilePath, ImageStringList& sImageList);

protected:	// Python

/*
	static PyObject* Settings(PyObject *self, PyObject *args);
	static PyMethodDef _XPubPicture[];
*/

	static char src[255];

protected:

#if !defined(MAC_OS_MWE)
  bool GetImageList(const xtstring& sFilePath, ImageList& sImageList);
#endif

};

#endif // _XPICTURE_H_
