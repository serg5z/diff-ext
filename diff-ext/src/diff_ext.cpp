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
#include "server.h"
#include "diff_ext_res.h"

const UINT IDM_TEST_COMMAND=10;
const UINT IDM_DIFF=20;
const UINT IDM_DIFF_LATER=30;
const UINT IDM_DIFF_WITH=40;
const UINT IDM_DIFF_WITH_BASE=50;

DEQUE<STRING> DIFF_EXT::_recent_files(4);

#define _T(x)  x

// *********************** DIFF_EXT *************************
DIFF_EXT::DIFF_EXT() : _n_files(0), _file_name1(""), _file_name2(""), _language(1033), _ref_count(0L) {
  if(_recent_files.size() == 0)
    _recent_files = DEQUE<STRING>(8);

  _resource = SERVER::instance()->handle();
  
  SERVER::instance()->lock();
}

DIFF_EXT::~DIFF_EXT() {
  if(_resource != SERVER::instance()->handle()) {
    FreeLibrary(_resource);
  }
  
  SERVER::instance()->release();
}

STDMETHODIMP
DIFF_EXT::QueryInterface(REFIID refiid, void** ppv) {
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
DIFF_EXT::AddRef() {
  return InterlockedIncrement((LPLONG)&_ref_count);
}

STDMETHODIMP_(ULONG)
DIFF_EXT::Release() {
  ULONG ret = 0L;
  
  if(InterlockedDecrement((LPLONG)&_ref_count) != 0)
    ret = _ref_count;
  else
    delete this;

  return ret;
}

void
DIFF_EXT::initialize_language() {
  HKEY key;
  DWORD language = 0;
  DWORD len;
  TCHAR language_lib[MAX_PATH];

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\DIFF_EXT\\"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
    len = sizeof(DWORD);
    RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&language, &len);

    RegCloseKey(key);

    if(language != _language) {
      _language = language;
      
      if(_resource != SERVER::instance()->handle()) {
	FreeLibrary(_resource);
      }
      
      if(_language != 1033) {
	GetModuleFileName(SERVER::instance()->handle(), language_lib, sizeof(language_lib)/sizeof(language_lib[0]));
	
	wsprintf(language_lib+strlen(language_lib)-4, "%ld.dll", language);
	
	_resource = LoadLibrary(language_lib);

	if(_resource == 0) {
	  _resource = SERVER::instance()->handle();
	  _language = 1033;
 	}
      }
      else {
	_resource = SERVER::instance()->handle();
      }
    }
  }
}

STDMETHODIMP
DIFF_EXT::Initialize(LPCITEMIDLIST /*folder not used*/, IDataObject* data, HKEY /*key not used*/) {
  FORMATETC format = {CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
  STGMEDIUM medium;
  medium.tymed = TYMED_HGLOBAL;
  HRESULT ret = E_INVALIDARG;

  if(data->GetData(&format, &medium) == S_OK) {
    HDROP drop = (HDROP)medium.hGlobal;
    _n_files = DragQueryFile(drop, 0xFFFFFFFF, 0, 0);

    TCHAR tmp[MAX_PATH];
    
    if(_n_files == 1) {
      initialize_language();
      DragQueryFile(drop, 0, tmp, MAX_PATH);

      _file_name1 = STRING(tmp);

      ret = S_OK;
    } else if(_n_files == 2) {
      initialize_language();
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
DIFF_EXT::QueryContextMenu(HMENU menu, UINT position, UINT first_cmd, UINT /*last_cmd not used*/, UINT flags) {
  TCHAR resource_string[256];
  int resource_string_length;
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
      resource_string_length = LoadString(_resource, DIFF_WITH_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
      if(resource_string_length == 0) {
	resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_WITH_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  lstrcpy(resource_string, TEXT("diff with"));
	  MessageBox(0, TEXT("Can not load 'DIFF_WITH_STR' string resource"), TEXT("ERROR"), MB_OK);
	}
      }
      
      STRING str(resource_string);
      LPSTR c_str;
      UINT state = MFS_DISABLED;

      if(_recent_files.count() > 0) {
        state = MFS_ENABLED;
	str += " '";
        str += cut_to_length(_recent_files.front());
	str += "'";
      }

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

      resource_string_length = LoadString(_resource, DIFF_LATER_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
      if(resource_string_length == 0) {
	resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_LATER_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  lstrcpy(resource_string, TEXT("diff later"));
	  MessageBox(0, TEXT("Can not load 'DIFF_LATER_STR' string resource"), TEXT("ERROR"), MB_OK);
	}
      }
      
      ZeroMemory(&item_info, sizeof(item_info));
      item_info.cbSize = sizeof(MENUITEMINFO);
      item_info.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
      item_info.fType = MFT_STRING;
      item_info.fState = MFS_ENABLED;
      item_info.wID = first_cmd + IDM_DIFF_LATER;
      item_info.dwTypeData = resource_string;
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
      
      resource_string_length = LoadString(_resource, DIFF_WITH_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
      if(resource_string_length == 0) {
	resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_WITH_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  lstrcpy(resource_string, TEXT("diff with"));
	  MessageBox(0, TEXT("Can not load 'DIFF_WITH_STR' string resource"), TEXT("ERROR"), MB_OK);
	}
      }
      
      ZeroMemory(&item_info, sizeof(item_info));
      item_info.cbSize = sizeof(MENUITEMINFO);
      item_info.fMask = MIIM_SUBMENU | MIIM_TYPE | MIIM_STATE;
      item_info.fType = MFT_STRING;
      item_info.fState = state;
      item_info.hSubMenu = file_list;
      item_info.dwTypeData = resource_string;
      InsertMenuItem(menu, position, TRUE, &item_info);
      position++;
    } else if(_n_files == 2) {
      resource_string_length = LoadString(_resource, DIFF_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
      if(resource_string_length == 0) {
	resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  lstrcpy(resource_string, TEXT("diff"));
	  MessageBox(0, TEXT("Can not load 'DIFF_STR' string resource"), TEXT("ERROR"), MB_OK);
	}
      }
      
      STRING str(resource_string); //= "Diff " + cut_to_length(_file_name1, 20)+" and "+cut_to_length(_file_name2, 20);
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
DIFF_EXT::InvokeCommand(LPCMINVOKECOMMANDINFO ici) {
  HRESULT ret = NOERROR;

  _hwnd = ici->hwnd;

  if(HIWORD(ici->lpVerb) == 0) {
//~ char buf[256];
//~ sprintf(buf, "id: %d", LOWORD(ici->lpVerb));
//~ MessageBox(0, buf, "InvokeCommand", MB_OK);
    if(LOWORD(ici->lpVerb) == IDM_DIFF) {
      diff();
    } else if(LOWORD(ici->lpVerb) == IDM_DIFF_WITH) {
      diff_with(0);
    } else if(LOWORD(ici->lpVerb) == IDM_DIFF_LATER) {
      diff_later();
    } else if((LOWORD(ici->lpVerb) >= IDM_DIFF_WITH_BASE) && (LOWORD(ici->lpVerb) < IDM_DIFF_WITH_BASE+_recent_files.size())) {
      diff_with(LOWORD(ici->lpVerb)-IDM_DIFF_WITH_BASE);
    } else {
      ret = E_INVALIDARG;
      assert(0);
    }
  }

  return ret;
}

STDMETHODIMP
DIFF_EXT::GetCommandString(UINT idCmd, UINT uFlags, UINT*, LPSTR pszName, UINT cchMax) {
  HRESULT ret = NOERROR;
  TCHAR resource_string[256];
  int resource_string_length;
  
  if(uFlags == GCS_HELPTEXT) {
    if(idCmd == IDM_DIFF) {
      resource_string_length = LoadString(_resource, DIFF_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
      if(resource_string_length == 0) {
	resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  lstrcpy(resource_string, TEXT("Compare selected files"));
	  MessageBox(0, TEXT("Can not load 'DIFF_HINT' string resource"), TEXT("ERROR"), MB_OK);
	}
      }
      
      lstrcpyn(pszName, resource_string, cchMax);
    } else if(idCmd == IDM_DIFF_WITH) {
      if(!_recent_files.empty()) {
	LPSTR file_name1 = _file_name1;
	LPSTR file_name2 = _recent_files.front();
	LPSTR message;
	LPSTR args[] = {file_name1, file_name2};

	resource_string_length = LoadString(_resource, DIFF_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	  
	  if(resource_string_length == 0) {
	    lstrcpy(resource_string, TEXT("Compare '%1' with '%2'"));
	    MessageBox(0, TEXT("Can not load 'DIFF_WITH_HINT' string resource"), TEXT("ERROR"), MB_OK);
	  }
	}
	
	FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, resource_string, 0, 0, (LPTSTR) &message, 0, args);
	lstrcpyn(pszName, message, cchMax);
	LocalFree(message);
      }
      else {
	lstrcpyn(pszName, _T(""), cchMax);
      }
    } else if(idCmd == IDM_DIFF_LATER) {
      resource_string_length = LoadString(_resource, DIFF_LATER_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
      if(resource_string_length == 0) {
	resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_LATER_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  lstrcpy(resource_string, TEXT("Save '%1' for later comparison"));
	  MessageBox(0, TEXT("Can not load 'DIFF_LATER_HINT' string resource"), TEXT("ERROR"), MB_OK);
	}
      }
      
      LPSTR file_name1 = _file_name1;
      LPSTR message;
      LPSTR args[] = {file_name1};

      FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, resource_string, 0, 0, (LPTSTR) &message, 0, args);
      lstrcpyn(pszName, message, cchMax);
      LocalFree(message);
    } else if((idCmd >= IDM_DIFF_WITH_BASE) && (idCmd < IDM_DIFF_WITH_BASE+_recent_files.size())) {
      if(!_recent_files.empty()) {
	resource_string_length = LoadString(_resource, DIFF_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
	if(resource_string_length == 0) {
	  resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	  
	  if(resource_string_length == 0) {
	    lstrcpy(resource_string, TEXT("Compare '%1' with '%2'"));
	    MessageBox(0, TEXT("Can not load 'DIFF_WITH_HINT' string resource"), TEXT("ERROR"), MB_OK);
	  }
	}
	unsigned int num = idCmd-IDM_DIFF_WITH_BASE;
	LPSTR file_name1 = _file_name1;
	
	DEQUE<STRING>::CURSOR i = _recent_files.begin();
	for(unsigned int j = 0; j < num; j++)
	  i++;
      
	LPSTR file_name2 = _recent_files.item(i);
	LPSTR message;
	LPSTR args[] = {file_name1, file_name2};

	FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, resource_string, 0, 0, (LPTSTR) &message, 0, args);
	lstrcpyn(pszName, message, cchMax);
	LocalFree(message);
      }
      else {
	lstrcpyn(pszName, _T(""), cchMax);
      }
    }
  }

  return ret;
}

void
DIFF_EXT::diff() {
  //~ FILE* f = fopen("d:/DIFF_EXT.log", "a");
  //~ MessageBox(_hwnd, "diff", "command", MB_OK);
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  HKEY key;
  DWORD length = MAX_PATH;
  TCHAR command[MAX_PATH*3 + 6];
  TCHAR tmp[MAX_PATH];

  ZeroMemory(command, sizeof(command));

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\DIFF_EXT"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
    if (RegQueryValueEx(key, TEXT("diff"), 0, 0, (BYTE*)command, &length) != ERROR_SUCCESS)
      command[0] = '\0';
    else
      command[length] = '\0';

    RegCloseKey(key);
  } else {
    char* message;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0,
      GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &message, 0, 0);

    MessageBox(0, message, TEXT("GetLastError"), MB_OK | MB_ICONINFORMATION);

    LocalFree(message);
  }

  lstrcat(command," \"");
  lstrcpyn(tmp, _file_name1, MAX_PATH);
  lstrcat(command, tmp);
  lstrcat(command,"\" \"");
  lstrcpyn(tmp, _file_name2, MAX_PATH);
  lstrcat(command, tmp);
  lstrcat(command,"\"");

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  if (CreateProcess(0, command, 0, 0, FALSE, 0, 0, 0, &si, &pi) == 0) {
/*
    LPTSTR message;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0,
      GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &message, 0, 0);
    MessageBox(0, message, TEXT("GetLastError"), MB_OK | MB_ICONINFORMATION);

    LocalFree(message);
*/
    MessageBox(_hwnd, TEXT("Error creating process: Check if differ is in your path!"), TEXT("diff_ext.dll error"), MB_OK);
  } else {
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
  }

  //~ fclose(f);
}

void
DIFF_EXT::diff_with(unsigned int num) {
  //~ STRING str = "diff "+_file_name1+" and "+_recent_files.at(num);
  //~ MessageBox(_hwnd, str, "command", MB_OK);
  DEQUE<STRING>::CURSOR i = _recent_files.begin();
  for(unsigned int j = 0; j < num; j++)
    i++;

  _file_name2 = _recent_files.item(i);

  diff();
}

void
DIFF_EXT::diff_later() {
  //~ FILE* f = fopen("d:/DIFF_EXT.log", "a");
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
DIFF_EXT::cut_to_length(STRING in, int max_len) {
  STRING ret;
  int length = in.length();
  
  if(length > max_len) {
    ret = in.substr(0, (max_len-3)/2);
    ret += "...";
    ret += in.substr(length-(max_len-3)/2, STRING::end);
  }
  else {
    ret = in;
  }
  
  return ret;
  //~ return in;
}
