/*
 * $Id$
 */

#include <windows.h>

#define SHELLEXNAME    "diff_ext"

//
// Global variables
//
static UINT      g_cRefThisDll = 0;    // Reference count of this DLL.
HINSTANCE g_hmodThisDll = NULL; // Handle to this DLL itself.

void
inc_cRefThisDLL() {
  InterlockedIncrement((LPLONG)&g_cRefThisDll);
}

void
dec_cRefThisDLL() {
  InterlockedDecrement((LPLONG)&g_cRefThisDll);
}

//---------------------------------------------------------------------------
// DllCanUnloadNow
//---------------------------------------------------------------------------

STDAPI 
DllCanUnloadNow(void) {
  return (g_cRefThisDll == 0 ? S_OK : S_FALSE);
}

//---------------------------------------------------------------------------
// DllMain
//---------------------------------------------------------------------------
int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
  switch (dwReason) {
    case DLL_PROCESS_ATTACH:
      // Extension DLL one-time initialization
      g_hmodThisDll = hInstance;
      break;

    case DLL_PROCESS_DETACH:
      break;
  }

  return 1;   // ok
}

