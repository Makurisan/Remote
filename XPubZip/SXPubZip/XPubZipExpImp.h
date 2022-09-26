

#if defined(LINUX_OS)

#define XPUBZIP_EXPORTIMPORT

#elif defined(WIN32)

#if defined(_LIB) || defined(XPUB_STATIC)
#define XPUBZIP_EXPORTIMPORT
#else // defined(_LIB) || defined(XPUB_STATIC)
#ifdef EXPORT_XPUBZIP
#define XPUBZIP_EXPORTIMPORT __declspec(dllexport)
#else
#define XPUBZIP_EXPORTIMPORT __declspec(dllimport)
#endif
#endif // defined(_LIB) || defined(XPUB_STATIC)

#elif defined(MAC_OS_MWE)

#define XPUBZIP_EXPORTIMPORT

#ifdef EXPORT_XPUBZIP
#pragma export on 
#endif

#else

#error "not implemented"

#endif


