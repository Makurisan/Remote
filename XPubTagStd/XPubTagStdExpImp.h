

#if defined(LINUX_OS) || defined(__UNIX__) || defined(__LINUX__)

#define XPUBTAGSTD_EXPORTIMPORT

#elif defined(WIN32)

#ifdef EXPORT_XPUBTAGSTD
#define XPUBTAGSTD_EXPORTIMPORT __declspec(dllexport)
#else
#define XPUBTAGSTD_EXPORTIMPORT __declspec(dllimport)
#endif

#elif defined(MAC_OS_MWE)

#define XPUBTAGSTD_EXPORTIMPORT

#ifdef EXPORT_XPUBTAGSTD
#pragma export on 
#endif


#else

#error "not implemented"

#endif

