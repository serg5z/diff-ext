#ifndef __server_h__
#define __server_h__

#include <util/cont/list.h>
#include <util/string.h>
#include <log/file_sink.h>

#include <windows.h>

class SERVER {
  public:
    static SERVER* instance() {
      if(_instance == 0) {
	_instance = new SERVER();
        MessageBox(0, TEXT("new SERVER"), TEXT("info"), MB_OK);
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
    
    LIST<STRING>* recent_files();
    
    void save_history() const;
  
  private:
    SERVER();
    SERVER(const SERVER&) {}
      
  private:
    LONG _refference_count;
    LIST<STRING>* _recent_files;
    FILE_SINK* _file_sink;
    static SERVER* _instance;
};

#endif // __server_h__
