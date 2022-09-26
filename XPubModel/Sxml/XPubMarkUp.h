#if !defined(_XPUBMARKUP_H_)
#define _XPUBMARKUP_H_


#include "SModelInd/ModelExpImp.h" // MIK

#include "MarkupSTL.h"
#include "../SSTLdef/STLdef.h"



class XPUBMODEL_EXPORTIMPORT CXPubMarkUp : protected CMarkupSTL
{
public:
  static void SetGlobalCodePage(unsigned int nCodePage){m_nCodePage = nCodePage;};


  CXPubMarkUp();
  virtual ~CXPubMarkUp();

  bool Load( const char* szFileName );
  bool SetDoc( const char* szDoc );
  const std::string& GetDoc() const {return CMarkupSTL::GetDoc();};
  bool IsWellFormed(){return CMarkupSTL::IsWellFormed();};

  std::string GetElemPath() const {return CMarkupSTL::GetElemPath();};

  bool SavePos( const char* szPosName="" ){return CMarkupSTL::SavePos(szPosName);};
  bool RestorePos( const char* szPosName="" ){return CMarkupSTL::RestorePos(szPosName);};

  void ResetChildPos(){CMarkupSTL::ResetChildPos();};
  void ResetMainPos(){CMarkupSTL::ResetMainPos();};
  void ResetPos(){CMarkupSTL::ResetPos();};

  bool FindElem( const char* szName=NULL ){return CMarkupSTL::FindElem(szName);};
  bool FindChildElem( const char* szName=NULL ){return CMarkupSTL::FindChildElem(szName);};

  std::string GetAttribName( int n ) const {return CMarkupSTL::GetAttribName(n);};
  std::string GetTagName() const {return CMarkupSTL::GetTagName();};

  bool IntoElem(){return CMarkupSTL::IntoElem();};
  bool OutOfElem(){return CMarkupSTL::OutOfElem();};

  static std::string EncodeBase64( const unsigned char* pBuffer, int nBufferLen ){return CMarkupSTL::EncodeBase64(pBuffer, nBufferLen);};



	bool AddChildElem( const char* szName, const char* szData=NULL, int nFlags=0 );
	bool AddChildElem( const char* szName, int nValue, int nFlags=0 );

  bool SetAttrib( const char* szAttrib, const char* szValue );
	bool SetAttrib( const char* szAttrib, int nValue );

	bool AddAttrib( const char* szAttrib, const char* szValue );
	bool AddAttrib( const char* szAttrib, int nValue );

  std::string GetAttrib( const char* szAttrib ) const;
  std::string GetAttrib_UTF8( const char* szAttrib ) const;
  long GetAttribLong( const char* szAttrib ) const;

  std::string GetData() const;
  std::string GetData_UTF8() const;
  bool SetData( const char* szData, int nFlags=0 );
  bool SetData_UTF8( const char* szData, int nFlags=0 );

protected:
  static unsigned int  m_nCodePage; // !!!!!
  bool          m_bFileInUTF8;
};



#endif // !defined(_XPUBMARKUP_H_)
