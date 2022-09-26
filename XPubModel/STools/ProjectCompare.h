#if !defined(_PROJECTCOMPARE_H_)
#define _PROJECTCOMPARE_H_

#include "SModelInd/ModelExpImp.h"

#include "SSTLdef/STLdef.h"


#define TYPEOFACTION_NOTHING    _XT("nothing")
#define TYPEOFACTION_DOWNLOAD   _XT("download")
#define TYPEOFACTION_UPLOAD     _XT("upload")

class XPUBMODEL_EXPORTIMPORT CProjectCompare_Base
{
public:

  typedef enum TTypeOfAction
  {
    tTOA_Nothing = 0,
    tTOA_Download,
    tTOA_Upload,
  };

  CProjectCompare_Base(){};
  virtual ~CProjectCompare_Base(){};

  virtual xtstring GetStatus() = 0;
  virtual xtstring GetExtendedStatus() = 0;

  TTypeOfAction GetAction(){return m_nAction;};
  void SetAction(TTypeOfAction nAction){m_nAction = nAction;};

  bool GetModulesIdentical(){return m_bModulesIdentical;};
  bool GetFileNameIdentical(){return m_bFileNameIdentical;};

protected:
  TTypeOfAction m_nAction;
  bool          m_bModulesIdentical;
  bool          m_bFileNameIdentical;
};
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CProjectCompare_Project : public CProjectCompare_Base
{
public:
  CProjectCompare_Project(){};
  virtual ~CProjectCompare_Project(){};

  void SetStatus();

  virtual xtstring GetStatus(){return m_sStatus;};
  virtual xtstring GetExtendedStatus(){return m_sExtendedStatus;};

  xtstring  m_sStatus;
  xtstring  m_sExtendedStatus;

  // Local
  xtstring  m_sLocalModuleName;
  xtstring  m_sLocalFileName;
  xtstring  m_sLocalMD5Sum;
  time_t    m_timeLocalChanged;
  // Remote
  xtstring  m_sRemoteModuleName;
  xtstring  m_sRemoteFileName;
  xtstring  m_sRemoteMD5Sum;
  time_t    m_timeRemoteChanged;
};
typedef vector<CProjectCompare_Project*>  CPC_Projects;
typedef CPC_Projects::iterator            CPC_ProjectsIterator;
typedef CPC_Projects::const_iterator      CPC_ProjectsConstIterator;
typedef CPC_Projects::reverse_iterator    CPC_ProjectsReverseIterator;
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CProjectCompare_ER : public CProjectCompare_Base
{
public:
  CProjectCompare_ER(){};
  virtual ~CProjectCompare_ER(){};

  void SetStatus();

  virtual xtstring GetStatus(){return m_sStatus;};
  virtual xtstring GetExtendedStatus(){return m_sExtendedStatus;};

  xtstring  m_sStatus;
  xtstring  m_sExtendedStatus;

  // Local
  xtstring  m_sLocalModuleName;
  xtstring  m_sLocalFileName;
  xtstring  m_sLocalMD5Sum;
  time_t    m_timeLocalChanged;
  // Remote
  xtstring  m_sRemoteModuleName;
  xtstring  m_sRemoteFileName;
  xtstring  m_sRemoteMD5Sum;
  time_t    m_timeRemoteChanged;
};
typedef vector<CProjectCompare_ER*>       CPC_ERs;
typedef CPC_ERs::iterator                 CPC_ERsIterator;
typedef CPC_ERs::const_iterator           CPC_ERsConstIterator;
typedef CPC_ERs::reverse_iterator         CPC_ERsReverseIterator;
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CProjectCompare_IFS : public CProjectCompare_Base
{
public:
  CProjectCompare_IFS(){};
  virtual ~CProjectCompare_IFS(){};

  void SetStatus();

  virtual xtstring GetStatus(){return m_sStatus;};
  virtual xtstring GetExtendedStatus(){return m_sExtendedStatus;};

  xtstring  m_sStatus;
  xtstring  m_sExtendedStatus;

  // Local
  xtstring  m_sLocalModuleName;
  xtstring  m_sLocalFileName;
  xtstring  m_sLocalMD5Sum;
  time_t    m_timeLocalChanged;
  // Remote
  xtstring  m_sRemoteModuleName;
  xtstring  m_sRemoteFileName;
  xtstring  m_sRemoteMD5Sum;
  time_t    m_timeRemoteChanged;
};
typedef vector<CProjectCompare_IFS*>      CPC_IFSs;
typedef CPC_IFSs::iterator                CPC_IFSsIterator;
typedef CPC_IFSs::const_iterator          CPC_IFSsConstIterator;
typedef CPC_IFSs::reverse_iterator        CPC_IFSsReverseIterator;
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CProjectCompare_Stone : public CProjectCompare_Base
{
public:
  CProjectCompare_Stone(){};
  virtual ~CProjectCompare_Stone(){};

  void SetStatus();

  virtual xtstring GetStatus(){return m_sStatus;};
  virtual xtstring GetExtendedStatus(){return m_sExtendedStatus;};

  xtstring  m_sStatus;
  xtstring  m_sExtendedStatus;

  // Local
  xtstring  m_sLocalModuleName;
  xtstring  m_sLocalFileName;
  xtstring  m_sLocalMD5Sum;
  time_t    m_timeLocalChanged;
  xtstring  m_sLocalFileName_Data;
  xtstring  m_sLocalMD5Sum_Data;
  time_t    m_timeLocalChanged_Data;
  // Remote
  xtstring  m_sRemoteModuleName;
  xtstring  m_sRemoteFileName;
  xtstring  m_sRemoteMD5Sum;
  time_t    m_timeRemoteChanged;
  xtstring  m_sRemoteFileName_Data;
  xtstring  m_sRemoteMD5Sum_Data;
  time_t    m_timeRemoteChanged_Data;
};
typedef vector<CProjectCompare_Stone*>    CPC_Stones;
typedef CPC_Stones::iterator              CPC_StonesIterator;
typedef CPC_Stones::const_iterator        CPC_StonesConstIterator;
typedef CPC_Stones::reverse_iterator      CPC_StonesReverseIterator;
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CProjectCompare_Template : public CProjectCompare_Base
{
public:
  CProjectCompare_Template(){};
  ~CProjectCompare_Template(){};

  void SetStatus();

  virtual xtstring GetStatus(){return m_sStatus;};
  virtual xtstring GetExtendedStatus(){return m_sExtendedStatus;};

  xtstring  m_sStatus;
  xtstring  m_sExtendedStatus;

  // Local
  xtstring  m_sLocalModuleName;
  xtstring  m_sLocalFileName;
  xtstring  m_sLocalMD5Sum;
  time_t    m_timeLocalChanged;
  xtstring  m_sLocalFileName_Data;
  xtstring  m_sLocalMD5Sum_Data;
  time_t    m_timeLocalChanged_Data;
  // Remote
  xtstring  m_sRemoteModuleName;
  xtstring  m_sRemoteFileName;
  xtstring  m_sRemoteMD5Sum;
  time_t    m_timeRemoteChanged;
  xtstring  m_sRemoteFileName_Data;
  xtstring  m_sRemoteMD5Sum_Data;
  time_t    m_timeRemoteChanged_Data;
};
typedef vector<CProjectCompare_Template*> CPC_Templates;
typedef CPC_Templates::iterator           CPC_TemplatesIterator;
typedef CPC_Templates::const_iterator     CPC_TemplatesConstIterator;
typedef CPC_Templates::reverse_iterator   CPC_TemplatesReverseIterator;
////////////////////////////////////////////////////////////////////////////////
class XPUBMODEL_EXPORTIMPORT CProjectCompare_Python : public CProjectCompare_Base
{
public:
  CProjectCompare_Python(){};
  ~CProjectCompare_Python(){};

  void SetStatus();

  virtual xtstring GetStatus(){return m_sStatus;};
  virtual xtstring GetExtendedStatus(){return m_sExtendedStatus;};

  xtstring  m_sStatus;
  xtstring  m_sExtendedStatus;

  // Local
  xtstring  m_sLocalModuleName;
  xtstring  m_sLocalFileName;
  xtstring  m_sLocalMD5Sum;
  time_t    m_timeLocalChanged;
  // Remote
  xtstring  m_sRemoteModuleName;
  xtstring  m_sRemoteFileName;
  xtstring  m_sRemoteMD5Sum;
  time_t    m_timeRemoteChanged;
};
typedef vector<CProjectCompare_Python*>   CPC_Pythons;
typedef CPC_Pythons::iterator             CPC_PythonsIterator;
typedef CPC_Pythons::const_iterator       CPC_PythonsConstIterator;
typedef CPC_Pythons::reverse_iterator     CPC_PythonsReverseIterator;


class XPUBMODEL_EXPORTIMPORT CProjectCompare
{
public:
  CProjectCompare();
  virtual ~CProjectCompare();

  void ResetContent();
  void SetStatus();

  void SetProjectFile_Project_Local(const xtstring& sModuleName,
                                    const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged);
  void SetProjectFile_Project_Remote(const xtstring& sModuleName,
                                     const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged);
  void SetProjectFile_ER_Local(const xtstring& sModuleName,
                               const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged);
  void SetProjectFile_ER_Remote(const xtstring& sModuleName,
                                const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged);
  void SetProjectFile_IFS_Local(const xtstring& sModuleName,
                                const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged);
  void SetProjectFile_IFS_Remote(const xtstring& sModuleName,
                                 const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged);
  void SetProjectFile_Stone_Local(const xtstring& sModuleName,
                                  const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged,
                                  const xtstring& sFileName_Data, const xtstring& sMD5Sum_Data, time_t timeChanged_Data);
  void SetProjectFile_Stone_Remote(const xtstring& sModuleName,
                                   const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged,
                                   const xtstring& sFileName_Data, const xtstring& sMD5Sum_Data, time_t timeChanged_Data);
  void SetProjectFile_Template_Local(const xtstring& sModuleName,
                                     const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged,
                                     const xtstring& sFileName_Data, const xtstring& sMD5Sum_Data, time_t timeChanged_Data);
  void SetProjectFile_Template_Remote(const xtstring& sModuleName,
                                      const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged,
                                      const xtstring& sFileName_Data, const xtstring& sMD5Sum_Data, time_t timeChanged_Data);
  void SetProjectFile_Python_Local(const xtstring& sModuleName,
                                   const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged);
  void SetProjectFile_Python_Remote(const xtstring& sModuleName,
                                    const xtstring& sFileName, const xtstring& sMD5Sum, time_t timeChanged);

  size_t CountOfER(){return m_cERs.size();};
  size_t CountOfIFS(){return m_cIFSs.size();};
  size_t CountOfStone(){return m_cStones.size();};
  size_t CountOfTemplate(){return m_cTemplates.size();};
  size_t CountOfPython(){return m_cPythons.size();};
  CProjectCompare_Project*  GetProject();
  CProjectCompare_ER*       GetER(size_t nIndex);
  CProjectCompare_IFS*      GetIFS(size_t nIndex);
  CProjectCompare_Stone*    GetStone(size_t nIndex);
  CProjectCompare_Template* GetTemplate(size_t nIndex);
  CProjectCompare_Python*   GetPython(size_t nIndex);
protected:
  CProjectCompare_ER*       GetER(const xtstring& sModuleName);
  CProjectCompare_IFS*      GetIFS(const xtstring& sModuleName);
  CProjectCompare_Stone*    GetStone(const xtstring& sModuleName);
  CProjectCompare_Template* GetTemplate(const xtstring& sModuleName);
  CProjectCompare_Python*   GetPython(const xtstring& sModuleName);

protected:
  CProjectCompare_Project   m_cProject;

#ifdef WIN32
#pragma warning(disable : 4251) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#endif // WIN32
  CPC_ERs                   m_cERs;
  CPC_IFSs                  m_cIFSs;
  CPC_Stones                m_cStones;
  CPC_Templates             m_cTemplates;
  CPC_Pythons               m_cPythons;
#ifdef WIN32
#pragma warning(default : 4251)
#endif // WIN32
};

#endif // _PROJECTCOMPARE_H_
