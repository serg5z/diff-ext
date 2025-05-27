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

class __declspec(uuid("C42D8359-32CB-11F0-8E44-FAE5B572B91D"))
MRUSubmenu : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IExplorerCommand, IEnumExplorerCommand> {
  public:
    MRUSubmenu();
    
    IFACEMETHODIMP GetTitle(IShellItemArray*, LPWSTR* ppszName) override;
    IFACEMETHODIMP GetToolTip(IShellItemArray*, LPWSTR* ppszTip) override;
    IFACEMETHODIMP GetIcon(IShellItemArray*, LPWSTR*) override;
    IFACEMETHODIMP GetCanonicalName(GUID*) override;
    IFACEMETHODIMP GetFlags(EXPCMDFLAGS*) override;
    IFACEMETHODIMP EnumSubCommands(IEnumExplorerCommand**) override;
    IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
    IFACEMETHODIMP GetState(IShellItemArray*, BOOL, EXPCMDSTATE*) override;

    IFACEMETHODIMP Next(ULONG, IExplorerCommand**, ULONG*) override;
    IFACEMETHODIMP Skip(ULONG) override;
    IFACEMETHODIMP Reset() override;
    IFACEMETHODIMP Clone(IEnumExplorerCommand**) override;

  private:
    std::vector<ComPtr<IExplorerCommand>> _commands;
    size_t _current = 0;
};
