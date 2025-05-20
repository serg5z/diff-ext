#pragma once

#include "basecommand.h"


class RememberFilesCommand : public BaseCommand {
public:
  RememberFilesCommand();
  IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
};
