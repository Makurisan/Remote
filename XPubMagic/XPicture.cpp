//

#if defined(WIN32)
#include <windows.h>
#endif // WIN32

#include <string>
#include <list>
#include <vector>
#include <iostream>

#include "XPicture.h"

using namespace std; 

#if !defined(MAC_OS_MWE)
using namespace Magick;
#endif

/*
char CXPubPicture::src[]={0};

PyMethodDef CXPubPicture::_XPubPicture[] = {
	{"src", CXPubPicture::Settings, METH_VARARGS, NULL},
	{NULL, NULL, 0, NULL}
};
*/

CXPubPicture::CXPubPicture()
{

}

CXPubPicture::~CXPubPicture()
{
}

/*
PyObject* CXPubPicture::Settings(PyObject *self, PyObject *args)
{
	if(PyArg_ParseTuple(args, ":src")) return Py_BuildValue("s", src);

	return NULL;
}
*/

#if !defined(MAC_OS_MWE)

bool CXPubPicture::GetImageList(const xtstring& sFilePath, ImageList& sImageList)
{
#if defined(WIN32)
	xtstring input_file;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( sFilePath.c_str(), drive, dir, fname, ext );

	xtstring input_path;
	input_path = drive;
	input_path += dir;

	WIN32_FIND_DATA topdata;
	HANDLE tophandle = FindFirstFile(sFilePath.c_str(), &topdata);
	do
	{
		if (tophandle == INVALID_HANDLE_VALUE)
			break;

		xtstring sCmpFileName(topdata.cFileName);
		sCmpFileName.MakeLower();

		if (sCmpFileName == ".")
			continue;
		if (sCmpFileName == "..")
			continue;

		_splitpath( topdata.cFileName, drive, dir, fname, ext );

		bool bGrafik = false;

  // check for _small file
		xtstring sfname(fname);
		sfname.MakeLower();
		size_t LeftPos = sfname.find("_small");
		if (LeftPos != -1)
			continue;
			
		if (stricmp(ext, ".jpg") == 0)
			bGrafik = true;
		if (stricmp(ext, ".gif") == 0)
			bGrafik = true;
		if (stricmp(ext, ".tif") == 0)
			bGrafik = true;
		if (stricmp(ext, ".gif") == 0)
			bGrafik = true;
		if (stricmp(ext, ".png") == 0)
			bGrafik = true;
		if (stricmp(ext, ".psd") == 0)
			bGrafik = true;
		if (stricmp(ext, ".pcx") == 0)
			bGrafik = true;

		/* geht nicht!
		if (stricmp(ext, ".tga") == 0)
		bGrafik = true;
		*/
 
		if(bGrafik == true)
		{
			input_file = input_path;
			input_file += topdata.cFileName;

			Image imageIn( input_file.c_str() );
			sImageList.insert(sImageList.end(), imageIn);
		}

	} while (FindNextFile(tophandle, &topdata));
	FindClose(tophandle);
#endif // WIN32
  return true;
}

#endif // MAC_OS_MWE

bool CXPubPicture::GetImageList(const xtstring& sFilePath, ImageStringList& sImageList)
{
#if defined(WIN32)
  xtstring input_file;

  char drive[_MAX_DRIVE];
  char dir[_MAX_DIR];
  char fname[_MAX_FNAME];
  char ext[_MAX_EXT];

  _splitpath( sFilePath.c_str(), drive, dir, fname, ext );

  xtstring input_path;
  input_path = drive;
  input_path += dir;

  WIN32_FIND_DATA topdata;
  HANDLE tophandle = FindFirstFile(sFilePath.c_str(), &topdata);
  do
  {
    if (tophandle == INVALID_HANDLE_VALUE)
      break;

    xtstring sCmpFileName(topdata.cFileName);
    sCmpFileName.MakeLower();

    if (sCmpFileName == ".")
      continue;
    if (sCmpFileName == "..")
      continue;

    _splitpath( topdata.cFileName, drive, dir, fname, ext );

    bool bGrafik = false;

    // check for _small file
    xtstring sfname(fname);
    sfname.MakeLower();
    size_t LeftPos = sfname.find("_small");
    if (LeftPos != -1)
      continue;

    if (stricmp(ext, ".jpg") == 0)
      bGrafik = true;
    if (stricmp(ext, ".gif") == 0)
      bGrafik = true;
    if (stricmp(ext, ".tif") == 0)
      bGrafik = true;
    if (stricmp(ext, ".gif") == 0)
      bGrafik = true;
    if (stricmp(ext, ".png") == 0)
      bGrafik = true;
    if (stricmp(ext, ".psd") == 0)
      bGrafik = true;
    if (stricmp(ext, ".pcx") == 0)
      bGrafik = true;

    if(bGrafik == true)
    {
      input_file = input_path;
      input_file += topdata.cFileName;
      sImageList.insert(sImageList.end(), input_file);
    }

  } while (FindNextFile(tophandle, &topdata));
  FindClose(tophandle);
#endif // WIN32
  return true;

}


bool CXPubPicture::BuildSmallFileName(const xtstring& sInFile, xtstring& sOutFile)
{
#if defined(WIN32)
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( sInFile.c_str(), drive, dir, fname, ext );

	sOutFile = drive;
	sOutFile += dir;
	sOutFile += fname;
	sOutFile += "_small";
	sOutFile += ext;
#endif // WIN32
  return true;
}

bool CXPubPicture::Album(const xtstring& sFileName, int nSize/*=0*/)
{
#if !defined(MAC_OS_MWE)

	try
	{ 

		list<Image> imageList;
		GetImageList(sFileName, imageList);

		if(imageList.size() > 0)
		{
			vector<Image> montage;
			MontageFramed montageOpts;

			montage.clear();
			montageOpts.borderColor( "green" );
			montageOpts.borderWidth( 1 );
			montageOpts.compose( OverCompositeOp );
			montageOpts.fileName( "Montage" );

			//montageOpts.frameGeometry( "6x6+3+3" );
			//montageOpts.geometry("150x150+2+2>");

			montageOpts.gravity( CenterGravity );
			//montageOpts.penColor( "yellow" );
			
			//montageOpts.shadow( true );
			//montageOpts.texture( "granite:" );
			montageOpts.tile("4x10");

			// Default montage
			montageImages( &montage, imageList.begin(), imageList.end(), montageOpts );
		
			montage[0].write("c:\\temp\\g.gif");
		
		}

	} 
	catch ( exception &e )
	{
		e.what();
		// FIXME: need a way to report warnings in constructor
	}
#endif
	return true;

}

bool CXPubPicture::Copy(const xtstring& sInFile, xtstring& sOutPathFileName, xtstring& sPictureType, int nSize/*=0*/)
{
  bool bRetf = false;
#if defined(WIN32)
	ImageStringList imageList;
	GetImageList(sInFile, imageList);

	ImageStringList::iterator entry = imageList.begin();
	while( entry != imageList.end() )
	{
    try
    {
      Image image( entry->c_str() );
	    if(nSize != 0)
    	  image.scale(Geometry(nSize, nSize));

			xtstring sBaseFilename(entry->c_str());
			xtstring sOutFileName(sOutPathFileName);
  		
			if(sBaseFilename.size() > 0)
			{
				char drive[_MAX_DRIVE];
				char dir[_MAX_DIR];
				char fname[_MAX_FNAME];
				char ext[_MAX_EXT];

				_splitpath( sBaseFilename.c_str(), drive, dir, fname, ext );

				if(sOutFileName[sOutFileName.size()] != '\\' && sOutFileName[sOutFileName.size()] != '/' )
					sOutFileName += "\\";	

				sOutFileName += fname;
				sOutFileName += ".";
				sOutFileName += sPictureType;

 				image.write((string&)sOutFileName);
			}
    }	
	  catch ( exception &e )
	  {
		  e.what();
	  }
    bRetf = true;
    entry++;	
	}
#endif // WIN32
  return bRetf;
}

bool CXPubPicture::Rotate(const xtstring& sFileName, int nDegree)
{

	bool bRetf = false;

#if !defined(MAC_OS_MWE)

	try
	{
		Image image( (string&)sFileName );
		image.rotate(nDegree);
		image.write((string&)sFileName);
		bRetf = true;
	}
	catch ( exception &e )
	{
		e.what();
		// FIXME: need a way to report warnings in constructor
	}

#endif

	return bRetf;

}

bool CXPubPicture::Resize(const xtstring& sInFileName, const xtstring& sOutFileName,
													int nWidth, int nHeight, int nXRes/*=96*/, int nYRes/*=96*/)
{

	bool bRetf = false;

#if !defined(MAC_OS_MWE)

	try
	{
		Image image( (string&)sInFileName );
		image.resolutionUnits(PixelsPerInchResolution); 

		Geometry cGeometry;

		if(nWidth != -1)
			cGeometry.width(nWidth);
		
		if(nHeight != -1)
			cGeometry.height(nHeight);

		image.scale(cGeometry);
		image.density(Geometry(nXRes,nYRes)); // display resolution  
		image.write((string&)sOutFileName);

		bRetf = true;

	}
	catch ( exception &e )
	{
		e.what();
	}
#endif
	return bRetf;

}



bool CXPubPicture::Resize(const xtstring& sInFileName, const xtstring& sOutFileName, int nPercent,
													int nXRes, int nYRes)
{

	bool bRetf = false;

#if !defined(MAC_OS_MWE)

	try
	{
		Image image( (string&)sInFileName );
		image.resolutionUnits(PixelsPerInchResolution); 

		Geometry cGeometry;

		cGeometry.percent(true);
		cGeometry.width(nPercent);
		cGeometry.height(nPercent);
		
		image.scale(cGeometry);
		image.density(Geometry(nXRes,nYRes)); // display resolution  
		image.write((string&)sOutFileName);

		bRetf = true;

	}
	catch ( exception &e )
	{
		e.what();
	}

#endif

	return bRetf;

}

// This function builds a thumbnail with the name
// "g2[_small].gif" if pFile is "g2.gif"

bool CXPubPicture::BuildThumbnailSmall(const xtstring& sFile, int nSize, int nBorderWidth, unsigned long clrBorder)
{
  bool bRetf = false;
#if defined(WIN32)
  ImageStringList imageList;
  GetImageList(sFile, imageList);

  ImageStringList::iterator entry = imageList.begin();
  while( entry != imageList.end() )
  {
    try
    {
      Image image( entry->c_str() );

			image.resolutionUnits(PixelsPerInchResolution); 
			image.scale(Geometry(nSize,nSize));
			image.density(Geometry(96,96)); // display resolution  

			image.border(Geometry(nBorderWidth,nBorderWidth));
			image.borderColor(Color(GetRValue(clrBorder),GetGValue(clrBorder),GetBValue(clrBorder)));

			xtstring sOutFile, sInFileName(entry->c_str());
			BuildSmallFileName(sInFileName, sOutFile);

			image.write((string&)sOutFile);

	  }
	  catch ( exception &e )
	  {
		  e.what();
		  // FIXME: need a way to report warnings in constructor
	  }
		entry++;
    bRetf = true;

  }
#endif // WIN32
  return bRetf;
}
