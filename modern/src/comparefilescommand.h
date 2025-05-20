#pragma once

#include "basecommand.h"


class CompareFilesCommand : public BaseCommand {
  public:
    CompareFilesCommand();
    IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
    IFACEMETHODIMP GetState(IShellItemArray*, BOOL, EXPCMDSTATE*) override;
};
