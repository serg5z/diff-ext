#ifndef __server_h__
#define __server_h__

#include <util/cont/deque.h>
#include <util/string.h>
#include <log/file_sink.h>

class SERVER {
  public:
    static SERVER* instance() {
      static SERVER* instance = 0;
      
      if(instance == 0)
	instance = new SERVER();
      
      return instance;
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
    
    DEQUE<STRING>* recent_files();
    
    void save_history() const;
  
  private:
    SERVER();
    SERVER(const SERVER&) {}
      
  private:
    LONG _refference_count;
    DEQUE<STRING>* _recent_files;
    FILE_SINK* _file_sink;
};

#endif // __server_h__
