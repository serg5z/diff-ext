/*
 * Copyright (c) 2003-2005, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
#ifndef __server_h__
#define __server_h__

#include <util/cont/dlist.h>
#include <util/string.h>
#include <log/file_sink.h>

#include <windows.h>

class SERVER {
  public:
    static SERVER* instance() {
      if(_instance == 0) {
	_instance = new SERVER();
//        MessageBox(0, TEXT("new SERVER"), TEXT("info"), MB_OK);
      }
      
      return _instance;
    }
  
  public:
    virtual ~SERVER();
  
    HINSTANCE handle() const;
  
    HRESULT do_register();
    HRESULT do_unregister();
  
    void lock();
    void release();
  
    ULONG refference_count() const {
      return _refference_count;
    }
    
    DLIST<STRING>* recent_files();
    
    void save_history() const;
    
    unsigned int history_size() const {
      return _history_size;
    }
  
  private:
    SERVER();
    SERVER(const SERVER&) {}
      
  private:
    LONG _refference_count;
    DLIST<STRING>* _recent_files;
    unsigned int _history_size;
    FILE_SINK* _file_sink;
    static SERVER* _instance;
};

#endif // __server_h__
