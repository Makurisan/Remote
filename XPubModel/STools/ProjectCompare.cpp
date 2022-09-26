#include <time.h>
#include "ProjectCompare.h"


#define COMPARE_FILE_NOT_IDENTICAL                  _XT("file name not identical ")
#define COMPARE_MD5_NOT_IDENTICAL                   _XT("content not identical ")
#define COMPARE_MODIFIED_TIME_NOT_IDENTICAL         _XT("modified time not identical ")
#define COMPARE_FILE_NOT_IDENTICAL_DATA             _XT("DATA file name not identical ")
#define COMPARE_MD5_NOT_IDENTICAL_DATA              _XT("DATA content not identical ")
#define COMPARE_MODIFIED_TIME_NOT_IDENTICAL_DATA    _XT("DATA modified time not identical ")
#define COMPARE_IDENTICAL                           _XT("identical ")

#define COMPARE_ONLY_LOCAL_FILE                     _XT("Only local file")
#define COMPARE_ONLY_REMOTE_FILE                    _XT("Only remote file")
#define COMPARE_FATAL_ERROR                         _XT("FATAL ERROR")


void CProjectCompare_Project::SetStatus()
{
  m_nAction = tTOA_Nothing;
  m_bFileNameIdentical = true;
  m_bModulesIdentical = false;

  m_sStatus.clear();
  if (m_sLocalModuleName == m_sRemoteModuleName)
  {
    if (m_sLocalFileName != m_sRemoteFileName)
    {
      m_bFileNameIdentical = false;
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_FILE_NOT_IDENTICAL;
    }
    if (m_sLocalMD5Sum != m_sRemoteMD5Sum)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MD5_NOT_IDENTICAL;
    }
    if (m_timeLocalChanged != m_timeRemoteChanged)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MODIFIED_TIME_NOT_IDENTICAL;
    }

    if (m_sStatus.size() == 0)
    {
      m_bModulesIdentical = true;
      m_sStatus = COMPARE_IDENTICAL;
    }
  }
  else if (m_sRemoteModuleName.size() != 0 && m_sLocalModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_REMOTE_FILE;
  else if (m_sLocalModuleName.size() != 0 && m_sRemoteModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_LOCAL_FILE;
  else
    m_sStatus = COMPARE_FATAL_ERROR;

  XTCHAR buff[100];
  struct tm* ptmTemp;
  strstream str;

  str << _XT("LOCAL");
  str << _XT("\n\tmodule name: ") << m_sLocalModuleName;
  str << _XT("\n\tfile name: ") << m_sLocalFileName;
  str << _XT("\n\tmd5: ") << m_sLocalMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeLocalChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;

  str << _XT("\nREMOTE");
  str << _XT("\n\tmodule name: ") << m_sRemoteModuleName;
  str << _XT("\n\tfile name: ") << m_sRemoteFileName;
  str << _XT("\n\tmd5: ") << m_sRemoteMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeRemoteChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;

  m_sExtendedStatus.assign(str.str(), str.pcount());
}
void CProjectCompare_ER::SetStatus()
{
  m_nAction = tTOA_Nothing;
  m_bFileNameIdentical = true;
  m_bModulesIdentical = false;

  m_sStatus.clear();
  if (m_sLocalModuleName == m_sRemoteModuleName)
  {
    if (m_sLocalFileName != m_sRemoteFileName)
    {
      m_bFileNameIdentical = false;
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_FILE_NOT_IDENTICAL;
    }
    if (m_sLocalMD5Sum != m_sRemoteMD5Sum)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MD5_NOT_IDENTICAL;
    }
    if (m_timeLocalChanged != m_timeRemoteChanged)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MODIFIED_TIME_NOT_IDENTICAL;
    }

    if (m_sStatus.size() == 0)
    {
      m_bModulesIdentical = true;
      m_sStatus = COMPARE_IDENTICAL;
    }
  }
  else if (m_sRemoteModuleName.size() != 0 && m_sLocalModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_REMOTE_FILE;
  else if (m_sLocalModuleName.size() != 0 && m_sRemoteModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_LOCAL_FILE;
  else
    m_sStatus = COMPARE_FATAL_ERROR;

  XTCHAR buff[100];
  struct tm* ptmTemp;
  strstream str;

  str << _XT("LOCAL");
  str << _XT("\n\tmodule name: ") << m_sLocalModuleName;
  str << _XT("\n\tfile name: ") << m_sLocalFileName;
  str << _XT("\n\tmd5: ") << m_sLocalMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeLocalChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;

  str << _XT("\nREMOTE");
  str << _XT("\n\tmodule name: ") << m_sRemoteModuleName;
  str << _XT("\n\tfile name: ") << m_sRemoteFileName;
  str << _XT("\n\tmd5: ") << m_sRemoteMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeRemoteChanged);
      if (ptmTemp)        
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;

  m_sExtendedStatus.assign(str.str(), str.pcount());
}
void CProjectCompare_IFS::SetStatus()
{
  m_nAction = tTOA_Nothing;
  m_bFileNameIdentical = true;
  m_bModulesIdentical = false;

  m_sStatus.clear();
  if (m_sLocalModuleName == m_sRemoteModuleName)
  {
    if (m_sLocalFileName != m_sRemoteFileName)
    {
      m_bFileNameIdentical = false;
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_FILE_NOT_IDENTICAL;
    }
    if (m_sLocalMD5Sum != m_sRemoteMD5Sum)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MD5_NOT_IDENTICAL;
    }
    if (m_timeLocalChanged != m_timeRemoteChanged)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MODIFIED_TIME_NOT_IDENTICAL;
    }

    if (m_sStatus.size() == 0)
    {
      m_bModulesIdentical = true;
      m_sStatus = COMPARE_IDENTICAL;
    }
  }
  else if (m_sRemoteModuleName.size() != 0 && m_sLocalModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_REMOTE_FILE;
  else if (m_sLocalModuleName.size() != 0 && m_sRemoteModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_LOCAL_FILE;
  else
    m_sStatus = COMPARE_FATAL_ERROR;

  XTCHAR buff[100];
  struct tm* ptmTemp;
  strstream str;

  str << _XT("LOCAL");
  str << _XT("\n\tmodule name: ") << m_sLocalModuleName;
  str << _XT("\n\tfile name: ") << m_sLocalFileName;
  str << _XT("\n\tmd5: ") << m_sLocalMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeLocalChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;

  str << _XT("\nREMOTE");
  str << _XT("\n\tmodule name: ") << m_sRemoteModuleName;
  str << _XT("\n\tfile name: ") << m_sRemoteFileName;
  str << _XT("\n\tmd5: ") << m_sRemoteMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeRemoteChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;

  m_sExtendedStatus.assign(str.str(), str.pcount());
}
void CProjectCompare_Stone::SetStatus()
{
  m_nAction = tTOA_Nothing;
  m_bFileNameIdentical = true;
  m_bModulesIdentical = false;

  m_sStatus.clear();
  if (m_sLocalModuleName == m_sRemoteModuleName)
  {
    if (m_sLocalFileName != m_sRemoteFileName)
    {
      m_bFileNameIdentical = false;
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_FILE_NOT_IDENTICAL;
    }
    if (m_sLocalMD5Sum != m_sRemoteMD5Sum)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MD5_NOT_IDENTICAL;
    }
    if (m_timeLocalChanged != m_timeRemoteChanged)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MODIFIED_TIME_NOT_IDENTICAL;
    }

    if (m_sLocalFileName_Data != m_sRemoteFileName_Data)
    {
      m_bFileNameIdentical = false;
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_FILE_NOT_IDENTICAL_DATA;
    }
    if (m_sLocalMD5Sum_Data != m_sRemoteMD5Sum_Data)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MD5_NOT_IDENTICAL_DATA;
    }
    if (m_timeLocalChanged_Data != m_timeRemoteChanged_Data)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MODIFIED_TIME_NOT_IDENTICAL_DATA;
    }

    if (m_sStatus.size() == 0)
    {
      m_bModulesIdentical = true;
      m_sStatus = COMPARE_IDENTICAL;
    }
  }
  else if (m_sRemoteModuleName.size() != 0 && m_sLocalModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_REMOTE_FILE;
  else if (m_sLocalModuleName.size() != 0 && m_sRemoteModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_LOCAL_FILE;
  else
    m_sStatus = COMPARE_FATAL_ERROR;

  XTCHAR buff[100];
  struct tm* ptmTemp;
  strstream str;

  str << _XT("LOCAL");
  str << _XT("\n\tmodule name: ") << m_sLocalModuleName;
  str << _XT("\n\tfile name: ") << m_sLocalFileName;
  str << _XT("\n\tmd5: ") << m_sLocalMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeLocalChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;
  str << _XT("\n\tfile name (data): ") << m_sLocalFileName_Data;
  str << _XT("\n\tmd5 (data): ") << m_sLocalMD5Sum_Data;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeLocalChanged_Data);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged (data): ") << buff;

  str << _XT("\nREMOTE");
  str << _XT("\n\tmodule name: ") << m_sRemoteModuleName;
  str << _XT("\n\tfile name: ") << m_sRemoteFileName;
  str << _XT("\n\tmd5: ") << m_sRemoteMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeRemoteChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;
  str << _XT("\n\tfile name (data): ") << m_sRemoteFileName_Data;
  str << _XT("\n\tmd5 (data): ") << m_sRemoteMD5Sum_Data;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeRemoteChanged_Data);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged (data): ") << buff;

  m_sExtendedStatus.assign(str.str(), str.pcount());
}
void CProjectCompare_Template::SetStatus()
{
  m_nAction = tTOA_Nothing;
  m_bFileNameIdentical = true;
  m_bModulesIdentical = false;

  m_sStatus.clear();
  if (m_sLocalModuleName == m_sRemoteModuleName)
  {
    if (m_sLocalFileName != m_sRemoteFileName)
    {
      m_bFileNameIdentical = false;
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_FILE_NOT_IDENTICAL;
    }
    if (m_sLocalMD5Sum != m_sRemoteMD5Sum)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MD5_NOT_IDENTICAL;
    }
    if (m_timeLocalChanged != m_timeRemoteChanged)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MODIFIED_TIME_NOT_IDENTICAL;
    }

    if (m_sLocalFileName_Data != m_sRemoteFileName_Data)
    {
      m_bFileNameIdentical = false;
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_FILE_NOT_IDENTICAL_DATA;
    }
    if (m_sLocalMD5Sum_Data != m_sRemoteMD5Sum_Data)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MD5_NOT_IDENTICAL_DATA;
    }
    if (m_timeLocalChanged_Data != m_timeRemoteChanged_Data)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MODIFIED_TIME_NOT_IDENTICAL_DATA;
    }

    if (m_sStatus.size() == 0)
    {
      m_bModulesIdentical = true;
      m_sStatus = COMPARE_IDENTICAL;
    }
  }
  else if (m_sRemoteModuleName.size() != 0 && m_sLocalModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_REMOTE_FILE;
  else if (m_sLocalModuleName.size() != 0 && m_sRemoteModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_LOCAL_FILE;
  else
    m_sStatus = COMPARE_FATAL_ERROR;

  XTCHAR buff[100];
  struct tm* ptmTemp;
  strstream str;

  str << _XT("LOCAL");
  str << _XT("\n\tmodule name: ") << m_sLocalModuleName;
  str << _XT("\n\tfile name: ") << m_sLocalFileName;
  str << _XT("\n\tmd5: ") << m_sLocalMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeLocalChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;
  str << _XT("\n\tfile name (data): ") << m_sLocalFileName_Data;
  str << _XT("\n\tmd5 (data): ") << m_sLocalMD5Sum_Data;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeLocalChanged_Data);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged (data): ") << buff;

  str << _XT("\nREMOTE");
  str << _XT("\n\tmodule name: ") << m_sRemoteModuleName;
  str << _XT("\n\tfile name: ") << m_sRemoteFileName;
  str << _XT("\n\tmd5: ") << m_sRemoteMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeRemoteChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;
  str << _XT("\n\tfile name (data): ") << m_sRemoteFileName_Data;
  str << _XT("\n\tmd5 (data): ") << m_sRemoteMD5Sum_Data;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeRemoteChanged_Data);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged (data): ") << buff;

  m_sExtendedStatus.assign(str.str(), str.pcount());
}
void CProjectCompare_Python::SetStatus()
{
  m_nAction = tTOA_Nothing;
  m_bFileNameIdentical = true;
  m_bModulesIdentical = false;

  m_sStatus.clear();
  if (m_sLocalModuleName == m_sRemoteModuleName)
  {
    if (m_sLocalFileName != m_sRemoteFileName)
    {
      m_bFileNameIdentical = false;
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_FILE_NOT_IDENTICAL;
    }
    if (m_sLocalMD5Sum != m_sRemoteMD5Sum)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MD5_NOT_IDENTICAL;
    }
    if (m_timeLocalChanged != m_timeRemoteChanged)
    {
      if (m_sStatus.size()) m_sStatus += _XT(" / ");
      m_sStatus += COMPARE_MODIFIED_TIME_NOT_IDENTICAL;
    }

    if (m_sStatus.size() == 0)
    {
      m_bModulesIdentical = true;
      m_sStatus = COMPARE_IDENTICAL;
    }
  }
  else if (m_sRemoteModuleName.size() != 0 && m_sLocalModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_REMOTE_FILE;
  else if (m_sLocalModuleName.size() != 0 && m_sRemoteModuleName.size() == 0)
    m_sStatus = COMPARE_ONLY_LOCAL_FILE;
  else
    m_sStatus = COMPARE_FATAL_ERROR;

  XTCHAR buff[100];
  struct tm* ptmTemp;
  strstream str;

  str << _XT("LOCAL");
  str << _XT("\n\tmodule name: ") << m_sLocalModuleName;
  str << _XT("\n\tfile name: ") << m_sLocalFileName;
  str << _XT("\n\tmd5: ") << m_sLocalMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeLocalChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;

  str << _XT("\nREMOTE");
  str << _XT("\n\tmodule name: ") << m_sRemoteModuleName;
  str << _XT("\n\tfile name: ") << m_sRemoteFileName;
  str << _XT("\n\tmd5: ") << m_sRemoteMD5Sum;
      buff[0] = 0;
      ptmTemp = localtime(&m_timeRemoteChanged);
      if (ptmTemp)
        strftime(buff, sizeof(buff), _XT("%d.%m.%Y %H:%M:%S"), ptmTemp);
  str << _XT("\n\tchanged: ") << buff;

  m_sExtendedStatus.assign(str.str(), str.pcount());
}
















CProjectCompare::CProjectCompare()
{
}

CProjectCompare::~CProjectCompare()
{
  ResetContent();
}

void CProjectCompare::ResetContent()
{
  // project
  m_cProject.m_sLocalModuleName = _XT("");
  m_cProject.m_sLocalFileName = _XT("");
  m_cProject.m_sLocalMD5Sum = _XT("");
  m_cProject.m_timeLocalChanged = 0;
  m_cProject.m_sRemoteModuleName = _XT("");
  m_cProject.m_sRemoteFileName = _XT("");
  m_cProject.m_sRemoteMD5Sum = _XT("");
  m_cProject.m_timeRemoteChanged = 0;
  // er
  for (CPC_ERsIterator it = m_cERs.begin(); it != m_cERs.end(); it++)
    delete (*it);
  m_cERs.erase(m_cERs.begin(), m_cERs.end());
  // ifs
  for (CPC_IFSsIterator it = m_cIFSs.begin(); it != m_cIFSs.end(); it++)
    delete (*it);
  m_cIFSs.erase(m_cIFSs.begin(), m_cIFSs.end());
  // stone
  for (CPC_StonesIterator it = m_cStones.begin(); it != m_cStones.end(); it++)
    delete (*it);
  m_cStones.erase(m_cStones.begin(), m_cStones.end());
  // template
  for (CPC_TemplatesIterator it = m_cTemplates.begin(); it != m_cTemplates.end(); it++)
    delete (*it);
  m_cTemplates.erase(m_cTemplates.begin(), m_cTemplates.end());
  // python
  for (CPC_PythonsIterator it = m_cPythons.begin(); it != m_cPythons.end(); it++)
    delete (*it);
  m_cPythons.erase(m_cPythons.begin(), m_cPythons.end());
}

void CProjectCompare::SetStatus()
{
  // project
  m_cProject.SetStatus();
  // er
  for (CPC_ERsIterator it = m_cERs.begin(); it != m_cERs.end(); it++)
    (*it)->SetStatus();
  // ifs
  for (CPC_IFSsIterator it = m_cIFSs.begin(); it != m_cIFSs.end(); it++)
    (*it)->SetStatus();
  // stone
  for (CPC_StonesIterator it = m_cStones.begin(); it != m_cStones.end(); it++)
    (*it)->SetStatus();
  // template
  for (CPC_TemplatesIterator it = m_cTemplates.begin(); it != m_cTemplates.end(); it++)
    (*it)->SetStatus();
  // python
  for (CPC_PythonsIterator it = m_cPythons.begin(); it != m_cPythons.end(); it++)
    (*it)->SetStatus();
}

CProjectCompare_Project* CProjectCompare::GetProject()
{
  return &m_cProject;
}
CProjectCompare_ER* CProjectCompare::GetER(size_t nIndex)
{
  if (nIndex >= m_cERs.size())
    return 0;
  return m_cERs[nIndex];
}
CProjectCompare_IFS* CProjectCompare::GetIFS(size_t nIndex)
{
  if (nIndex >= m_cIFSs.size())
    return 0;
  return m_cIFSs[nIndex];
}
CProjectCompare_Stone* CProjectCompare::GetStone(size_t nIndex)
{
  if (nIndex >= m_cStones.size())
    return 0;
  return m_cStones[nIndex];
}
CProjectCompare_Template* CProjectCompare::GetTemplate(size_t nIndex)
{
  if (nIndex >= m_cTemplates.size())
    return 0;
  return m_cTemplates[nIndex];
}
CProjectCompare_Python* CProjectCompare::GetPython(size_t nIndex)
{
  if (nIndex >= m_cPythons.size())
    return 0;
  return m_cPythons[nIndex];
}

CProjectCompare_ER* CProjectCompare::GetER(const xtstring& sModuleName)
{
  for (CPC_ERsIterator it = m_cERs.begin(); it != m_cERs.end(); it++)
  {
    if ((*it)->m_sLocalModuleName == sModuleName
        || (*it)->m_sRemoteModuleName == sModuleName)
      return (*it);
  }
  return 0;
}

CProjectCompare_IFS* CProjectCompare::GetIFS(const xtstring& sModuleName)
{
  for (CPC_IFSsIterator it = m_cIFSs.begin(); it != m_cIFSs.end(); it++)
  {
    if ((*it)->m_sLocalModuleName == sModuleName
        || (*it)->m_sRemoteModuleName == sModuleName)
      return (*it);
  }
  return 0;
}

CProjectCompare_Stone* CProjectCompare::GetStone(const xtstring& sModuleName)
{
  for (CPC_StonesIterator it = m_cStones.begin(); it != m_cStones.end(); it++)
  {
    if ((*it)->m_sLocalModuleName == sModuleName
        || (*it)->m_sRemoteModuleName == sModuleName)
      return (*it);
  }
  return 0;
}

CProjectCompare_Template* CProjectCompare::GetTemplate(const xtstring& sModuleName)
{
  for (CPC_TemplatesIterator it = m_cTemplates.begin(); it != m_cTemplates.end(); it++)
  {
    if ((*it)->m_sLocalModuleName == sModuleName
        || (*it)->m_sRemoteModuleName == sModuleName)
      return (*it);
  }
  return 0;
}

CProjectCompare_Python* CProjectCompare::GetPython(const xtstring& sModuleName)
{
  for (CPC_PythonsIterator it = m_cPythons.begin(); it != m_cPythons.end(); it++)
  {
    if ((*it)->m_sLocalModuleName == sModuleName
        || (*it)->m_sRemoteModuleName == sModuleName)
      return (*it);
  }
  return 0;
}

void CProjectCompare::SetProjectFile_Project_Local(const xtstring& sModuleName,
                                                   const xtstring& sFileName,
                                                   const xtstring& sMD5Sum,
                                                   time_t timeChanged)
{
  m_cProject.m_sLocalModuleName = sModuleName;
  m_cProject.m_sLocalFileName = sFileName;
  m_cProject.m_sLocalMD5Sum = sMD5Sum;
  m_cProject.m_timeLocalChanged = timeChanged;
}
void CProjectCompare::SetProjectFile_Project_Remote(const xtstring& sModuleName,
                                                    const xtstring& sFileName,
                                                    const xtstring& sMD5Sum,
                                                    time_t timeChanged)
{
  m_cProject.m_sRemoteModuleName = sModuleName;
  m_cProject.m_sRemoteFileName = sFileName;
  m_cProject.m_sRemoteMD5Sum = sMD5Sum;
  m_cProject.m_timeRemoteChanged = timeChanged;
}
void CProjectCompare::SetProjectFile_ER_Local(const xtstring& sModuleName,
                                              const xtstring& sFileName,
                                              const xtstring& sMD5Sum,
                                              time_t timeChanged)
{
  CProjectCompare_ER* p = GetER(sModuleName);
  if (!p)
  {
    p = new CProjectCompare_ER;
    m_cERs.push_back(p);
  }
  p->m_sLocalModuleName = sModuleName;
  p->m_sLocalFileName = sFileName;
  p->m_sLocalMD5Sum = sMD5Sum;
  p->m_timeLocalChanged = timeChanged;
}
void CProjectCompare::SetProjectFile_ER_Remote(const xtstring& sModuleName,
                                               const xtstring& sFileName,
                                               const xtstring& sMD5Sum,
                                               time_t timeChanged)
{
  CProjectCompare_ER* p = GetER(sModuleName);
  if (!p)
  {
    p = new CProjectCompare_ER;
    m_cERs.push_back(p);
  }
  p->m_sRemoteModuleName = sModuleName;
  p->m_sRemoteFileName = sFileName;
  p->m_sRemoteMD5Sum = sMD5Sum;
  p->m_timeRemoteChanged = timeChanged;
}
void CProjectCompare::SetProjectFile_IFS_Local(const xtstring& sModuleName,
                                               const xtstring& sFileName,
                                               const xtstring& sMD5Sum,
                                               time_t timeChanged)
{
  CProjectCompare_IFS* p = GetIFS(sModuleName);
  if (!p)
  {
    p = new CProjectCompare_IFS;
    m_cIFSs.push_back(p);
  }
  p->m_sLocalModuleName = sModuleName;
  p->m_sLocalFileName = sFileName;
  p->m_sLocalMD5Sum = sMD5Sum;
  p->m_timeLocalChanged = timeChanged;
}
void CProjectCompare::SetProjectFile_IFS_Remote(const xtstring& sModuleName,
                                                const xtstring& sFileName,
                                                const xtstring& sMD5Sum,
                                                time_t timeChanged)
{
  CProjectCompare_IFS* p = GetIFS(sModuleName);
  if (!p)
  {
    p = new CProjectCompare_IFS;
    m_cIFSs.push_back(p);
  }
  p->m_sRemoteModuleName = sModuleName;
  p->m_sRemoteFileName = sFileName;
  p->m_sRemoteMD5Sum = sMD5Sum;
  p->m_timeRemoteChanged = timeChanged;
}
void CProjectCompare::SetProjectFile_Stone_Local(const xtstring& sModuleName,
                                                 const xtstring& sFileName,
                                                 const xtstring& sMD5Sum,
                                                 time_t timeChanged,
                                                 const xtstring& sFileName_Data,
                                                 const xtstring& sMD5Sum_Data,
                                                 time_t timeChanged_Data)
{
  CProjectCompare_Stone* p = GetStone(sModuleName);
  if (!p)
  {
    p = new CProjectCompare_Stone;
    m_cStones.push_back(p);
  }
  p->m_sLocalModuleName = sModuleName;
  p->m_sLocalFileName = sFileName;
  p->m_sLocalMD5Sum = sMD5Sum;
  p->m_timeLocalChanged = timeChanged;
  p->m_sLocalFileName_Data = sFileName_Data;
  p->m_sLocalMD5Sum_Data = sMD5Sum_Data;
  p->m_timeLocalChanged_Data = timeChanged_Data;
}
void CProjectCompare::SetProjectFile_Stone_Remote(const xtstring& sModuleName,
                                                  const xtstring& sFileName,
                                                  const xtstring& sMD5Sum,
                                                  time_t timeChanged,
                                                  const xtstring& sFileName_Data,
                                                  const xtstring& sMD5Sum_Data,
                                                  time_t timeChanged_Data)
{
  CProjectCompare_Stone* p = GetStone(sModuleName);
  if (!p)
  {
    p = new CProjectCompare_Stone;
    m_cStones.push_back(p);
  }
  p->m_sRemoteModuleName = sModuleName;
  p->m_sRemoteFileName = sFileName;
  p->m_sRemoteMD5Sum = sMD5Sum;
  p->m_timeRemoteChanged = timeChanged;
  p->m_sRemoteFileName_Data = sFileName_Data;
  p->m_sRemoteMD5Sum_Data = sMD5Sum_Data;
  p->m_timeRemoteChanged_Data = timeChanged_Data;
}
void CProjectCompare::SetProjectFile_Template_Local(const xtstring& sModuleName,
                                                    const xtstring& sFileName,
                                                    const xtstring& sMD5Sum,
                                                    time_t timeChanged,
                                                    const xtstring& sFileName_Data,
                                                    const xtstring& sMD5Sum_Data,
                                                    time_t timeChanged_Data)
{
  CProjectCompare_Template* p = GetTemplate(sModuleName);
  if (!p)
  {
    p = new CProjectCompare_Template;
    m_cTemplates.push_back(p);
  }
  p->m_sLocalModuleName = sModuleName;
  p->m_sLocalFileName = sFileName;
  p->m_sLocalMD5Sum = sMD5Sum;
  p->m_timeLocalChanged = timeChanged;
  p->m_sLocalFileName_Data = sFileName_Data;
  p->m_sLocalMD5Sum_Data = sMD5Sum_Data;
  p->m_timeLocalChanged_Data = timeChanged_Data;
}
void CProjectCompare::SetProjectFile_Template_Remote(const xtstring& sModuleName,
                                                     const xtstring& sFileName,
                                                     const xtstring& sMD5Sum,
                                                     time_t timeChanged,
                                                     const xtstring& sFileName_Data,
                                                     const xtstring& sMD5Sum_Data,
                                                     time_t timeChanged_Data)
{
  CProjectCompare_Template* p = GetTemplate(sModuleName);
  if (!p)
  {
    p = new CProjectCompare_Template;
    m_cTemplates.push_back(p);
  }
  p->m_sRemoteModuleName = sModuleName;
  p->m_sRemoteFileName = sFileName;
  p->m_sRemoteMD5Sum = sMD5Sum;
  p->m_timeRemoteChanged = timeChanged;
  p->m_sRemoteFileName_Data = sFileName_Data;
  p->m_sRemoteMD5Sum_Data = sMD5Sum_Data;
  p->m_timeRemoteChanged_Data = timeChanged_Data;
}
void CProjectCompare::SetProjectFile_Python_Local(const xtstring& sModuleName,
                                                  const xtstring& sFileName,
                                                  const xtstring& sMD5Sum,
                                                  time_t timeChanged)
{
  CProjectCompare_Python* p = GetPython(sModuleName);
  if (!p)
  {
    p = new CProjectCompare_Python;
    m_cPythons.push_back(p);
  }
  p->m_sLocalModuleName = sModuleName;
  p->m_sLocalFileName = sFileName;
  p->m_sLocalMD5Sum = sMD5Sum;
  p->m_timeLocalChanged = timeChanged;
}
void CProjectCompare::SetProjectFile_Python_Remote(const xtstring& sModuleName,
                                                   const xtstring& sFileName,
                                                   const xtstring& sMD5Sum,
                                                   time_t timeChanged)
{
  CProjectCompare_Python* p = GetPython(sModuleName);
  if (!p)
  {
    p = new CProjectCompare_Python;
    m_cPythons.push_back(p);
  }
  p->m_sRemoteModuleName = sModuleName;
  p->m_sRemoteFileName = sFileName;
  p->m_sRemoteMD5Sum = sMD5Sum;
  p->m_timeRemoteChanged = timeChanged;
}
