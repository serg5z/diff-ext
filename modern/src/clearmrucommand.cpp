#include "settings.h"
#include "clearmrucommand.h"


ClearMRUCommand::ClearMRUCommand() : BaseCommand(L"Clear MRU", L"Clear list of remembered files", L"%SystemRoot%\\System32\\shell32.dll,32") {}

IFACEMETHODIMP 
ClearMRUCommand::Invoke(IShellItemArray*, IBindCtx*) {
  clearMRU();
  SaveSettings();

  return S_OK;
}
