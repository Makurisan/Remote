

#if defined(LINUX_OS)

#define XPUBCNVTPDF_EXPORTIMPORT

#elif defined(WIN32)

#if defined(_LIB) || defined(XPUB_STATIC)
#define XPUBCNVTPDF_EXPORTIMPORT
#else // defined(_LIB) || defined(XPUB_STATIC)
#ifdef EXPORT_XPUBCNVTPDF
#define XPUBCNVTPDF_EXPORTIMPORT __declspec(dllexport)
#else
#define XPUBCNVTPDF_EXPORTIMPORT __declspec(dllimport)
#endif
#endif // defined(_LIB) || defined(XPUB_STATIC)

#elif defined(MAC_OS_MWE)


#define XPUBCNVTPDF_EXPORTIMPORT

#if  defined(EXPORT_XPUBCNVTPDF)
#pragma export on 
#endif

#else

#error "not implemented"

#endif



