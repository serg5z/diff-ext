#pragma once

#include "basecommand.h"


class CompareWithTopCommand : public BaseCommand {
  public:
    CompareWithTopCommand();
    IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
    IFACEMETHODIMP GetState(IShellItemArray*, BOOL, EXPCMDSTATE*) override;
};
