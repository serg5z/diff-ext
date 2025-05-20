#pragma once

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <comdef.h>
#include <wrl.h>
#include <wrl\wrappers\corewrappers.h>
#include <string>


using namespace Microsoft::WRL;

// Base command
class BaseCommand : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IExplorerCommand> {
  public:
    BaseCommand(const std::wstring& title, const std::wstring& tooltip = L"", const std::wstring& icon = L"");
    IFACEMETHODIMP GetTitle(IShellItemArray*, LPWSTR*) override;
    IFACEMETHODIMP GetIcon(IShellItemArray*, LPWSTR*) override;
    IFACEMETHODIMP GetToolTip(IShellItemArray*, LPWSTR*) override;
    IFACEMETHODIMP GetCanonicalName(GUID*) override;
    IFACEMETHODIMP GetState(IShellItemArray*, BOOL, EXPCMDSTATE*) override;
    IFACEMETHODIMP GetFlags(EXPCMDFLAGS*) override;
    IFACEMETHODIMP EnumSubCommands(IEnumExplorerCommand**) override;

  protected:
    std::wstring _title;
    std::wstring _tooltip;
    std::wstring _icon;
};
