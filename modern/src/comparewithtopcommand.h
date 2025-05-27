#pragma once

#include "basecommand.h"


class __declspec(uuid("C42D8358-32CB-11F0-8E44-FAE5B572B91D"))
CompareWithTopCommand : public BaseCommand {
  public:
    CompareWithTopCommand();
    IFACEMETHODIMP GetIcon(IShellItemArray*, LPWSTR* icon) override;
    IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
    IFACEMETHODIMP GetState(IShellItemArray*, BOOL, EXPCMDSTATE*) override;
};
