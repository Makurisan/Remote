#define _CRTDBG_MAP_ALLOC
#include <windows.h>

// Wegen XPubMagic.vcproj und XPubMagicLib.vcproj
// muessen wir ein bisschen zaubern
#if !defined(_LIB)
#define _LIB
#define _LIB_INTERNAL_DEFINED
#endif // !defined(_LIB)
#include <Magick++.h>
#if defined(_LIB_INTERNAL_DEFINED)
#undef _LIB
#endif // defined(_LIB_INTERNAL_DEFINED)


#include "XPicture.h"

using namespace Magick;


void XPubMagicInit(HANDLE hModule, char* pModuleName/*=0*/)
{
#define ENV_VAR_MAX 32767 /* environment variable max is 32,767 bytes */
#define DLL_PATH_MAX 1024

  char dll_path[DLL_PATH_MAX];
  char current_path[ENV_VAR_MAX];
  static char client_filename[DLL_PATH_MAX];

  if (pModuleName)
    strcpy(client_filename, pModuleName);
  else
    strcpy(client_filename, "XPubMagic.dll");

  long count;

  count = GetModuleFileName((HMODULE)hModule, dll_path, DLL_PATH_MAX);
  if (count)
  {
    for ( ; count > 0 ; --count)
    {
      if (dll_path[count]=='\\')
      {
        dll_path[count+1]='\0';
        break;
      }
    }
  }
  MagickLib::SetClientFilename(client_filename);
  ::InitializeMagick(dll_path);

  count = GetEnvironmentVariable("PATH", current_path, ENV_VAR_MAX);
  if (strstr(current_path, dll_path) == NULL)
  {
    if (strlen(dll_path) + count + 1 < ENV_VAR_MAX - 1)
    {
      char new_path[ENV_VAR_MAX];

      sprintf(new_path, "%.*s;%.*s", DLL_PATH_MAX, dll_path,
              ENV_VAR_MAX - DLL_PATH_MAX - 1, current_path);
      SetEnvironmentVariable("PATH", new_path);
    }
  }
}

void XPubMagicRelease()
{
  MagickLib::DestroyMagick();
}
