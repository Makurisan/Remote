

#if defined(LINUX_OS)

#define XPUBCNVFTAG_EXPORTIMPORT

#elif defined(WIN32)

#if defined(_LIB) || defined(XPUB_STATIC)
#define XPUBCNVFTAG_EXPORTIMPORT
#else // defined(_LIB) || defined(XPUB_STATIC)
#ifdef EXPORT_XPUBCNVFTAG
#define XPUBCNVFTAG_EXPORTIMPORT __declspec(dllexport)
#else
#define XPUBCNVFTAG_EXPORTIMPORT __declspec(dllimport)
#endif
#endif // defined(_LIB) || defined(XPUB_STATIC)

#elif defined(MAC_OS_MWE)

#define XPUBCNVFTAG_EXPORTIMPORT

#ifdef EXPORT_XPUBCNVFTAG
#pragma export on
#endif

#else

#error "not implemented"

#endif

