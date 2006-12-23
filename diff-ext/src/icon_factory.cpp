/*
 * Copyright (c) 2006 Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
#include <shlwapi.h>
#include "server.h"

#include "icon_factory.h"

class ICON_CHAIN;

// In general statics are bad, but here we have a ICON_FACTORY singleton,
// so it does not hurt to have all chains static vs. members of the ICON_FACTORY class
static ICON_CHAIN* _diff_chain = 0;
static ICON_CHAIN* _diff3_chain = 0;
static ICON_CHAIN* _diff_later_chain = 0;
static ICON_CHAIN* _diff_with_chain = 0;
static ICON_CHAIN* _diff3_with_chain = 0;
static ICON_CHAIN* _clear_chain = 0;

class ICON_CHAIN {
  public:
    ICON_CHAIN(ICON_CHAIN* next = 0) : _next(next) {
    }
    
    virtual ~ICON_CHAIN() {
      if(_next != 0) {
        delete _next;
      }
    }
    
    virtual HICON icon() = 0;
  
  protected:
    HICON next() {
      HICON result = 0;
      
      if(_next != 0) {
        result = _next->icon();
      }
      
      return result;
    }
    
  private:
    ICON_CHAIN* _next;
};

class FILE_ICON_CHAIN : public ICON_CHAIN {
  public:    
    FILE_ICON_CHAIN(LPTSTR file_name, ICON_CHAIN* next) : ICON_CHAIN(next)  {
      _file_name = StrDup(file_name);
    }
    
    virtual ~FILE_ICON_CHAIN() {
      LocalFree(_file_name);
    }
    
    virtual HICON icon() {
      HICON result = (HICON)LoadImage(0, _file_name, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTCOLOR);
//      HICON result = ExtractIcon(SERVER::instance()->handle(), _file_name, 0);
      
      if(result == 0) {
        result = next();
      }
      
      return result;
    }
    
  private:
    LPTSTR _file_name;
};

class RESOURCE_ICON_CHAIN : public ICON_CHAIN {
  public:
    RESOURCE_ICON_CHAIN(LPTSTR library_name, int resource, ICON_CHAIN* next) : ICON_CHAIN(next) {
      if(library_name != 0) {
        _own_module_instance = false;
        _module = LoadLibrary(library_name);
      } else {
        _own_module_instance = false;
      }
      
      _resource = resource;
    }
    
    RESOURCE_ICON_CHAIN(HINSTANCE module, int resource, ICON_CHAIN* next, bool own_module = false) : ICON_CHAIN(next), _own_module_instance(own_module) {
      _module = module;
      _resource = resource;
    }
    
    virtual ~RESOURCE_ICON_CHAIN() {
      if(_own_module_instance) {
        FreeLibrary(_module);
      }
    }
    
    virtual HICON icon() {
      HICON result = (HICON)LoadImage(_module, MAKEINTRESOURCE(_resource), IMAGE_ICON, 16, 16, LR_SHARED | LR_DEFAULTCOLOR);

      if(result == 0) {
        result = next();
      }
      
      return result;
    }
    
  private:
    bool _own_module_instance;
    HINSTANCE _module;
    int _resource;
};

ICON_FACTORY::ICON_FACTORY() {
//  HINSTANCE shell32 = LoadLibrary(TEXT("shell32"));    
  TCHAR home[MAX_PATH];
  TCHAR* end;
/*    
  _diff_later_chain = new RESOURCE_ICON_CHAIN(shell32, 22, _diff_later_chain, true);
  _diff_later_chain = new RESOURCE_ICON_CHAIN(shell32, 137, _diff_later_chain);
  _diff_later_chain = new RESOURCE_ICON_CHAIN(shell32, 321, _diff_later_chain);
  _diff_later_chain = new RESOURCE_ICON_CHAIN(shell32, 330, _diff_later_chain);
  
  _clear_chain = new RESOURCE_ICON_CHAIN(shell32, 240, _clear_chain);
*/  
  GetModuleFileName(SERVER::instance()->handle(), home, sizeof(home)/sizeof(home[0]));
  end = _tcsrchr(home, TEXT('\\'));

  lstrcpy(end, TEXT("\\icons\\diff.ico"));  
  _diff_chain = new FILE_ICON_CHAIN(home, _diff_chain);

  lstrcpy(end, TEXT("\\icons\\diff3.ico"));  
  _diff3_chain = new FILE_ICON_CHAIN(home, _diff3_chain);

  lstrcpy(end, TEXT("\\icons\\diff_later.ico"));  
  _diff_later_chain = new FILE_ICON_CHAIN(home, _diff_later_chain);

  lstrcpy(end, TEXT("\\icons\\diff_with.ico"));  
  _diff_with_chain = new FILE_ICON_CHAIN(home, _diff_with_chain);

  lstrcpy(end, TEXT("\\icons\\diff3_with.ico"));  
  _diff3_with_chain = new FILE_ICON_CHAIN(home, _diff3_with_chain);

  lstrcpy(end, TEXT("\\icons\\clear.ico"));  
  _clear_chain = new FILE_ICON_CHAIN(home, _clear_chain);
}

ICON_FACTORY::~ICON_FACTORY() {
}

HICON 
ICON_FACTORY::diff_icon() {
  return _diff_chain->icon();
}

HICON 
ICON_FACTORY::diff3_icon() {
  return _diff3_chain->icon();
}

HICON 
ICON_FACTORY::diff_later_icon() {
  return _diff_later_chain->icon();
}

HICON 
ICON_FACTORY::diff_with_icon() {
  return _diff_with_chain->icon();
}

HICON 
ICON_FACTORY::diff3_with_icon() {
  return _diff3_with_chain->icon();
}

HICON
ICON_FACTORY::clear_icon() {
  return _clear_chain->icon();
}

ICON_FACTORY* 
ICON_FACTORY::instance() {
  static ICON_FACTORY* instance = 0;
  
  if(instance == 0) {
    instance = new ICON_FACTORY;
  }
  
  return instance;
}
