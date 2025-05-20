#pragma once

#include "basecommand.h"


class MRUSubmenu : public BaseCommand {
  public:
    MRUSubmenu();
    IFACEMETHODIMP GetFlags(EXPCMDFLAGS*) override;
    IFACEMETHODIMP EnumSubCommands(IEnumExplorerCommand**) override;
    IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
    IFACEMETHODIMP GetState(IShellItemArray*, BOOL, EXPCMDSTATE*) override;
};
