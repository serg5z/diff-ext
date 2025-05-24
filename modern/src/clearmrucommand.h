#pragma once

#include "basecommand.h"


class ClearMRUCommand : public BaseCommand {
  public:
    ClearMRUCommand();
    IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
    IFACEMETHODIMP GetFlags(EXPCMDFLAGS*) override;
};
