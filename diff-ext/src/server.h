#ifndef __server_h__
#define __server_h__

#include <util/cont/deque.h>
#include <util/string.h>

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
    SERVER() : _refference_count(0), _recent_files(0) {}
    SERVER(const SERVER&) {}
      
  private:
    LONG _refference_count;
    DEQUE<STRING>* _recent_files;
};

#endif // __server_h__
