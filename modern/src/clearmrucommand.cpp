/*
* Copyright (c) 2025, Sergey Zorin.
* All rights reserved.
*
* This software is distributed under the BSD license. See the terms
* of the BSD license in the LICENSE file provided with this software.
*
*/

#include "settings.h"
#include "clearmrucommand.h"


ClearMRUCommand::ClearMRUCommand() : BaseCommand(L"Clear MRU", L"Clear list of remembered files", L"%SystemRoot%\\System32\\shell32.dll,32") {}

IFACEMETHODIMP
ClearMRUCommand::Invoke(IShellItemArray*, IBindCtx*) {
    clearMRU();
    SaveSettings();

    return S_OK;
}

IFACEMETHODIMP
ClearMRUCommand::GetFlags(EXPCMDFLAGS* pFlags) {
    *pFlags = ECF_DEFAULT | ECF_SEPARATORBEFORE;
    return S_OK;
}
