

#if defined(LINUX_OS) || defined(__UNIX__) || defined(__LINUX__)

#define XPUBPDF_EXPORTIMPORT

#elif defined(WIN32)

#if defined(_LIB) || defined(XPUB_STATIC)
#define XPUBPDF_EXPORTIMPORT
#else // defined(_LIB) || defined(XPUB_STATIC)
#ifdef EXPORT_XPUBPDF
#define XPUBPDF_EXPORTIMPORT __declspec(dllexport)
#else
#define XPUBPDF_EXPORTIMPORT __declspec(dllimport)
#endif
#endif // defined(_LIB) || defined(XPUB_STATIC)

#elif defined(MAC_OS_MWE)


#define XPUBPDF_EXPORTIMPORT

#if  defined(EXPORT_XPUBPDF)
#pragma export on 
#endif

#else

#error "not implemented"

#endif

