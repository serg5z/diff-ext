#include <util/icon.h>

class ICON_REF_COUNTER {
  public:
    ICON_REF_COUNTER(HICON icon) : _icon(icon), _count(1) {
    }
    
    ~ICON_REF_COUNTER() {
      DestroyIcon(_icon);
    }
      
    HICON _icon;
    unsigned int _count;
};

ICON::ICON(const HICON& icon) {
  if(icon != 0) {
    _counter = new ICON_REF_COUNTER(icon);
  } else {
    _counter = 0;
  }
}

ICON::ICON(const ICON& icon) {
  acquire(icon._counter);
}

ICON::~ICON() {
  release();
}

ICON::operator HICON() const {
  HICON result = 0;
  
  if(_counter != 0) {
    result = _counter->_icon;
  }
  
  return result;
}

ICON& 
ICON::operator=(const ICON& icon) {
  if(&icon != this) {
    release();
    acquire(icon._counter);
  }
  
  return *this;
}

ICON& 
ICON::operator=(const HICON& icon) {
  if(_counter != 0) {
    if(icon != _counter->_icon) {
      release();
      if(icon != 0) {
        _counter = new ICON_REF_COUNTER(icon);
      } else {
        _counter = 0;
      }
    }
  }
  
  return *this;
}

void
ICON::acquire(ICON_REF_COUNTER* counter) {
  _counter = counter;
  if (counter) { 
    counter->_count++;
  }
}

void
ICON::release() {
  if (_counter != 0) {
    if (--_counter->_count == 0) {
      delete _counter;
    }
    
    _counter = 0;
  }
}
