#include <shlwapi.h>
#include <strsafe.h>

#include "contextmenu.h"
#include "contextmenufactory.h"

IFACEMETHODIMP ContextMenuFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) {
  if (pUnkOuter != nullptr)
    return CLASS_E_NOAGGREGATION;

  OutputDebugStringW(L"ContextMenuFactory::CreateInstance called\n");

  ComPtr<ContextMenu> contextMenu = Make<ContextMenu>();
  return contextMenu.CopyTo(riid, ppvObject);
}

IFACEMETHODIMP ContextMenuFactory::LockServer(BOOL) {
  return S_OK;
}

