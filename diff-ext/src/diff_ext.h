/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __diff_ext_h__
#define __diff_ext_h__

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>

#include <util/cont/deque.h>
#include <util/string.h>

// this is the actual OLE Shell context menu handler
class DIFF_EXT : public IContextMenu, IShellExtInit {
  public:
    DIFF_EXT();
    virtual ~DIFF_EXT();

    //IUnknown members
    STDMETHODIMP QueryInterface(REFIID interface_id, void** result);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    //IShell members
    STDMETHODIMP QueryContextMenu(HMENU menu, UINT index, UINT cmd_first, UINT cmd_last, UINT flags);
    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO info);
    STDMETHODIMP GetCommandString(UINT cmd, UINT flags, UINT* reserved, LPSTR name, UINT name_length);

    //IShellExtInit methods
    STDMETHODIMP Initialize(LPCITEMIDLIST folder, IDataObject* subj, HKEY key);

  private:
    void diff();
    void diff_with(unsigned int num);
    void diff_later();
    STRING cut_to_length(STRING, int length = 64);
    void initialize_language();

  private:
    UINT _n_files;
    STRING _file_name1;
    STRING _file_name2;
    DEQUE<STRING>* _recent_files;
    HINSTANCE _resource;
    DWORD _language;
    HWND _hwnd;

    ULONG  _ref_count;
};

#endif // __diff_ext_h__
