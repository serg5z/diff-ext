/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __diff_ext_h__
#define __diff_ext_h__

/*
 * $Id$
 */

#include <windows.h>
#include <windowsx.h>
#include <shlobj.h>

#include <olectl.h>

#include <util/cont/deque.h>
#include <util/string.h>

//
// The class ID of this Shell extension class.
//
// class id:  {51EEE242-AD87-11d3-9C1E-0090278BBD99}
//
//
// NOTE!!!  If you use this shell extension as a starting point,
//     you MUST change the GUID below.  Simply run UUIDGEN.EXE
//     to generate a new GUID.
//

// {51EEE242-AD87-11d3-9C1E-0090278BBD99}
// static const GUID <<name>> =
// { 0x51eee242, 0xad87, 0x11d3, { 0x9c, 0x1e, 0x0, 0x90, 0x27, 0x8b, 0xbd, 0x99 } };
//
//

// {51EEE242-AD87-11d3-9C1E-0090278BBD99}
// IMPLEMENT_OLECREATE(<<class>>, <<external_name>>,
// 0x51eee242, 0xad87, 0x11d3, 0x9c, 0x1e, 0x0, 0x90, 0x27, 0x8b, 0xbd, 0x99);
//

// {51EEE242-AD87-11d3-9C1E-0090278BBD99}  -- this is the registry format

// this is the actual OLE Shell context menu handler
class CShellExt : public IContextMenu, IShellExtInit {
  public:
    CShellExt();
    virtual ~CShellExt();

    //IUnknown members
    STDMETHODIMP QueryInterface(REFIID interface_id, void** result);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    //IShell members
    STDMETHODIMP QueryContextMenu(HMENU menu, UINT index, UINT cmd_first, UINT cmd_last, UINT flags);
    STDMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO info);
    STDMETHODIMP GetCommandString(UINT cmd, UINT flags, LPUINT reserved, LPSTR name, UINT name_length);

    //IShellExtInit methods
    STDMETHODIMP Initialize(LPCITEMIDLIST folder, IDataObject* subj, HKEY key);

  private:
    void diff();
    void diff_with(unsigned int num);
    void diff_later();

  private:
    UINT _n_files;
    STRING _file_name1;
    STRING _file_name2;
    static DEQUE<STRING> _recent_files;
    HWND _hwnd;

    ULONG  _ref_count;
};

#endif // __diff_ext_h__
