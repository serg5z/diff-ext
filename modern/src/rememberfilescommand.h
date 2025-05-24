#pragma once

#include "basecommand.h"


class __declspec(uuid("C42D8357-32CB-11F0-8E44-FAE5B572B91D"))
RememberFilesCommand : public BaseCommand {
public:
  RememberFilesCommand();
  IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
};
