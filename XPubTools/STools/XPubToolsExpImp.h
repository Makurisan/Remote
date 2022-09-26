

#if defined(LINUX_OS)

#define XPUBTOOLS_EXPORTIMPORT

#elif defined(WIN32)

#if defined(_LIB) || defined(XPUB_STATIC)
#define XPUBTOOLS_EXPORTIMPORT
#else // defined(_LIB) || defined(XPUB_STATIC)
#ifdef EXPORT_XPUBTOOLS
#define XPUBTOOLS_EXPORTIMPORT __declspec(dllexport)
#else
#define XPUBTOOLS_EXPORTIMPORT __declspec(dllimport)
#endif
#endif // defined(_LIB) || defined(XPUB_STATIC)

#elif defined(MAC_OS_MWE)

#define XPUBTOOLS_EXPORTIMPORT

#ifdef EXPORT_XPUBTOOLS
#pragma export on 
#endif

#else

#error "not implemented"

#endif


