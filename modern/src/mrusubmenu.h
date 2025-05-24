#pragma once

#include "basecommand.h"


class __declspec(uuid("C42D8359-32CB-11F0-8E44-FAE5B572B91D"))
MRUSubmenu : public BaseCommand {
  public:
    MRUSubmenu();
    IFACEMETHODIMP GetFlags(EXPCMDFLAGS*) override;
    IFACEMETHODIMP EnumSubCommands(IEnumExplorerCommand**) override;
    IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
    IFACEMETHODIMP GetState(IShellItemArray*, BOOL, EXPCMDSTATE*) override;
};
