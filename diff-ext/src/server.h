#ifndef __server_h__
#define __server_h__

class SERVER {
  public:
    static SERVER* instance() {
      static SERVER* instance = 0;
      
      if(instance == 0)
	instance = new SERVER();
      
      return instance;
    }
  
  public:
    HINSTANCE handle() const;
  
    HRESULT do_register();
    HRESULT do_unregister();
  
    void lock();
    void release();
  
    ULONG refference_count() const {
      return _refference_count;
    }
  
  private:
    SERVER() {}
    SERVER(const SERVER&) {}
      
  private:
    LONG _refference_count;
};

#endif // __server_h__
