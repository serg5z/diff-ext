#pragma once

#include "basecommand.h"


class CompareWithMRUCommand : public BaseCommand {
  public:
    CompareWithMRUCommand(const std::wstring& title, ULONG index);
    IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
    IFACEMETHODIMP GetState(IShellItemArray*, BOOL, EXPCMDSTATE*) override;
    IFACEMETHODIMP GetIcon(IShellItemArray*, LPWSTR*) override;
    ULONG getIndex() const;

  private:
    ULONG _index;
};
