/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#pragma once

#include "basecommand.h"


class
__declspec(uuid("C42D8356-32CB-11F0-8E44-FAE5B572B91D"))
CompareFilesCommand : public BaseCommand {
    public:
        CompareFilesCommand();
        IFACEMETHODIMP GetIcon(IShellItemArray*, LPWSTR* icon) override;
        IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
        IFACEMETHODIMP GetState(IShellItemArray*, BOOL, EXPCMDSTATE*) override;
};
