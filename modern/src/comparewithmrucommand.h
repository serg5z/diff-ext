#pragma once

#include "basecommand.h"


class CompareWithMRUCommand : public BaseCommand {
  public:
    CompareWithMRUCommand(const std::wstring& title, ULONG index);
    IFACEMETHODIMP GetIcon(IShellItemArray*, LPWSTR* icon) override;
    IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
    IFACEMETHODIMP GetState(IShellItemArray*, BOOL, EXPCMDSTATE*) override;
    ULONG getIndex() const;

  private:
    ULONG _index;
};
