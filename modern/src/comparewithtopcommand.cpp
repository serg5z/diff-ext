#include "settings.h"
#include "util.h"
#include "comparewithtopcommand.h"


CompareWithTopCommand::CompareWithTopCommand() 
  : BaseCommand(L"", L"Compare with most recently remembered file") {
  auto mru = getMRU();

  if(!mru.empty()) {
    _title = (L"Compare to " + get_shortened_display_path(mru[0])).c_str();
  }
}

IFACEMETHODIMP 
CompareWithTopCommand::Invoke(IShellItemArray* items, IBindCtx*) {
  DWORD count = 0;

  if(FAILED(items->GetCount(&count)) || count != 1)
      return E_INVALIDARG;

  auto mru = getMRU();

  if(!mru.empty()) {
    ComPtr<IShellItem> item;
    items->GetItemAt(0, &item);

    PWSTR path1 = nullptr;
    item->GetDisplayName(SIGDN_FILESYSPATH, &path1);

    launch_diff_tool(path1, mru[0]);

    CoTaskMemFree(path1);
  } else {
    return E_INVALIDARG;
  }

  return S_OK;
}

IFACEMETHODIMP 
CompareWithTopCommand::GetState(IShellItemArray* psiItemArray, BOOL, EXPCMDSTATE* pCmdState) {
  DWORD count;

  psiItemArray->GetCount(&count);
  *pCmdState = (count == 1 && !getMRU().empty()) ? ECS_ENABLED : ECS_HIDDEN;
  
  return S_OK;
}
