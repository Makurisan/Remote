

#if defined(LINUX_OS)

#define XPUBADDONMANAGER_EXPORTIMPORT

#elif defined(WIN32)

#if defined(_LIB) || defined(XPUB_STATIC)
#define XPUBADDONMANAGER_EXPORTIMPORT
#else // defined(_LIB) || defined(XPUB_STATIC)
#ifdef EXPORT_XPUBADDONMANAGER
#define XPUBADDONMANAGER_EXPORTIMPORT __declspec(dllexport)
#else
#define XPUBADDONMANAGER_EXPORTIMPORT __declspec(dllimport)
#endif
#endif // defined(_LIB) || defined(XPUB_STATIC)

#elif defined(MAC_OS_MWE)

#define XPUBADDONMANAGER_EXPORTIMPORT

#ifdef EXPORT_XPUBADDONMANAGER
 #pragma export on
#endif

#else

#error "not implemented"

#endif



