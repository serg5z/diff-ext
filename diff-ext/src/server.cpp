/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#include <stdio.h>

#include <windows.h>

#include <shlguid.h>
#include <olectl.h>
#include <objidl.h>

#include <objbase.h>
#include <initguid.h>

#include <log/log.h>
#include <log/file_sink.h>

#include "server.h"
#include "class_factory.h"

#define ARRAYSIZE(a)    (sizeof(a)/sizeof(a[0]))

#define SHELLEXNAME    "diff_ext"

// registry key util struct
typedef struct {
  LPTSTR subkey;
  LPTSTR name;
  LPTSTR value;
}
REGSTRUCT, *LPREGSTRUCT;

static HINSTANCE server_instance; // Handle to this DLL itself.

DEFINE_GUID(CLSID_DIFF_EXT, 0xA0482097, 0xC69D, 0x4DEC, 0x8A, 0xB6, 0xD3, 0xA2, 0x59, 0xAC, 0xC1, 0x51);

static LPCTSTR no[] = {
  TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"), TEXT("5"), TEXT("6"), TEXT("7"), TEXT("8"), TEXT("9"), 
  TEXT("10"), TEXT("11"), TEXT("12"), TEXT("13"), TEXT("14"), TEXT("15"), TEXT("16"), TEXT("17"), TEXT("18"), TEXT("19"), 
  TEXT("20"), TEXT("21"), TEXT("22"), TEXT("23"), TEXT("24"), TEXT("25"), TEXT("26"), TEXT("27"), TEXT("28"), TEXT("29"),
  TEXT("30"), TEXT("31"), TEXT("32"), TEXT("33"), TEXT("34"), TEXT("35"), TEXT("36"), TEXT("37"), TEXT("38"), TEXT("39"),
  TEXT("40"), TEXT("41"), TEXT("42"), TEXT("43"), TEXT("44"), TEXT("45"), TEXT("46"), TEXT("47"), TEXT("48"), TEXT("49"), 
  TEXT("50"), TEXT("51"), TEXT("52"), TEXT("53"), TEXT("54"),TEXT("55"), TEXT("56"), TEXT("57"), TEXT("58"), TEXT("59"),
  TEXT("60"), TEXT("61"), TEXT("62"), TEXT("63"), TEXT("64"),
  0
};

static const unsigned int max_history_size = sizeof(no)/sizeof(no[0]);

STDAPI 
DllCanUnloadNow(void) {
  HRESULT ret = S_FALSE;
  
  if(SERVER::instance()->refference_count() == 0)
    ret = S_OK;
  
  return ret;
}

extern "C" int APIENTRY
DllMain(HINSTANCE instance, DWORD reason, LPVOID /* reserved */) {
  switch (reason) {
    case DLL_PROCESS_ATTACH:
      server_instance = instance;
      break;

    case DLL_PROCESS_DETACH:
      SERVER::instance()->save_history();
      break;
  }

  return 1;
}

STDAPI 
DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppvOut) {
  HRESULT ret = CLASS_E_CLASSNOTAVAILABLE;
  *ppvOut = 0;

  if (IsEqualIID(rclsid, CLSID_DIFF_EXT)) {
    CLASS_FACTORY* pcf = new CLASS_FACTORY();

    ret = pcf->QueryInterface(riid, ppvOut);
  }

  return ret;
}

STDAPI
DllRegisterServer() {
  return SERVER::instance()->do_register();
}

STDAPI
DllUnregisterServer() {
  return SERVER::instance()->do_unregister();
}

SERVER::SERVER()  : _refference_count(0), _recent_files(0), _file_sink(0) {
  HKEY key;
  TCHAR log_path[MAX_PATH] = TEXT("");
  LRESULT enabled = 0;
  LRESULT level = 0;
    
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\diff_ext\\"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
    DWORD hlen = MAX_PATH;
  
    RegQueryValueEx(key, TEXT("log_file"), 0, NULL, (BYTE*)log_path, &hlen);
    RegQueryValueEx(key, TEXT("log_level"), 0, NULL, (BYTE*)(&level), &hlen);

    hlen = sizeof(DWORD);
    if(RegQueryValueEx(key, TEXT("log"), 0, NULL, (BYTE*)(&enabled), &hlen) != ERROR_SUCCESS) {
      enabled = 0;
    }

    RegCloseKey(key);
  }
  
  if(enabled == 1) {
    _file_sink = new FILE_SINK(log_path, level);
    
    LOG::instance()->add_sink(_file_sink);
  }
}

SERVER::~SERVER() {
  if(_recent_files != 0) {
    delete _recent_files;
  }
  
  if(_file_sink != 0) {
    delete _file_sink;
  }
}

HINSTANCE 
SERVER::handle() const {
  return server_instance;
}

void 
SERVER::lock() {
  InterlockedIncrement(&_refference_count);
}

void  
SERVER::release() {
  InterlockedDecrement(&_refference_count);
}

DEQUE<STRING>*
SERVER::recent_files() {
  HKEY key;
  DWORD history_size = 8;
  DWORD len;
  
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\DIFF_EXT\\"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
    len = sizeof(DWORD);
    RegQueryValueEx(key, TEXT("history_size"), 0, NULL, (BYTE*)&history_size, &len);
    
    RegCloseKey(key);
  }
  
  if(history_size > max_history_size) {
    history_size = max_history_size;
  }

  DEQUE<STRING>* new_history = new DEQUE<STRING>(history_size);
  
  if(_recent_files != 0) {
    unsigned int n = history_size;
    DEQUE<STRING>::CURSOR i = _recent_files->begin();

    if(n > _recent_files->count()) {
      n = _recent_files->count();
    }
    
    while((!_recent_files->done(i)) && (n > 0)) {
      new_history->push_back((*i));
      i++;
      n--;
    }
    
    delete _recent_files;
  } else { // read history from the registry
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\DIFF_EXT\\history\\"), 0, KEY_READ, &key) == ERROR_SUCCESS) {
      TCHAR file[MAX_PATH];
      bool stop = false;
      
      for(unsigned int i = 0; !stop && (no[i] != 0) && (i < history_size); i++) {
        len = MAX_PATH;
	if(RegQueryValueEx(key, no[i], 0, 0, (BYTE*)file, &len) == ERROR_SUCCESS) {
	  new_history->push_back(file);
	} else {
	  stop = true;
	  LPTSTR message;
	  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0,
	    GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	    (LPTSTR) &message, 0, 0);
	  MessageBox(0, message, TEXT("GetLastError"), MB_OK | MB_ICONINFORMATION);
      
	  LocalFree(message);
	}
      }
      
      RegCloseKey(key);
    }
  }
  
  _recent_files = new_history;
  
  return _recent_files;
}

void
SERVER::save_history() const {
  HKEY key;
  DWORD len;

  if(_recent_files != 0) {
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Software\\Z\\DIFF_EXT\\history\\"), 0, KEY_SET_VALUE, &key) == ERROR_SUCCESS) {
      len = MAX_PATH;
      int n = 0;
      
      DEQUE<STRING>::CURSOR i = _recent_files->begin();
      
      while(!_recent_files->done(i)) {
	STRING str = (*i);
	LPSTR c_str = str;
	if(RegSetValueEx(key, no[n], 0, REG_SZ, (const BYTE*)c_str, (*i).length()) != ERROR_SUCCESS) {
	  LPTSTR message;
	  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0,
	    GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
	    (LPTSTR) &message, 0, 0);
	  MessageBox(0, message, TEXT("GetLastError"), MB_OK | MB_ICONINFORMATION);
      
	  LocalFree(message);
	}
	n++;
	i++;
      }
      
      RegCloseKey(key);
    }
  }
}

HRESULT
SERVER::do_register() {
  TCHAR   szCLSID[MAX_PATH];
  LPWSTR  pwszShellExt;
  HRESULT ret = SELFREG_E_CLASS;

  if (StringFromIID(CLSID_DIFF_EXT, &pwszShellExt) == S_OK) {
    wcstombs(szCLSID, pwszShellExt, ARRAYSIZE(szCLSID));
    CoTaskMemFree((void*)pwszShellExt);
    
    TCHAR    szSubKey[MAX_PATH];
    TCHAR    szModule[MAX_PATH];
    HKEY     hKey;
    LRESULT  lResult = NOERROR;
    DWORD    dwDisp;

    GetModuleFileName(SERVER::instance()->handle(), szModule, ARRAYSIZE(szModule));
  
    REGSTRUCT ShExClsidEntries[] = {
      {TEXT("CLSID\\%s"), 0, TEXT(SHELLEXNAME)},
      {TEXT("CLSID\\%s\\InProcServer32"), 0, TEXT("%s")},
      {TEXT("CLSID\\%s\\InProcServer32"), TEXT("ThreadingModel"), TEXT("Apartment")}};
  
    for(unsigned int i = 0; (i < ARRAYSIZE(ShExClsidEntries)) && (lResult == NOERROR); i++) {
      wsprintf(szSubKey, ShExClsidEntries[i].subkey, szCLSID);
      lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT, szSubKey, 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hKey, &dwDisp);
    
      if(lResult == NOERROR) {
        TCHAR szData[MAX_PATH];

        wsprintf(szData, ShExClsidEntries[i].value, szModule);
    
        lResult = RegSetValueEx(hKey, ShExClsidEntries[i].name, 0, REG_SZ, (LPBYTE)szData, lstrlen(szData) + 1);
      }
      
      RegCloseKey(hKey);
    }
    
    if(lResult == NOERROR) {  
      lResult = RegCreateKeyEx(HKEY_CLASSES_ROOT, TEXT("*\\shellex\\ContextMenuHandlers\\"SHELLEXNAME), 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hKey, &dwDisp);
    
      if(lResult == NOERROR) {
        TCHAR szData[MAX_PATH];
    
        //if necessary, create the value string
        wsprintf(szData, TEXT("%s"), szCLSID);
    
        lResult = RegSetValueEx(hKey, 0, 0, REG_SZ, (LPBYTE)szData, lstrlen(szData) + 1);
    
        RegCloseKey(hKey);
    
        //If running on NT, register the extension as approved.
        OSVERSIONINFO  osvi;
      
        osvi.dwOSVersionInfoSize = sizeof(osvi);
        GetVersionEx(&osvi);
      
        // NT needs to have shell extensions "approved".
        if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
          lstrcpy(szSubKey, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"));
      
          lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, szSubKey, 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, 0, &hKey, &dwDisp);
      
          if(lResult == NOERROR) {
            TCHAR szData[MAX_PATH];
      
            lstrcpy(szData, TEXT(SHELLEXNAME));
      
            lResult = RegSetValueEx(hKey, szCLSID, 0, REG_SZ, (LPBYTE)szData, lstrlen(szData) + 1);
      
            RegCloseKey(hKey);
            
            ret = S_OK;
          } else if (lResult == ERROR_ACCESS_DENIED) {
	    TCHAR msg[] = TEXT("Warning! You have unsuficient rights to write to a specific registry key.\n")
			  TEXT("The application may work anyway, but it is adsvised to register this module ")
			  TEXT("again while having administrator rights.");
	    
	    MessageBox(0, msg, TEXT("Warning"), MB_ICONEXCLAMATION);
	    
	    ret = S_OK;
          }
        }
        else {
          ret = S_OK;
        }
      }
    }
  }
  
  return ret;
}

HRESULT
SERVER::do_unregister() {
  TCHAR    szCLSID[MAX_PATH];
  LPWSTR   pwszShellExt;
  HRESULT ret = SELFREG_E_CLASS;

  if (StringFromIID(CLSID_DIFF_EXT, &pwszShellExt) == S_OK) {
    wcstombs(szCLSID, pwszShellExt, ARRAYSIZE(szCLSID));
    CoTaskMemFree(pwszShellExt);

    LRESULT  lResult = NOERROR;
    TCHAR    szSubKey[MAX_PATH];

    REGSTRUCT ShExClsidEntries[] = {
      {TEXT("CLSID\\%s\\InProcServer32"), 0, 0},
      {TEXT("CLSID\\%s"), 0, 0}};
  
    for(unsigned int i = 0; (i < ARRAYSIZE(ShExClsidEntries)) && (lResult == NOERROR); i++) {
      wsprintf(szSubKey, ShExClsidEntries[i].subkey, szCLSID);
      lResult = RegDeleteKey(HKEY_CLASSES_ROOT, szSubKey);
    }
  
    if(lResult == NOERROR) {
      lResult = RegDeleteKey(HKEY_CLASSES_ROOT, TEXT("*\\shellex\\ContextMenuHandlers\\"SHELLEXNAME));
    
      if(lResult == NOERROR) {
        //If running on NT, register the extension as approved.
        OSVERSIONINFO  osvi;
      
        osvi.dwOSVersionInfoSize = sizeof(osvi);
        GetVersionEx(&osvi);
      
        // NT needs to have shell extensions "approved".
        if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) {
          HKEY key; 
          
          RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Shell Extensions\\Approved"), 0, KEY_ALL_ACCESS, &key);
  
          lResult = RegDeleteValue(key, szCLSID);
        
          RegCloseKey(key);
        
          if(lResult == ERROR_SUCCESS) {
            ret = S_OK;
          }
        }
        else {
          ret = S_OK;
        }
      }
    }
  }
  
  return ret;
}
