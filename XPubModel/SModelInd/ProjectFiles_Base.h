#if !defined(_PROJECTFILES_BASE_H_)
#define _PROJECTFILES_BASE_H_

#include "ModelExpImp.h"
#include "ModelDef.h"
#include "ModelInd.h"
#include "../SMD5/MD5.h"






#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32

#define XML_LAYER_START           _XT("<!-- XPublisher XML\r\n")
#define XML_LAYER_END             _XT("XPublisher XML -->\r\n")


class CProjectFilesInd;

/////////////////////////////////////////////////////////////////////////////
// CProjectFileBase
class XPUBMODEL_EXPORTIMPORT CProjectFileBase
{
  friend class CProjectFilesInd;
public:
  TProjectFileType GetTypeOfFile(){return m_nTypeOfFile;};

protected:
  CProjectFileBase(TProjectFileType nTypeOfFile);
  virtual ~CProjectFileBase();
  void operator = (CProjectFileBase* pFile);
  void GetCreatedAndChangedTime(const xtstring& sFullFileName, xtstring& sCreated, xtstring& sChanged, bool bWithSeconds);

public:
  static bool ProjectFileIsCompressed(const xtstring& sFileName);
  static bool ReadFile_Compressed(const xtstring& sCompressedFileName, const xtstring& sFileName, xtstring& sFileContent);
  static bool ReadFile_Uncompressed(const xtstring& sFileName, xtstring& sFileContent);
  static bool WriteFile(const xtstring& sFileName, const xtstring& sFileContent);

  virtual xtstring GetNameForShow();
  xtstring GetModuleName(){return m_sModuleName;};
  xtstring GetXMLFileName(){return m_sXMLFileName;};
  xtstring GetBaseFileName();
  xtstring GetBaseFolder(){return m_sBaseFolder;};

  virtual LPCXTCHAR GetFileExtension() = 0;
  virtual LPCXTCHAR GetSubfolder() = 0;

  virtual void GetCreatedAndChangedTime(xtstring& sCreated, xtstring& sChanged, bool bWithSeconds) = 0;
  //
  virtual void SetModuleName(const xtstring& sModuleName){m_sModuleName = sModuleName;};
  void SetXMLFileName(const xtstring& sFileName){m_sXMLFileName = sFileName;};
  void SetBaseFileName(const xtstring& sBaseFileName);
  void SetBaseFolder(const xtstring& sBaseFolder){m_sBaseFolder = sBaseFolder;};
  //
  void SetCallBacks(CDirectoryCheckerCallBack* pDirChecker,
                    COutMessageContainerCallBack* pOutMsgContainer);
protected:

  TProjectFileType m_nTypeOfFile;
  CDirectoryCheckerCallBack*    m_pDirChecker;
  COutMessageContainerCallBack* m_pOutMsgContainer;

public:
  xtstring GetMD5Sum_XML(){return m_sMD5Sum_XML;};
  xtstring GetMD5Sum_DATA(){return m_sMD5Sum_DATA;};
  void GetTime_XML(time_t& timeChanged){timeChanged = timeChanged_XML;};
  void GetTime_DATA(time_t& timeChanged){timeChanged = timeChanged_DATA;};
protected:
  static xtstring GetMD5Sum(const xtstring& sText);
  static void GetFileTimeChanged(const xtstring& sFullFileName, time_t& timeChanged);

  void SetMD5Sum_XML(const xtstring& sMD5Sum){m_sMD5Sum_XML = sMD5Sum;};
  void SetMD5Sum_DATA(const xtstring& sMD5Sum){m_sMD5Sum_DATA = sMD5Sum;};

  void SetTime_XML(time_t timeChanged){timeChanged_XML = timeChanged;};
  void SetTime_DATA(time_t timeChanged){timeChanged_DATA = timeChanged;};

protected:
		// error log
  xtstring m_sErrorLog;

private:
  xtstring m_sMD5Sum_XML;
  xtstring m_sMD5Sum_DATA;
  time_t timeChanged_XML;
  time_t timeChanged_DATA;

  xtstring m_sXMLFileName; // diese Variable ist in XML abgespeichert
  xtstring m_sModuleName; // diese Variable muessen wir von der Datei bekommen
  xtstring m_sBaseFolder; // diese Variable ist bei Read und Write gesetzt

};


#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32

#endif // !defined(_PROJECTFILES_BASE_H_)
