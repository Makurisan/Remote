

#if defined(LINUX_OS) || defined(__UNIX__) || defined(__LINUX__)

#define XPUBTAGBUILDER_EXPORTIMPORT

#elif defined(WIN32)

#ifdef EXPORT_XPUBTAGBUILDER
#define XPUBTAGBUILDER_EXPORTIMPORT __declspec(dllexport)
#else
#define XPUBTAGBUILDER_EXPORTIMPORT __declspec(dllimport)
#endif

#elif defined(MAC_OS_MWE)

#define XPUBTAGBUILDER_EXPORTIMPORT

#ifdef EXPORT_XPUBTAGBUILDER
#pragma export on 
#endif


#else

#error "not implemented"

#endif

