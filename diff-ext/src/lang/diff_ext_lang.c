#include <windows.h>

int APIENTRY __declspec(dllexport)
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
  switch (fdwReason)
  {
    case DLL_PROCESS_ATTACH:
      break;
    case DLL_PROCESS_DETACH:
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
  }

  return 1;
}
