#include <log/file_sink.h>
#include <log/log_message.h>

FILE_SINK::FILE_SINK(const char* name, unsigned int log_level) {
  _file = fopen(name, "a");
  _log_level = log_level;
}

FILE_SINK::~FILE_SINK() {
  fflush(_file);
  fclose(_file);
}

void 
FILE_SINK::debug(const LOG_MESSAGE& msg) {
  if(msg.log_level() < _log_level) {
    STRING m = msg.message();
    char* str = m;
    fprintf(_file, "DEBUG: %s\n", str);
    fflush(_file);
  }
}

void 
FILE_SINK::info(const LOG_MESSAGE& msg) {
  if(msg.log_level() < _log_level) {
    STRING m = msg.message();
    char* str = m;
    fprintf(_file, "INFO: %s", str);
    fflush(_file);
  }
}

void 
FILE_SINK::warning(const LOG_MESSAGE& msg) {
  if(msg.log_level() < _log_level) {
    STRING m = msg.message();
    char* str = m;
    fprintf(_file, "WARNING: %s\n", str);
    fflush(_file);
  }
}

void 
FILE_SINK::error(const LOG_MESSAGE& msg) {
  if(msg.log_level() < _log_level) {
    STRING m = msg.message();
    char* str = m;
    fprintf(_file, "ERROR: %s\n", str);
    fflush(_file);
  }
}

void 
FILE_SINK::failure(const LOG_MESSAGE& msg) {
  if(msg.log_level() < _log_level) {
    STRING m = msg.message();
    char* str = m;
    fprintf(_file, "FAILURE: %s\n", str);
    fflush(_file);
  }
}
