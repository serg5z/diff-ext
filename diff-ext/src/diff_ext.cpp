/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#include <assert.h>
#include <stdio.h>

#include "diff_ext.h"

const int IDM_TEST_COMMAND=10;
const int IDM_DIFF=20;
const int IDM_DIFF_LATER=30;
const int IDM_DIFF_WITH=40;
const int IDM_DIFF_WITH_BASE=50;

DEQUE<STRING> CShellExt::_recent_files(4);

extern "C" void inc_cRefThisDLL();
extern "C" void dec_cRefThisDLL();

#define _T(x)  x

// *********************** CShellExt *************************
CShellExt::CShellExt() : _n_files(0), _file_name1(""), _file_name2(""), _ref_count(0L) {
  if(_recent_files.size() == 0)
    _recent_files = DEQUE<STRING>(8);

  inc_cRefThisDLL();
}

CShellExt::~CShellExt() {
  dec_cRefThisDLL();
}

STDMETHODIMP
CShellExt::QueryInterface(REFIID refiid, void** ppv) {
  HRESULT ret = E_NOINTERFACE;
  *ppv = 0;

  if(IsEqualIID(refiid, IID_IShellExtInit) || IsEqualIID(refiid, IID_IUnknown)) {
    *ppv = static_cast<IShellExtInit*>(this);
  } else if (IsEqualIID(refiid, IID_IContextMenu)) {
    *ppv = static_cast<IContextMenu*>(this);
  }

  if(*ppv != 0) {
    AddRef();

    ret = NOERROR;
  }

  return ret;
}

STDMETHODIMP_(ULONG)
CShellExt::AddRef() {
  return InterlockedIncrement((LPLONG)&_ref_count);
}

STDMETHODIMP_(ULONG)
CShellExt::Release() {
  ULONG ret = 0L;
  
  if(InterlockedDecrement((LPLONG)&_ref_count) != 0)
    ret = _ref_count;
  else
    delete this;

  return ret;
}

STDMETHODIMP
CShellExt::Initialize(LPCITEMIDLIST /*folder not used*/, IDataObject* data, HKEY /*key not used*/) {
  FORMATETC format = {CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
  STGMEDIUM medium;
  medium.tymed = TYMED_HGLOBAL;
  HRESULT ret = E_INVALIDARG;

  if(data->GetData(&format, &medium) == S_OK) {
    HDROP drop = (HDROP)medium.hGlobal;
    _n_files = DragQueryFile(drop, 0xFFFFFFFF, 0, 0);

    TCHAR tmp[MAX_PATH];
    
    if(_n_files == 1) {
      DragQueryFile(drop, 0, tmp, MAX_PATH);

      _file_name1 = STRING(tmp);

      ret = S_OK;
    } else if(_n_files == 2) {
      DragQueryFile(drop, 1, tmp, MAX_PATH);

      _file_name1 = STRING(tmp);

      DragQueryFile(drop, 0, tmp, MAX_PATH);

      _file_name2 = STRING(tmp);

      ret = S_OK;
    }
  }

  return ret;
}

STDMETHODIMP
CShellExt::QueryContextMenu(HMENU menu, UINT position, UINT first_cmd, UINT /*last_cmd not used*/, UINT flags) {
  HRESULT ret = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

  if(!(flags & CMF_DEFAULTONLY)) {
    MENUITEMINFO item_info;

    ZeroMemory(&item_info, sizeof(item_info));
    item_info.cbSize = sizeof(MENUITEMINFO);
    item_info.fMask = MIIM_TYPE;
    item_info.fType = MFT_SEPARATOR;
    item_info.dwTypeData = 0;
    InsertMenuItem(menu, position, TRUE, &item_info);
    position++;

    if(_n_files == 1) {
      STRING str = "Diff with: ";
      LPSTR c_str;
      UINT state = MFS_DISABLED;

      if(_recent_files.count() > 0) {
        state = MFS_ENABLED;
        str += _recent_files.front();
      }

      str = cut_to_length(str);
      c_str = str;

      ZeroMemory(&item_info, sizeof(item_info));
      item_info.cbSize = sizeof(MENUITEMINFO);
      item_info.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
      item_info.fType = MFT_STRING;
      item_info.fState = state;
      item_info.wID = first_cmd + IDM_DIFF_WITH;
      item_info.dwTypeData = c_str; //+filename
      InsertMenuItem(menu, position, TRUE, &item_info);
      position++;

      ZeroMemory(&item_info, sizeof(item_info));
      item_info.cbSize = sizeof(MENUITEMINFO);
      item_info.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
      item_info.fType = MFT_STRING;
      item_info.fState = MFS_ENABLED;
      item_info.wID = first_cmd + IDM_DIFF_LATER;
      item_info.dwTypeData = "Diff later";
      InsertMenuItem(menu, position, TRUE, &item_info);
      position++;

      HMENU file_list = CreateMenu();

      DEQUE<STRING>::CURSOR i = _recent_files.begin();

      int n = 0;
      while(!_recent_files.done(i)) {
        str = cut_to_length(_recent_files.item(i));
        c_str = str;

        ZeroMemory(&item_info, sizeof(item_info));
        item_info.cbSize = sizeof(MENUITEMINFO);
        item_info.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
        item_info.fType = MFT_STRING;
        item_info.fState = MFS_ENABLED;
        item_info.wID = first_cmd + IDM_DIFF_WITH_BASE+n;
        item_info.dwTypeData = c_str;
        InsertMenuItem(file_list, first_cmd + IDM_DIFF_WITH_BASE+n, TRUE, &item_info);

        i++;
        n++;
      }
      
      ZeroMemory(&item_info, sizeof(item_info));
      item_info.cbSize = sizeof(MENUITEMINFO);
      item_info.fMask = MIIM_SUBMENU | MIIM_TYPE | MIIM_STATE;
      item_info.fType = MFT_STRING;
      item_info.fState = state;
      item_info.hSubMenu = file_list;
      item_info.dwTypeData = "Diff with";
      InsertMenuItem(menu, position, TRUE, &item_info);
      position++;
    } else if(_n_files == 2) {
      STRING str = "Diff " + cut_to_length(_file_name1, 20)+" and "+cut_to_length(_file_name2, 20);
      LPSTR c_str = str;

      ZeroMemory(&item_info, sizeof(item_info));
      item_info.cbSize = sizeof(MENUITEMINFO);
      item_info.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
      item_info.fType = MFT_STRING;
      item_info.fState = MFS_ENABLED;
      item_info.wID = first_cmd + IDM_DIFF;
      item_info.dwTypeData = c_str;
      InsertMenuItem(menu, position, TRUE, &item_info);
      position++;
    } else {
      assert(false);
    }

    ZeroMemory(&item_info, sizeof(item_info));
    item_info.cbSize = sizeof(MENUITEMINFO);
    item_info.fMask = MIIM_TYPE;
    item_info.fType = MFT_SEPARATOR;
    item_info.dwTypeData = 0;
    InsertMenuItem(menu, position, TRUE, &item_info);
    position++;

    ret = MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(IDM_DIFF_WITH_BASE+_recent_files.size()+1));
  }

  return ret;
}

STDMETHODIMP
CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO ici) {
  HRESULT ret = NOERROR;

  _hwnd = ici->hwnd;

  if(HIWORD(ici->lpVerb) == 0) {
//~ char buf[256];
//~ sprintf(buf, "id: %d", LOWORD(ici->lpVerb));
//~ MessageBox(0, buf, "InvokeCommand", MB_OK);
    if(LOWORD(ici->lpVerb) == IDM_TEST_COMMAND) {
      MessageBox(_hwnd, "Hello World!!!", "Test command", MB_OK);
    } else if(LOWORD(ici->lpVerb) == IDM_DIFF) {
      diff();
    } else if(LOWORD(ici->lpVerb) == IDM_DIFF_WITH) {
      diff_with(0);
    } else if(LOWORD(ici->lpVerb) == IDM_DIFF_LATER) {
      diff_later();
    } else if((LOWORD(ici->lpVerb) >= IDM_DIFF_WITH_BASE) || (LOWORD(ici->lpVerb) < IDM_DIFF_WITH_BASE+_recent_files.size())) {
      diff_with(LOWORD(ici->lpVerb)-IDM_DIFF_WITH_BASE);
    } else {
      ret = E_INVALIDARG;
      assert(0);
    }
  }

  return ret;
}

STDMETHODIMP
CShellExt::GetCommandString(UINT idCmd, UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax) {
  if (uFlags == GCS_HELPTEXT && cchMax > 35)
    lstrcpy(pszName, _T("nothing here yet."));

  return NOERROR;
}

void
CShellExt::diff() {
  //~ FILE* f = fopen("d:/diff_ext.log", "a");
  //~ MessageBox(_hwnd, "diff", "command", MB_OK);
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  HKEY key;
  DWORD length = MAX_PATH;
  char command[MAX_PATH*3 + 6];

  ZeroMemory(command, sizeof(command));

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Z\\diff_ext", 0, KEY_READ, &key) == ERROR_SUCCESS) {
    if (RegQueryValueEx(key, "diff", 0, 0, (BYTE *)command, &length) != ERROR_SUCCESS)
      command[0] = '\0';
    else
      command[length] = '\0';

    RegCloseKey(key);
  } else {
    char* message;

    FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      0,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &message,
      0,
      0
    );

    MessageBox(0, message, "GetLastError", MB_OK | MB_ICONINFORMATION);

    LocalFree(message);
  }

  strcat(command," \"");
  strncat(command,_file_name1, MAX_PATH-1);
  strcat(command,"\" \"");
  strncat(command,_file_name2, MAX_PATH-1);
  strcat(command,"\"");

  //~ const char* s;

  //~ fprintf(f, "command='%s'\n", command);
  //~ s = _file_name1;
  //~ fprintf(f, "f1='%s'\n", s);
  //~ s = _file_name2;
  //~ fprintf(f, "f2='%s'\n", s);
  //~ fprintf(f, "length=%d\n", MAX_PATH);

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  if (CreateProcess(0, command, 0, 0, FALSE, 0, 0, 0, &si, &pi) == 0) {
    MessageBox(_hwnd, "Error creating process: Check if differ is in your path!", "diff_ext.dll error", MB_OK);
  } else {
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
  }

  //~ fclose(f);
}

void
CShellExt::diff_with(unsigned int num) {
  //~ STRING str = "diff "+_file_name1+" and "+_recent_files.at(num);
  //~ MessageBox(_hwnd, str, "command", MB_OK);
  DEQUE<STRING>::CURSOR i = _recent_files.begin();
  for(unsigned int j = 0; j < num; j++)
    i++;

  _file_name2 = _recent_files.item(i);

  diff();
}

void
CShellExt::diff_later() {
  //~ FILE* f = fopen("d:/diff_ext.log", "a");
  //~ MessageBox(_hwnd, "diff later", "command", MB_OK);
  bool found = false;
  DEQUE<STRING>::CURSOR current = _recent_files.begin();
  
  while(!current.done() && !found) {
    if((*current) == _file_name1)
      found = true;
    current++;
  }

  if(!found) {
    if(_recent_files.full())
      _recent_files.pop_back();

    _recent_files.push_front(_file_name1);
  }
  //~ fprintf(f, "added file %s; new size: %d\n", _file_name1, _recent_files.size());

  //~ fclose(f);
}

STRING
CShellExt::cut_to_length(STRING in, int max_len) {
  //~ STRING ret;
  //~ int length = in.length();
  
  //~ if(length > max_len) {
    //~ ret = in.substr(0, (max_len-3)/2);
    //~ ret += "...";
    //~ ret += in.substr(length-(max_len-3)/2, STRING::end);
  //~ }
  //~ else {
    //~ ret = in;
  //~ }
  
  //~ return ret;
  return in;
}
