#include "XPubModelLibInit.h"
#include "STools/GlobalSettings.h"


CGS_DateTimeNumber* gpGlobalSettings = 0;

void XPubModelInit()
{
  gpGlobalSettings = new CGS_DateTimeNumber(CGlobalSettings::tGSU_Creator);
}

void XPubModelTerminate()
{
  if (gpGlobalSettings)
    delete gpGlobalSettings;
}

