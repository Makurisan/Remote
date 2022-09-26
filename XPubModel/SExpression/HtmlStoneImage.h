#if !defined(_HTMLSTONEIMAGE_H_)
#define _HTMLSTONEIMAGE_H_

#include "SModelInd/ModelExpImp.h"

#if !defined(NO_PYTHON)


#define IMAGE_IMPORTNAME		_XT("XPubHTMLStoneImage")	// Python Importname

class XPUBMODEL_EXPORTIMPORT CHtmlStoneImage
{
public:
	CHtmlStoneImage(void);
	virtual ~CHtmlStoneImage(void);

	CHtmlStoneImage& Clear();

	CHtmlStoneImage& SetSrc(xtstring& cSrc);
	CHtmlStoneImage& SetWidth(xtstring& cWidth);
	CHtmlStoneImage& SetHeight(xtstring& cHeight);
	CHtmlStoneImage& SetAlign(xtstring& cAlign);
	CHtmlStoneImage& SetName(xtstring& cName);
	CHtmlStoneImage& SetAlt(xtstring& cAlt);
	CHtmlStoneImage& SetBorder(xtstring& cBorder);
	CHtmlStoneImage& SetLongdesc(xtstring& cLongdesc);

	CHtmlStoneImage& SetHSpace(xtstring& cHSpace);
	CHtmlStoneImage& SetVSpace(xtstring& cVSpace);
	CHtmlStoneImage& SetUsemap(xtstring& cUsemap);

	static PyMethodDef* GetMethodDef();
	static char* GetImportName();

protected:

	static PyObject* Settings(PyObject *self, PyObject *args);
	static PyMethodDef _XPubHTMLStoneImage[];

protected:

	static char src[255];
	static char width[255];
	static char height[255];
	static char align[255];
	static char name[255];
	static char alt[255];
	static char border[255];
	static char longdesc[255];
	static char hspace[255];
	static char vspace[255];
	static char usemap[255];

};

inline PyMethodDef* CHtmlStoneImage::GetMethodDef()
{	return _XPubHTMLStoneImage;}

inline char* CHtmlStoneImage::GetImportName()
{	return IMAGE_IMPORTNAME;}

inline CHtmlStoneImage& CHtmlStoneImage::SetSrc(xtstring& cSrc)
{	strcpy(src, cSrc.c_str()); return *this;}

inline CHtmlStoneImage& CHtmlStoneImage::SetWidth(xtstring& cWidth)
{	strcpy(width, cWidth.c_str()); return *this;}

inline CHtmlStoneImage& CHtmlStoneImage::SetHeight(xtstring& cHeight)
{	strcpy(height, cHeight.c_str()); return *this;}

inline CHtmlStoneImage& CHtmlStoneImage::SetAlign(xtstring& cAlign)
{	strcpy(align, cAlign.c_str()); return *this;}

inline CHtmlStoneImage& CHtmlStoneImage::SetName(xtstring& cName)
{	strcpy(name, cName.c_str()); return *this;}

inline CHtmlStoneImage& CHtmlStoneImage::SetAlt(xtstring& cAlt)
{	strcpy(alt, cAlt.c_str()); return *this;}

inline CHtmlStoneImage& CHtmlStoneImage::SetBorder(xtstring& cBorder)
{	strcpy(border, cBorder.c_str()); return *this;}

inline CHtmlStoneImage& CHtmlStoneImage::SetLongdesc(xtstring& cLongdesc)
{	strcpy(longdesc, cLongdesc.c_str()); return *this;}

inline CHtmlStoneImage& CHtmlStoneImage::SetHSpace(xtstring& cHSpace)
{	strcpy(hspace, cHSpace.c_str()); return *this;}

inline CHtmlStoneImage& CHtmlStoneImage::SetVSpace(xtstring& cVSpace)
{	strcpy(vspace, cVSpace.c_str()); return *this;}

inline CHtmlStoneImage& CHtmlStoneImage::SetUsemap(xtstring& cUsemap)
{	strcpy(usemap, cUsemap.c_str()); return *this;}


#endif // NO_PYTHON

#endif // _HTMLSTONEIMAGE_H_
