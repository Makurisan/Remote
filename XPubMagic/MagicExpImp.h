

#if defined(LINUX_OS)

#define XPUBMAGIC_EXPORTIMPORT

#elif defined(WIN32)

#if defined(_LIB) || defined(XPUB_STATIC)
#define XPUBMAGIC_EXPORTIMPORT
#else // defined(_LIB) || defined(XPUB_STATIC)
#ifdef EXPORT_XPUBMAGIC
#define XPUBMAGIC_EXPORTIMPORT __declspec(dllexport)
#else
#define XPUBMAGIC_EXPORTIMPORT __declspec(dllimport)
#endif
#endif // defined(_LIB) || defined(XPUB_STATIC)

#elif defined(MAC_OS_MWE)

#define XPUBMAGIC_EXPORTIMPORT

#ifdef EXPORT_XPUBMAGIC
#pragma export on
#endif

#else

#error "not implemented"

#endif

