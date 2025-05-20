#include "basecommand.h"


BaseCommand::BaseCommand(const std::wstring& title, const std::wstring& tooltip, const std::wstring& icon)
  : _title(title), _tooltip(tooltip), _icon(icon) {
}

IFACEMETHODIMP 
BaseCommand::GetTitle(IShellItemArray*, LPWSTR* ppszName) {
  return SHStrDupW(_title.c_str(), ppszName);
}

IFACEMETHODIMP 
BaseCommand::GetIcon(IShellItemArray*, LPWSTR* ppszIcon) {
  return SHStrDupW(_icon.c_str(), ppszIcon);
}

IFACEMETHODIMP 
BaseCommand::GetToolTip(IShellItemArray*, LPWSTR* ppszTip) {
  return SHStrDupW(_tooltip.c_str(), ppszTip);
}

IFACEMETHODIMP 
BaseCommand::GetCanonicalName(GUID* pguidCommandName) {
  *pguidCommandName = GUID_NULL;
  return S_OK;
}

IFACEMETHODIMP 
BaseCommand::GetState(IShellItemArray*, BOOL, EXPCMDSTATE* pCmdState) {
  *pCmdState = ECS_ENABLED;
  return S_OK;
}

IFACEMETHODIMP 
BaseCommand::GetFlags(EXPCMDFLAGS* pFlags) {
  *pFlags = ECF_DEFAULT;
  return S_OK;
}

IFACEMETHODIMP 
BaseCommand::EnumSubCommands(IEnumExplorerCommand**) {
  return E_NOTIMPL;
}
