#include "log/log_message.h"
#include "log/log_sink.h"
#include "log/log.h"

void 
debug(const LOG_MESSAGE& msg) {
}

void 
info(const LOG_MESSAGE& msg) {
}

void 
warning(const LOG_MESSAGE& msg) {
}

void 
error(const LOG_MESSAGE& msg) {
}

void 
faluire(const LOG_MESSAGE& msg) {
}

void 
add_sink(LOG_SINK* sink) {
  _sinks.add(sink);
}

void 
remove_sink(LOG_SINK* sink) {
  LIST<int>::NODE* current = _sinks.begin();
  
  while(!_sinks.is_last(current)) {
    if(current->data() == sink) {
      LIST<int>::NODE* tmp = current;
      _sinks.remove(current);
      current = current->next();
      delete tmp;
    } else {
      current = current->next();
    }
  }  
}

void 
write_message(LOG_FUNCTION func, const STRING& msg) {
  LIST<int>::NODE* current = _sinks.begin();
  
  while(!_sinks.is_last(current)) {
    current->*func(msg);
    current = current->next();
  }  
}
