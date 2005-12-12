/*
 * Copyright (c) 2003-2005, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
#include <tchar.h>
#include <assert.h>
#include <stdio.h>

#include <log/log.h>
#include <log/log_message.h>
#include <debug/trace.h>

#include "diff_ext.h"
#include "server.h"
#include "diff_ext.rh"

const UINT IDM_DIFF=10;
const UINT IDM_DIFF_WITH=11;
const UINT IDM_DIFF_LATER=12;
const UINT IDM_DIFF_WITH_BASE=20;

DIFF_EXT::DIFF_EXT() : _n_files(0), _file_name1(TEXT("")), _file_name2(TEXT("")), _language(1033), _ref_count(0L) {
//  TRACE trace(TEXT("DIFF_EXT::DIFF_EXT()"), TEXT(__FILE__), __LINE__);
  
  _recent_files = SERVER::instance()->recent_files();

  _resource = SERVER::instance()->handle();
  
  SERVER::instance()->lock();
}

DIFF_EXT::~DIFF_EXT() {
//  TRACE trace(TEXT(__f__), TEXT(__FILE__), __LINE__);
  
  if(_resource != SERVER::instance()->handle()) {
    FreeLibrary(_resource);
  }
  
  SERVER::instance()->release();
}

STDMETHODIMP
DIFF_EXT::QueryInterface(REFIID refiid, void** ppv) {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);

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
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);

  return InterlockedIncrement((LPLONG)&_ref_count);
}

STDMETHODIMP_(ULONG)
DIFF_EXT::Release() {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);

  ULONG ret = 0L;
  
  if(InterlockedDecrement((LPLONG)&_ref_count) != 0) {
    ret = _ref_count;
  } else {
    delete this;
  }

  return ret;
}

void
DIFF_EXT::initialize_language() {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);

  HKEY key;
  DWORD language = 0;
  DWORD len;

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff-ext\\"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
    len = sizeof(DWORD);
    RegQueryValueEx(key, TEXT("language"), 0, NULL, (BYTE*)&language, &len);

    RegCloseKey(key);

    if(language != _language) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 5);
      _language = language;
      
      if(_resource != SERVER::instance()->handle()) {
	FreeLibrary(_resource);
      }
      
      if(_language != 1033) {
//        TRACE trace(__FUNCTION__, __FILE__, __LINE__, 6);
        TCHAR language_lib[MAX_PATH];
        TCHAR* dot;
	
        GetModuleFileName(SERVER::instance()->handle(), language_lib, sizeof(language_lib)/sizeof(language_lib[0]));
        dot = _tcsrchr(language_lib, TEXT('.'));
        _stprintf(dot, TEXT("%lu.dll"), language);
	
	_resource = LoadLibrary(language_lib);

	if(_resource == 0) {
          LPTSTR message;
          FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0,
            GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &message, 0, 0);
          MessageBox(0, message, TEXT("GetLastError"), MB_OK | MB_ICONINFORMATION);
      
          LocalFree(message);
          
          GetModuleFileName(SERVER::instance()->handle(), language_lib, sizeof(language_lib)/sizeof(language_lib[0]));
          MessageBox(0, language_lib, TEXT("GetLastError"), MB_OK | MB_ICONINFORMATION);
          
	  _resource = SERVER::instance()->handle();
	  _language = 1033;
 	}
      } else {
	_resource = SERVER::instance()->handle();
      }
    }
  }
}

STDMETHODIMP
DIFF_EXT::Initialize(LPCITEMIDLIST /*folder not used*/, IDataObject* data, HKEY /*key not used*/) {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);

  FORMATETC format = {CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
  STGMEDIUM medium;
  medium.tymed = TYMED_HGLOBAL;
  HRESULT ret = E_INVALIDARG;

  if(data->GetData(&format, &medium) == S_OK) {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__);
    HDROP drop = (HDROP)medium.hGlobal;
    _n_files = DragQueryFile(drop, 0xFFFFFFFF, 0, 0);

    TCHAR tmp[MAX_PATH];
    
    initialize_language();
    
    if(_n_files == 1) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__);
      DragQueryFile(drop, 0, tmp, MAX_PATH);

      _file_name1 = STRING(tmp);

      ret = S_OK;
    } else if(_n_files == 2) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__);
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
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);
  
  TCHAR resource_string[256];
  int resource_string_length;
  HRESULT ret = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

  if(!(flags & CMF_DEFAULTONLY)) {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__);
    MENUITEMINFO item_info;
    UINT id = first_cmd;

    ZeroMemory(&item_info, sizeof(item_info));
    item_info.cbSize = sizeof(MENUITEMINFO);
    item_info.fMask = MIIM_TYPE;
    item_info.fType = MFT_SEPARATOR;
    item_info.dwTypeData = 0;
    item_info.wID = id++;
    InsertMenuItem(menu, position, TRUE, &item_info);
    position++;

    if(_n_files == 1) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__);
      
      LPTSTR c_str;
      UINT state = MFS_DISABLED;

      //TODO: Use FormatMessage here
      if(_recent_files->count() > 0) {
//	TRACE trace(__FUNCTION__, __FILE__, __LINE__);
	DLIST<STRING>::ITERATOR i = _recent_files->head();
        STRING str  = cut_to_length((*i)->data());
        void* args[] = {(void*)str};
        
        resource_string_length = LoadString(_resource, DIFF_WITH_FILE_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
        
        if(resource_string_length == 0) {
          resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_WITH_FILE_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
          
          if(resource_string_length == 0) {
            lstrcpy(resource_string, TEXT("diff with '%1'"));
            MessageBox(0, TEXT("Can not load 'DIFF_WITH_FILE_STR' string resource"), TEXT("ERROR"), MB_OK);
          }
        }
        
        state = MFS_ENABLED;
        FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, resource_string, 0, 0, (LPTSTR)&c_str, 0, (char**)args);
      } else {
        resource_string_length = LoadString(_resource, DIFF_WITH_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
        
        if(resource_string_length == 0) {
          resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_WITH_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
          
          if(resource_string_length == 0) {
            lstrcpy(resource_string, TEXT("diff with"));
            MessageBox(0, TEXT("Can not load 'DIFF_WITH_STR' string resource"), TEXT("ERROR"), MB_OK);
          }
        }
        
        c_str = resource_string;
      }

      id = first_cmd + IDM_DIFF_WITH;
      ZeroMemory(&item_info, sizeof(item_info));
      item_info.cbSize = sizeof(MENUITEMINFO);
      item_info.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
      item_info.fType = MFT_STRING;
      item_info.fState = state;
      item_info.wID = id;
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
      
      id = first_cmd + IDM_DIFF_LATER;
      ZeroMemory(&item_info, sizeof(item_info));
      item_info.cbSize = sizeof(MENUITEMINFO);
      item_info.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
      item_info.fType = MFT_STRING;
      item_info.fState = MFS_ENABLED;
      item_info.wID = id;
      item_info.dwTypeData = resource_string;
      InsertMenuItem(menu, position, TRUE, &item_info);
      position++;

      HMENU file_list = CreateMenu();

      DLIST<STRING>::ITERATOR i = _recent_files->head();

      id = first_cmd+IDM_DIFF_WITH_BASE;
      int n = 0;
      while(!i.done()) {
//	TRACE trace(__FUNCTION__, __FILE__, __LINE__);
	STRING str;
        
        str = cut_to_length((*i)->data());
        c_str = str;

        ZeroMemory(&item_info, sizeof(item_info));
        item_info.cbSize = sizeof(MENUITEMINFO);
        item_info.fMask = MIIM_ID | MIIM_STRING;
        item_info.wID =id++;
        item_info.dwTypeData = c_str;
        InsertMenuItem(file_list, IDM_DIFF_WITH_BASE+n, TRUE, &item_info);

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
      item_info.fMask = MIIM_SUBMENU | MIIM_STRING | MIIM_ID | MIIM_STATE;
      item_info.wID = id++;
      item_info.fState = state;
      item_info.hSubMenu = file_list;
      item_info.dwTypeData = resource_string;
      InsertMenuItem(menu, position, TRUE, &item_info);
      position++;
    } else if(_n_files == 2) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__);
      
      resource_string_length = LoadString(_resource, DIFF_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
      if(resource_string_length == 0) {
	resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  lstrcpy(resource_string, TEXT("diff"));
	  MessageBox(0, TEXT("Can not load 'DIFF_STR' string resource"), TEXT("ERROR"), MB_OK);
	}
      }
      
      STRING str(resource_string); //= "Diff " + cut_to_length(_file_name1, 20)+" and "+cut_to_length(_file_name2, 20);
      LPTSTR c_str = str;

      id = first_cmd + IDM_DIFF;
      ZeroMemory(&item_info, sizeof(item_info));
      item_info.cbSize = sizeof(MENUITEMINFO);
      item_info.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
      item_info.fType = MFT_STRING;
      item_info.fState = MFS_ENABLED;
      item_info.wID = id++;
      item_info.dwTypeData = c_str;
      InsertMenuItem(menu, position, TRUE, &item_info);
      position++;
    } else {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__);
      assert(false);
    }

    ZeroMemory(&item_info, sizeof(item_info));
    item_info.cbSize = sizeof(MENUITEMINFO);
    item_info.fMask = MIIM_TYPE;
    item_info.fType = MFT_SEPARATOR;
    item_info.dwTypeData = 0;
    item_info.wID = id++;
    InsertMenuItem(menu, position, TRUE, &item_info);
    position++;

    ret = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, id-first_cmd);
  }

  return ret;
}

STDMETHODIMP
DIFF_EXT::InvokeCommand(LPCMINVOKECOMMANDINFO ici) {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);

  HRESULT ret = NOERROR;

  _hwnd = ici->hwnd;

  if(HIWORD(ici->lpVerb) == 0) {
    if(LOWORD(ici->lpVerb) == IDM_DIFF) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      diff();
    } else if(LOWORD(ici->lpVerb) == IDM_DIFF_WITH) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      diff_with(0);
    } else if(LOWORD(ici->lpVerb) == IDM_DIFF_LATER) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      diff_later();
    } else if((LOWORD(ici->lpVerb) >= IDM_DIFF_WITH_BASE) && (LOWORD(ici->lpVerb) < IDM_DIFF_WITH_BASE+_recent_files->count())) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      diff_with(LOWORD(ici->lpVerb)-IDM_DIFF_WITH_BASE);
    } else {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      ret = E_INVALIDARG;
//      assert(0);
      TCHAR verb[80];
      
      _stprintf(verb, TEXT("%d"), LOWORD(ici->lpVerb));
      MessageBox(0, verb, TEXT("Command id"), MB_OK);
    }
  }

  return ret;
}

STDMETHODIMP
DIFF_EXT::GetCommandString(UINT idCmd, UINT uFlags, UINT*, LPSTR pszName, UINT cchMax) {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);
  HRESULT ret = NOERROR;
  TCHAR resource_string[256];
  int resource_string_length;
  
  if(uFlags == GCS_HELPTEXT) {
    if(idCmd == IDM_DIFF) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      resource_string_length = LoadString(_resource, DIFF_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
      if(resource_string_length == 0) {
	resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  lstrcpy(resource_string, TEXT("Compare selected files"));
	  MessageBox(0, TEXT("Can not load 'DIFF_HINT' string resource"), TEXT("ERROR"), MB_OK);
	}
      }
      
      lstrcpyn((LPTSTR)pszName, resource_string, cchMax);
    } else if(idCmd == IDM_DIFF_WITH) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      if(!_recent_files->empty()) {
        DLIST<STRING>::ITERATOR i = _recent_files->head();
	LPTSTR file_name1 = _file_name1;
	LPTSTR file_name2 = (*i)->data();
	LPTSTR message;
        void* args[] = {(void*)file_name1, (void*)file_name2};

	resource_string_length = LoadString(_resource, DIFF_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	  
	  if(resource_string_length == 0) {
	    lstrcpy(resource_string, TEXT("Compare '%1' with '%2'"));
	    MessageBox(0, TEXT("Can not load 'DIFF_WITH_HINT' string resource"), TEXT("ERROR"), MB_OK);
	  }
	}
	
	FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, resource_string, 0, 0, (LPTSTR) &message, 0, (char**)args);
	lstrcpyn((LPTSTR)pszName, message, cchMax);
	LocalFree(message);
      }
      else {
	lstrcpyn((LPTSTR)pszName, TEXT(""), cchMax);
      }
    } else if(idCmd == IDM_DIFF_LATER) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      resource_string_length = LoadString(_resource, DIFF_LATER_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
      if(resource_string_length == 0) {
	resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_LATER_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	
	if(resource_string_length == 0) {
	  lstrcpy(resource_string, TEXT("Save '%1' for later comparison"));
	  MessageBox(0, TEXT("Can not load 'DIFF_LATER_HINT' string resource"), TEXT("ERROR"), MB_OK);
	}
      }
      
      LPTSTR file_name1 = _file_name1;
      LPTSTR message;
      void* args[] = {(void*)file_name1};

      FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, resource_string, 0, 0, (LPTSTR) &message, 0, (char**)args);
      lstrcpyn((LPTSTR)pszName, message, cchMax);
      LocalFree(message);
    } else if((idCmd >= IDM_DIFF_WITH_BASE) && (idCmd < IDM_DIFF_WITH_BASE+_recent_files->count())) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      if(!_recent_files->empty()) {
	resource_string_length = LoadString(_resource, DIFF_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
	if(resource_string_length == 0) {
	  resource_string_length = LoadString(SERVER::instance()->handle(), DIFF_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
	  
	  if(resource_string_length == 0) {
	    lstrcpy(resource_string, TEXT("Compare '%1' with '%2'"));
	    MessageBox(0, TEXT("Can not load 'DIFF_WITH_HINT' string resource"), TEXT("ERROR"), MB_OK);
	  }
	}
	unsigned int num = idCmd-IDM_DIFF_WITH_BASE;
	LPTSTR file_name1 = _file_name1;
	
	DLIST<STRING>::ITERATOR i = _recent_files->head();
	for(unsigned int j = 0; j < num; j++)
	  i++;
      
	LPTSTR file_name2 = (*i)->data();
	LPTSTR message;
        void* args[] = {file_name1, file_name2};

	FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, resource_string, 0, 0, (LPTSTR) &message, 0, (char**)args);
	lstrcpyn((LPTSTR)pszName, message, cchMax);
	LocalFree(message);
      }
      else {
//        TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
	lstrcpyn((LPTSTR)pszName, TEXT(""), cchMax);
      }
    }
  }

  return ret;
}

void
DIFF_EXT::diff() {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);
  //~ FILE* f = fopen("d:/DIFF_EXT.log", "a");
  //~ MessageBox(_hwnd, "diff", "command", MB_OK);
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  HKEY key;
  DWORD length = MAX_PATH;
  TCHAR command[MAX_PATH*3 + 6];
  TCHAR tmp[MAX_PATH];

  ZeroMemory(command, sizeof(command));

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
    if (RegQueryValueEx(key, TEXT("diff"), 0, 0, (BYTE*)command, &length) != ERROR_SUCCESS) {
      command[0] = TEXT('\0');
    }

    RegCloseKey(key);
  } else {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
    LPTSTR message;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0,
      GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &message, 0, 0);

    MessageBox(0, message, TEXT("GetLastError"), MB_OK | MB_ICONINFORMATION);

    LocalFree(message);
  }

  _tcscat(command, TEXT(" \""));
  _tcsncpy(tmp, _file_name1, MAX_PATH);
  _tcscat(command, tmp);
  _tcscat(command, TEXT("\" \""));
  _tcsncpy(tmp, _file_name2, MAX_PATH);
  _tcscat(command, tmp);
  _tcscat(command, TEXT("\""));

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);

  if (CreateProcess(0, command, 0, 0, FALSE, 0, 0, 0, &si, &pi) == 0) {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
    TCHAR resource_string[1024];
    TCHAR error_string[256];
    int string_length;
    
    string_length = LoadString(_resource, CREATE_PROCESS_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
  
    if(string_length == 0) {
      string_length = LoadString(SERVER::instance()->handle(), CREATE_PROCESS_STR, resource_string, sizeof(resource_string)/sizeof(resource_string[0]));
      
      if(string_length == 0) {
	lstrcpy(resource_string, TEXT("Could not start diff command. Please run diff_ext setup program and verify your configuration."));
	MessageBox(0, TEXT("Can not load 'CREATE_PROCESS_STR' string resource"), TEXT("ERROR"), MB_OK);
      }
    }
    
    string_length = LoadString(_resource, ERROR_STR, error_string, sizeof(error_string)/sizeof(error_string[0]));
  
    if(string_length == 0) {
      string_length = LoadString(SERVER::instance()->handle(), ERROR_STR, error_string, sizeof(error_string)/sizeof(error_string[0]));
      
      if(string_length == 0) {
	lstrcpy(error_string, TEXT("Error"));
	MessageBox(0, TEXT("Can not load 'ERROR_STR' string resource"), TEXT("ERROR"), MB_OK);
      }
    }

    MessageBox(_hwnd, resource_string, error_string, MB_OK);
  } else {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
/*    
    LPTSTR message;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0,
      GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &message, 0, 0);
    MessageBox(0, message, TEXT("GetLastError"), MB_OK | MB_ICONINFORMATION);

    LocalFree(message);
*/
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
  }

  //~ fclose(f);
}

void
DIFF_EXT::diff_with(unsigned int num) {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);
  //~ STRING str = "diff "+_file_name1+" and "+_recent_files->at(num);
  //~ MessageBox(_hwnd, str, "command", MB_OK);
  DLIST<STRING>::ITERATOR i = _recent_files->head();
  for(unsigned int j = 0; j < num; j++) {
    i++;
  }

  _file_name2 = (*i)->data();

  diff();
}

void
DIFF_EXT::diff_later() {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);
  //~ FILE* f = fopen("d:/DIFF_EXT.log", "a");
  //~ MessageBox(_hwnd, "diff later", "command", MB_OK);
  bool found = false;
  DLIST<STRING>::ITERATOR i = _recent_files->head();
  DLIST<STRING>::NODE* node = 0;
  
  while(!i.done() && !found) {
    if((*i)->data() == _file_name1) {
      found = true;
      node = *i;
      _recent_files->unlink(i);
    } else {
      i++;
    }
  }

  if(!found) {
    if(_recent_files->count() == SERVER::instance()->history_size()) {
      DLIST<STRING>::ITERATOR t = _recent_files->tail();
      _recent_files->remove(t);
    }
    _recent_files->prepend(_file_name1);
  } else {
    _recent_files->prepend(node);
  }
  //~ fprintf(f, "added file %s; new size: %d\n", _file_name1, _recent_files->size());

  //~ fclose(f);
}

STRING
DIFF_EXT::cut_to_length(STRING in, int max_len) {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);

  STRING ret;
  int length = in.length();
  
  if(length > max_len) {
    ret = in.substr(0, (max_len-3)/2);
    ret += TEXT("...");
    ret += in.substr(length-(max_len-3)/2, STRING::end);
  }
  else {
    ret = in;
  }
  
  return ret;
}
