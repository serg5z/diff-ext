/*
 * Copyright (c) 2003-2006, Sergey Zorin. All rights reserved.
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
#include <util/resource_string.h>

#include "diff_ext.h"
#include "server.h"
#include "resource.h"
#include "icon_factory.h"

const UINT IDM_DIFF=1;
const UINT IDM_DIFF_WITH=3;
const UINT IDM_DIFF_LATER=4;
const UINT IDM_CLEAR=5;
const UINT IDM_DIFF_WITH_BASE=6;

DIFF_EXT::DIFF_EXT() : _n_files(0), _selection(0), _language(1033), _start_menu(false), _ref_count(0L)  {
//  TRACE trace(TEXT("DIFF_EXT::DIFF_EXT()"), TEXT(__FILE__), __LINE__);
  
  _recent_files = SERVER::instance()->recent_files();

  _resource = SERVER::instance()->handle();
  
  SERVER::instance()->lock();
}

DIFF_EXT::~DIFF_EXT() {
//  TRACE trace(TEXT(__f__), TEXT(__FILE__), __LINE__);
  delete[] _selection;
  
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
  } else if(IsEqualIID(refiid, IID_IContextMenu)) {
    *ppv = static_cast<IContextMenu*>(this);
  } else if(IsEqualIID(refiid, IID_IContextMenu2)) {
    *ppv = static_cast<IContextMenu2*>(this);
  } else if(IsEqualIID(refiid, IID_IContextMenu3)) {
    *ppv = static_cast<IContextMenu3*>(this);
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

  if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
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
  HRESULT ret = S_OK/*E_INVALIDARG*/;

  if(data->GetData(&format, &medium) == S_OK) {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__);
    HDROP drop = (HDROP)GlobalLock(medium.hGlobal);
    if(drop != 0) {
      _n_files = DragQueryFile(drop, 0xFFFFFFFF, 0, 0);

      if(_n_files != 0) {
        TCHAR tmp[MAX_PATH]; // use DragQueryFile(drop, i, 0, 0) to obtain and reallocate buffer as needed
        
        initialize_language();
        
        _selection = new STRING[_n_files+1];
        for(unsigned int i = 0; i < _n_files; i++) {
          DragQueryFile(drop, i, tmp, MAX_PATH);
          _selection[i+1] = STRING(tmp);
        }
        
        if(_n_files == 1) {
          LPITEMIDLIST start_menu;
          
          DragQueryFile(drop, 0, tmp, MAX_PATH);
          
          if(SUCCEEDED(SHGetFolderLocation(NULL, CSIDL_STARTMENU, 0, 0, &start_menu))) {
            TCHAR start_menu_path[MAX_PATH]; // use DragQueryFile(drop, i, 0, 0) to obtain and reallocate buffer as needed
            SHGetPathFromIDList(start_menu, start_menu_path);
            
            if(lstrcmp(tmp, start_menu_path) == 0) {
              _start_menu = true;
            }
            
            ILFree(start_menu);
          }
        }
      } else {
        ret = E_INVALIDARG;
      }
      
      GlobalUnlock(medium.hGlobal);
    } else {
      ret = E_INVALIDARG;
    }
    
    ReleaseStgMedium(&medium);
  }
  
  return ret;
}

STDMETHODIMP 
DIFF_EXT::HandleMenuMsg(UINT msg, WPARAM w_param, LPARAM l_param) {
  LRESULT result;
  
  MessageBox(0, TEXT(""), TEXT("HandleMenuMsg"), MB_OK);
  return HandleMenuMsg2(msg, w_param, l_param, &result);
}

STDMETHODIMP 
DIFF_EXT::HandleMenuMsg2(UINT msg, WPARAM, LPARAM l_param, LRESULT *result) {
  LRESULT local;
  
  if(result == 0) {
    result = &local;
  }
  
  *result = NOERROR;
  
  if(msg == WM_MEASUREITEM) {
    MEASUREITEMSTRUCT* mis = (MEASUREITEMSTRUCT*)l_param;
    
    if(mis != 0) {
      if(mis->CtlType == ODT_MENU) {
        MENUITEM* item = (MENUITEM*)mis->itemData;
          
        if(item != 0) {
          item->measure(mis);
          *result = TRUE;
        }
      }
    }
  } else if(msg == WM_DRAWITEM) {
    DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)l_param;
    
    if(dis != 0) {
      if(dis->CtlType == ODT_MENU) {
        MENUITEM* item = (MENUITEM*)dis->itemData;
        
        if(item != 0) {
          item->draw(dis);
          
          *result = TRUE;
        }
      }
    }
  }
  
  return NOERROR;
}

STDMETHODIMP
DIFF_EXT::QueryContextMenu(HMENU menu, UINT position, UINT first_cmd, UINT /*last_cmd not used*/, UINT flags) {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);  
  HRESULT ret = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);
  SUBMENU context_menu(menu);

  if(!(flags & CMF_DEFAULTONLY)) {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__);
    UINT id = first_cmd;
    TCHAR c_str[1024];
    HICON icon = 0;
    
    if(_n_files > 0) {
      MENUITEMINFO item_info;
      MENUITEMINFO separator;

      ZeroMemory(&item_info, sizeof(item_info));
      separator.cbSize = sizeof(MENUITEMINFO);
      separator.fMask = MIIM_TYPE;
      separator.fType = MFT_SEPARATOR;
      separator.dwTypeData = 0;
      
      InsertMenuItem(menu, position, TRUE, &separator);
      position++;
      
      if(_n_files == 1) {
        if(_recent_files->count() > 0) {
          DLIST<STRING>::ITERATOR i = _recent_files->head();
          STRING str  = cut_to_length((*i)->data());
          TCHAR format[256];
          void* args[] = {(void*)str};
          
          load_resource_string(_resource, DIFF_WITH_FILE_STR, format, sizeof(format)/sizeof(format[0]), TEXT("Compare to '%1'"));
          FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, format, 0, 0, (LPTSTR)&c_str, sizeof(c_str)/sizeof(c_str[0]), (char**)args);
          id = first_cmd + IDM_DIFF_WITH;
          
          _diff_with_file = MENUITEM(id, c_str, _start_menu ? 0 : ICON_FACTORY::instance()->diff_with_icon()/*_diff_with_icon*/);
          context_menu.insert(_diff_with_file, position);
          position++;
        }        
      } else if(_n_files == 2) {
        if(SERVER::instance()->tree_way_compare_supported()) {
          if(_recent_files->count() > 0) {
            DLIST<STRING>::ITERATOR i = _recent_files->head();
            STRING str1  = cut_to_length((*i)->data());
            TCHAR format[256];
            void* args[] = {(void*)str1};
            
            load_resource_string(_resource, DIFF3_WITH_FILE_STR, format, sizeof(format)/sizeof(format[0]), TEXT("3-way compare '%1'"));
            FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, format, 0, 0, (LPTSTR)&c_str, sizeof(c_str)/sizeof(c_str[0]), (char**)args);
            id = first_cmd + IDM_DIFF_WITH;
            
            _diff_with_file = MENUITEM(id, c_str, _start_menu ? 0 : ICON_FACTORY::instance()->diff3_with_icon()/*_diff3_with_icon*/);
            context_menu.insert(_diff_with_file, position);
            position++;
          }
        }
        
        load_resource_string(_resource, DIFF_STR, c_str, sizeof(c_str)/sizeof(c_str[0]), TEXT("Compare"));
        
        id = first_cmd + IDM_DIFF;
        _diff = MENUITEM(id, c_str, _start_menu ? 0 : ICON_FACTORY::instance()->diff_icon()/*_diff_icon*/);
        context_menu.insert(_diff, position);
        position++;
      } else if(_n_files == 3) {
        if(SERVER::instance()->tree_way_compare_supported()) {
          load_resource_string(_resource, DIFF3_STR, c_str, sizeof(c_str)/sizeof(c_str[0]), TEXT("3-way compare"));
          
          id = first_cmd + IDM_DIFF;
          _diff = MENUITEM(id, c_str, _start_menu ? 0 : ICON_FACTORY::instance()->diff3_icon()/*_diff3_icon*/);
          context_menu.insert(_diff, position);
          position++;
        }
      }

      load_resource_string(_resource, DIFF_LATER_STR, c_str, sizeof(c_str)/sizeof(c_str[0]), TEXT("Compare later"));
      
      id = first_cmd + IDM_DIFF_LATER;
      _diff_later = MENUITEM(id, c_str, _start_menu ? 0 : ICON_FACTORY::instance()->diff_later_icon()/*_diff_later_icon*/);
      context_menu.insert(_diff_later, position);
      position++;
      
      c_str[0] = '\0';
      if(_n_files == 1) {
        load_resource_string(_resource, DIFF_WITH_STR, c_str, sizeof(c_str)/sizeof(c_str[0]), TEXT("Compare to"));
        icon = _start_menu ? 0 : ICON_FACTORY::instance()->diff_with_icon();
      } else if(_n_files == 2) {
        if(SERVER::instance()->tree_way_compare_supported()) {
          load_resource_string(_resource, DIFF3_WITH_STR, c_str, sizeof(c_str)/sizeof(c_str[0]), TEXT("3-way compare to"));
          icon = _start_menu ? 0 : ICON_FACTORY::instance()->diff3_with_icon();
        }
      }
      
      if(c_str[0] != '\0') {
        if(_recent_files->count() > 0) {
          HMENU file_list = CreateMenu();
          DLIST<STRING>::ITERATOR i = _recent_files->head();

          _diff_with = SUBMENU(file_list, first_cmd + IDM_DIFF_WITH, c_str, icon);
          
          id = first_cmd+IDM_DIFF_WITH_BASE;
          int n = 0;
          while(!i.done()) {
            STRING str;
//            SHFILEINFO file_info;
            LPTSTR tmp;
            
            str = cut_to_length((*i)->data());
            tmp = str;

//            SHGetFileInfo((*i)->data(), 0, &file_info, sizeof(file_info), SHGFI_ICON | SHGFI_SMALLICON);
            MENUITEM item(id, tmp);
            _diff_with.insert(item, n);
            id++;
            i++;
            n++;
          }
          
          InsertMenuItem(file_list, n, TRUE, &separator);
          n++;

          load_resource_string(_resource, CLEAR_STR, c_str, sizeof(c_str)/sizeof(c_str[0]), TEXT("Clear"));
          _clear = MENUITEM(first_cmd + IDM_CLEAR, c_str, _start_menu ? 0 : ICON_FACTORY::instance()->clear_icon());
          _diff_with.insert(_clear, n);

          context_menu.insert(_diff_with, position);

          position++;
        }
      }
      
      InsertMenuItem(menu, position, TRUE, &separator);
      position++;
    }    
    
    ret = MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, id-first_cmd+1);
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
      if(_n_files == 2) {
        _selection[0] = _selection[2];
        diff();
      } else if(_n_files == 3) {
        _selection[0] = _selection[3];
        diff3();
      }
    } else if(LOWORD(ici->lpVerb) == IDM_DIFF_WITH) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      if(_n_files == 1) {
        diff_with(0);
      } else if((_n_files == 2) && (SERVER::instance()->tree_way_compare_supported())) {
        diff3_with(0);
      }        
    } else if(LOWORD(ici->lpVerb) == IDM_DIFF_LATER) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      diff_later();
    } else if((LOWORD(ici->lpVerb) >= IDM_DIFF_WITH_BASE) && (LOWORD(ici->lpVerb) < IDM_DIFF_WITH_BASE+_recent_files->count())) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      if(_n_files == 1) {
        diff_with(LOWORD(ici->lpVerb)-IDM_DIFF_WITH_BASE);
      } else if((_n_files == 2) && (SERVER::instance()->tree_way_compare_supported())) {
        diff3_with(LOWORD(ici->lpVerb)-IDM_DIFF_WITH_BASE);
      }
    } else if(LOWORD(ici->lpVerb) == IDM_CLEAR) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      _recent_files->clear();
    } else {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      ret = E_INVALIDARG;
//      assert(0);
      TCHAR verb[80];
      
      _stprintf(verb, TEXT("%d"), LOWORD(ici->lpVerb));
      MessageBox(0, verb, TEXT("Command id"), MB_OK);
    }
  } else {
    ret = E_INVALIDARG;
  }

  return ret;
}

STDMETHODIMP
DIFF_EXT::GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT*, LPSTR pszName, UINT cchMax) {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);
  HRESULT ret = NOERROR;
  TCHAR resource_string[256];
  
  if(uFlags == GCS_HELPTEXT) {
    if(idCmd == IDM_DIFF) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      if(_n_files == 2) {
        load_resource_string(_resource, DIFF_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]), TEXT("Compare selected files."));
      } else if(_n_files == 3) {
        load_resource_string(_resource, DIFF3_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]), TEXT("3-way compare selected files."));
      } else {
        MessageBox(0, TEXT("This is bad"), TEXT(""), MB_OK);
      }
      
      lstrcpyn((LPTSTR)pszName, resource_string, cchMax);
    } else if(idCmd == IDM_DIFF_WITH) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      if(!_recent_files->empty()) {
        DLIST<STRING>::ITERATOR i = _recent_files->head();
        LPTSTR message;
        STRING fn = (*i)->data();
        LPTSTR file_name = fn;
        void* args[] = {(void*)file_name};
        
        if(_n_files == 1) {
  	  load_resource_string(_resource, DIFF_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]), TEXT("Compare ''%1''."));	
        } else if(_n_files == 2) {
  	  load_resource_string(_resource, DIFF3_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]), TEXT("3-way compare to ''%1''."));	
        } else {
          MessageBox(0, TEXT("This is bad"), TEXT(""), MB_OK);
        }

	FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, resource_string, 0, 0, (LPTSTR)&message, 0, (char**)args);
	lstrcpyn((LPTSTR)pszName, message, cchMax);
	LocalFree(message);
      } else {
	lstrcpyn((LPTSTR)pszName, TEXT(""), cchMax);
      }
    } else if(idCmd == IDM_DIFF_LATER) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      load_resource_string(_resource, DIFF_LATER_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]), TEXT("Save selected file(s) for later comparison."));
      
      lstrcpyn((LPTSTR)pszName, resource_string, cchMax);
    } else if(idCmd == IDM_CLEAR) {
      load_resource_string(_resource, CLEAR_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]), TEXT("Clear selected files list."));
      
      lstrcpyn((LPTSTR)pszName, resource_string, cchMax);
    } else if((idCmd >= IDM_DIFF_WITH_BASE) && (idCmd < IDM_DIFF_WITH_BASE+_recent_files->count())) {
//      TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
      if(!_recent_files->empty()) {
	UINT_PTR num = idCmd-IDM_DIFF_WITH_BASE;
	DLIST<STRING>::ITERATOR i = _recent_files->head();
	for(unsigned int j = 0; j < num; j++) {
	  i++;
        }
      
	LPTSTR message;
        STRING fn = (*i)->data();
        LPTSTR file_name = fn;
        void* args[] = {(void*)file_name};
        
        if(_n_files == 1) {
	  load_resource_string(_resource, DIFF_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]), TEXT("Compare to ''%1''."));
        } else if(_n_files == 2) {
	  load_resource_string(_resource, DIFF3_WITH_HINT, resource_string, sizeof(resource_string)/sizeof(resource_string[0]), TEXT("3-way compare to ''%1''."));
        }

	FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, resource_string, 0, 0, (LPTSTR)&message, 0, (char**)args);
	lstrcpyn((LPTSTR)pszName, message, cchMax);
	LocalFree(message);
      } else {
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
  TCHAR command_template[MAX_PATH*4 + 8]; // path_to_diff+options(MAX_PATH)+2*path_to_files+qoutes&spaces
  LPTSTR command;
  void* args[] = {_selection[1], _selection[0]};

  ZeroMemory(command_template, sizeof(command_template));

  if(RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
    if (RegQueryValueEx(key, TEXT("diff"), 0, 0, (BYTE*)command_template, &length) != ERROR_SUCCESS) {
      command_template[0] = TEXT('\0');
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
  
  FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, 
    command_template, 0, 0, (LPTSTR)&command, 0, (char**)args);
  
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
  
  LocalFree(command);

  //~ fclose(f);
}

void
DIFF_EXT::diff3() {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);
  //~ FILE* f = fopen("d:/DIFF_EXT.log", "a");
  //~ MessageBox(_hwnd, "diff", "command", MB_OK);
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  HKEY key;
  DWORD length = MAX_PATH;
  TCHAR command_template[MAX_PATH*5 + 11]; // path_to_diff+options(MAX_PATH)+3*path_to_files+qoutes&spaces
  LPTSTR command;
  void* args[] = {_selection[1], _selection[2], _selection[0]};

  ZeroMemory(command_template, sizeof(command_template));

  if(RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Z\\diff-ext"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
//    TRACE trace(__FUNCTION__, __FILE__, __LINE__, 4);
    if (RegQueryValueEx(key, TEXT("diff3"), 0, 0, (BYTE*)command_template, &length) != ERROR_SUCCESS) {
      command_template[0] = TEXT('\0');
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
  FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_ARGUMENT_ARRAY, 
    command_template, 0, 0, (LPTSTR)&command, 0, (char**)args);
  
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

  LocalFree(command);
  //~ fclose(f);
}

void
DIFF_EXT::diff_with(unsigned int num) {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);
  //~ STRING str = "diff "+_selection[0]+" and "+_recent_files->at(num);
  //~ MessageBox(_hwnd, str, "command", MB_OK);
  DLIST<STRING>::ITERATOR i = _recent_files->head();
  for(unsigned int j = 0; j < num; j++) {
    i++;
  }

  _selection[0] = (*i)->data();

  diff();
  
  if(!SERVER::instance()->persistent_selection()) {
    _recent_files->remove(i);
  }
}

void
DIFF_EXT::diff3_with(unsigned int num) {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);
  //~ STRING str = "diff "+_selection[0]+" and "+_recent_files->at(num);
  //~ MessageBox(_hwnd, str, "command", MB_OK);
  DLIST<STRING>::ITERATOR i = _recent_files->head();
  for(unsigned int j = 0; j < num; j++) {
    i++;
  }

  _selection[0] = (*i)->data();

  diff3();
  
  if(!SERVER::instance()->persistent_selection()) {
    _recent_files->remove(i);
  }
}

void
DIFF_EXT::diff_later() {
//  TRACE trace(__FUNCTION__, __FILE__, __LINE__);
  //~ FILE* f = fopen("d:/DIFF_EXT.log", "a");
  //~ MessageBox(_hwnd, "diff later", "command", MB_OK);
//  unsigned int start = max(min(SERVER::instance()->history_size(), _n_files)-1, 0);
  for(int i = _n_files; i > 0; i--) {
    bool found = false;
    DLIST<STRING>::ITERATOR it = _recent_files->head();
    DLIST<STRING>::NODE* node = 0;
    
    while(!it.done() && !found) {
      if((*it)->data() == _selection[i]/*_file_name1*/) {
        found = true;
        node = *it;
        _recent_files->unlink(it);
      } else {
        it++;
      }
    }

    if(!found) {
      if(_recent_files->count() == SERVER::instance()->history_size()) {
        DLIST<STRING>::ITERATOR t = _recent_files->tail();
        _recent_files->remove(t);
      }
      _recent_files->prepend(_selection[i]);
    } else {
      _recent_files->prepend(node);
    }
  }
  //~ fprintf(f, "added file %s; new size: %d\n", _selection[0], _recent_files->size());

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
