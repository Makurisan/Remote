#if !defined(NO_PYTHON)

#include "SSTLdef/STLdef.h"
#include "Python.h"
#include "HtmlStoneImage.h"


char CHtmlStoneImage::src[]={0};
char CHtmlStoneImage::width[]={0};
char CHtmlStoneImage::height[]={0};
char CHtmlStoneImage::align[]={0};
char CHtmlStoneImage::name[]={0};
char CHtmlStoneImage::alt[]={0};
char CHtmlStoneImage::border[]={0};
char CHtmlStoneImage::longdesc[]={0};
char CHtmlStoneImage::hspace[]={0};
char CHtmlStoneImage::vspace[]={0};
char CHtmlStoneImage::usemap[]={0};

PyMethodDef CHtmlStoneImage::_XPubHTMLStoneImage[] = {
	{"src", CHtmlStoneImage::Settings, METH_VARARGS, NULL},
	{"width", CHtmlStoneImage::Settings, METH_VARARGS, NULL},
	{"height", CHtmlStoneImage::Settings, METH_VARARGS, NULL},
	{"align", CHtmlStoneImage::Settings, METH_VARARGS, NULL},
	{"name", CHtmlStoneImage::Settings, METH_VARARGS, NULL},
	{"alt", CHtmlStoneImage::Settings, METH_VARARGS, NULL},
	{"border", CHtmlStoneImage::Settings, METH_VARARGS, NULL},
	{"longdesc", CHtmlStoneImage::Settings, METH_VARARGS, NULL},
	{"hspace", CHtmlStoneImage::Settings, METH_VARARGS, NULL},
	{"vspace", CHtmlStoneImage::Settings, METH_VARARGS, NULL},
	{"usemap", CHtmlStoneImage::Settings, METH_VARARGS, NULL},	 
	
	{NULL, NULL, 0, NULL}
};

CHtmlStoneImage::CHtmlStoneImage(void)
{
	Clear();
}

CHtmlStoneImage::~CHtmlStoneImage(void)
{
}

CHtmlStoneImage& CHtmlStoneImage::Clear(void)
{

	xtstring cEmpty(_XT(""));	
	SetSrc(cEmpty)
	.SetWidth(cEmpty).SetHeight(cEmpty ) .SetAlign(cEmpty)
	.SetName(cEmpty).SetAlt(cEmpty).SetBorder(cEmpty)
	.SetLongdesc(cEmpty).SetHSpace(cEmpty).SetVSpace(cEmpty)
	.SetUsemap(cEmpty);
	
	return *this;
}

PyObject* CHtmlStoneImage::Settings(PyObject *self, PyObject *args)
{
	if(PyArg_ParseTuple(args, ":src")) return Py_BuildValue("s", src);
	if(PyArg_ParseTuple(args, ":width")) return Py_BuildValue("s", width);
	if(PyArg_ParseTuple(args, ":height")) return Py_BuildValue("s", height);
	if(PyArg_ParseTuple(args, ":align")) return Py_BuildValue("s", align);
	if(PyArg_ParseTuple(args, ":name")) return Py_BuildValue("s", name);
	if(PyArg_ParseTuple(args, ":alt")) return Py_BuildValue("s", alt);
	if(PyArg_ParseTuple(args, ":border")) return Py_BuildValue("s", border);
	if(PyArg_ParseTuple(args, ":longdesc")) return Py_BuildValue("s", longdesc);
	if(PyArg_ParseTuple(args, ":hspace")) return Py_BuildValue("s", hspace);
	if(PyArg_ParseTuple(args, ":vspace")) return Py_BuildValue("s", vspace);
	if(PyArg_ParseTuple(args, ":usemap")) return Py_BuildValue("s", usemap);

	return NULL;
}

#endif // NO_PYTHON