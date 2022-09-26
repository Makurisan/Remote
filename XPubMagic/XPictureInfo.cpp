#include "XPictureInfo.h"

using namespace std; 

#if !defined(MAC_OS_MWE)
using namespace Magick;
#endif


CXPubPictureInfo::CXPubPictureInfo(const xtstring& sFilePath)
{
  m_sFilePath = sFilePath;
  m_image = new Image((string&)sFilePath);
}

CXPubPictureInfo::~CXPubPictureInfo()
{
  if (m_image)
    delete m_image;
}

int CXPubPictureInfo::baseColumns()
{
  if (!m_image)
    return 0;
  return m_image->baseColumns();
}
int CXPubPictureInfo::baseRows()
{
  if (!m_image)
    return 0;
  return m_image->baseRows();
}
double CXPubPictureInfo::xResolution()
{
  if (!m_image)
    return 0;

	if(m_image->density() == Geometry(72,72))
    return 72;

  return m_image->xResolution();
}
double CXPubPictureInfo::yResolution()
{
  if (!m_image)
    return 0;

	if(m_image->density() == Geometry(72,72))
    return 72;

	return m_image->yResolution();
}
int CXPubPictureInfo::depth()
{
  if (!m_image)
    return 0;
  return m_image->depth();
}
int CXPubPictureInfo::colorSpace()
{
  if (!m_image)
    return 0;
  return m_image->colorSpace();
}
