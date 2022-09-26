

#if defined(LINUX_OS)

#define XPUBMODEL_EXPORTIMPORT

#elif defined(WIN32)

#if defined(_LIB) || defined(XPUB_STATIC)
#define XPUBMODEL_EXPORTIMPORT
#else // defined(_LIB) || defined(XPUB_STATIC)
#ifdef EXPORT_XPUBMODEL
#define XPUBMODEL_EXPORTIMPORT __declspec(dllexport)
#define XPUPMODEL_EXTERN extern
#else
#define XPUBMODEL_EXPORTIMPORT __declspec(dllimport)
#define XPUPMODEL_EXTERN 
#endif
#endif // defined(_LIB) || defined(XPUB_STATIC)

#elif defined(MAC_OS_MWE)

#define XPUBMODEL_EXPORTIMPORT

#if  defined(EXPORT_XPUBMODEL)
#pragma export on 
#endif

#else

#error "not implemented"

#endif
