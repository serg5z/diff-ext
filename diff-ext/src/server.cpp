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
