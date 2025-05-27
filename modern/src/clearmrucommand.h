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


class ClearMRUCommand : public BaseCommand {
    public:
        ClearMRUCommand();
        IFACEMETHODIMP Invoke(IShellItemArray*, IBindCtx*) override;
        IFACEMETHODIMP GetFlags(EXPCMDFLAGS*) override;
};
