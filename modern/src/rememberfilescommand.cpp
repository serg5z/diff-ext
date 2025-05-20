#include "settings.h"
#include "rememberfilescommand.h"


RememberFilesCommand::RememberFilesCommand() : BaseCommand(L"Remember", L"Add selected files to MRU list") {
}

IFACEMETHODIMP 
RememberFilesCommand::Invoke(IShellItemArray* psiItemArray, IBindCtx*) {
  if(!psiItemArray) return E_INVALIDARG;

  DWORD count = 0;
  
  if(FAILED(psiItemArray->GetCount(&count)) || count == 0) return E_FAIL;

  for(DWORD i = 0; i < count; ++i) {
    ComPtr<IShellItem> item;

    if(SUCCEEDED(psiItemArray->GetItemAt(i, &item))) {
      PWSTR path = nullptr;
      if(SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
        addToMRU(path);
        CoTaskMemFree(path);
      }
    }
  }
  
  SaveSettings();

  return S_OK;
}
