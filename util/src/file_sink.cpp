#include <log/file_sink.h>
#include <log/log_message.h>

FILE_SINK::FILE_SINK(const TCHAR* name, unsigned int log_level) {
  _file = _tfopen(name, TEXT("a"));
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
    TCHAR* str = m;
    _ftprintf(_file, TEXT("DEBUG(%d): %s\n"), msg.log_level(), str);
    fflush(_file);
  }
}

void 
FILE_SINK::info(const LOG_MESSAGE& msg) {
  if(msg.log_level() < _log_level) {
    STRING m = msg.message();
    TCHAR* str = m;
    _ftprintf(_file, TEXT("INFO(%d): %s"), msg.log_level(), str);
    fflush(_file);
  }
}

void 
FILE_SINK::warning(const LOG_MESSAGE& msg) {
  if(msg.log_level() < _log_level) {
    STRING m = msg.message();
    TCHAR* str = m;
    _ftprintf(_file, TEXT("WARNING(%d): %s\n"), msg.log_level(), str);
    fflush(_file);
  }
}

void 
FILE_SINK::error(const LOG_MESSAGE& msg) {
  if(msg.log_level() < _log_level) {
    STRING m = msg.message();
    TCHAR* str = m;
    _ftprintf(_file, TEXT("ERROR(%d): %s\n"), msg.log_level(), str);
    fflush(_file);
  }
}

void 
FILE_SINK::failure(const LOG_MESSAGE& msg) {
  if(msg.log_level() < _log_level) {
    STRING m = msg.message();
    TCHAR* str = m;
    _ftprintf(_file, TEXT("FAILURE: %s\n"), str);
    fflush(_file);
  }
}
