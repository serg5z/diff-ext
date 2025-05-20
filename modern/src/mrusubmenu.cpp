#include "settings.h"
#include "mruenumerator.h"
#include "mrusubmenu.h"


MRUSubmenu::MRUSubmenu() : BaseCommand(L"Compare to", L"List of previously remembered files") {  
}

IFACEMETHODIMP 
MRUSubmenu::GetFlags(EXPCMDFLAGS* pFlags) { *pFlags = ECF_HASSUBCOMMANDS; return S_OK; }

IFACEMETHODIMP 
MRUSubmenu::EnumSubCommands(IEnumExplorerCommand** ppenum) {
  auto enumerator = Make<MRUEnumerator>();
  
  if (!enumerator) 
    return E_OUTOFMEMORY;

  *ppenum = enumerator.Detach();

  return S_OK;
}

IFACEMETHODIMP 
MRUSubmenu::Invoke(IShellItemArray*, IBindCtx*) { 
  return S_OK; 
}

IFACEMETHODIMP 
MRUSubmenu::GetState(IShellItemArray* psiItemArray, BOOL, EXPCMDSTATE* pCmdState) {
  DWORD count;
  
  psiItemArray->GetCount(&count);
  *pCmdState = (count == 1 && !getMRU().empty()) ? ECS_ENABLED : ECS_HIDDEN;

  return S_OK;
}
