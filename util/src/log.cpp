#include <log/log_message.h>
#include <log/log_sink.h>
#include <log/log.h>

void 
LOG::debug(const LOG_MESSAGE& msg) {
  write_message(&LOG_SINK::debug, msg);
}

void 
LOG::info(const LOG_MESSAGE& msg) {
  write_message(&LOG_SINK::info, msg);
}

void 
LOG::warning(const LOG_MESSAGE& msg) {
  write_message(&LOG_SINK::warning, msg);
}

void 
LOG::error(const LOG_MESSAGE& msg) {
  write_message(&LOG_SINK::error, msg);
}

void 
LOG::failure(const LOG_MESSAGE& msg) {
  write_message(&LOG_SINK::failure, msg);
}

void 
LOG::add_sink(LOG_SINK* sink) {
  _sinks.add(sink);
}

void 
LOG::remove_sink(LOG_SINK* sink) {
  LIST<LOG_SINK*>::NODE* current = _sinks.begin();
  
  while(!_sinks.is_last(current)) {
    if(current->data() == sink) {
      LIST<LOG_SINK*>::NODE* tmp = current;
      _sinks.remove(current);
      current = current->next();
      delete tmp;
    } else {
      current = current->next();
    }
  }  
}

void 
LOG::write_message(LOG_FUNCTION func, const LOG_MESSAGE& msg) {
  LIST<LOG_SINK*>::NODE* current = _sinks.begin();
  
  while(!_sinks.is_last(current)) {
    (current->data()->*func)(msg);
    current = current->next();
  }  
}
