#ifndef __trace_h__
#define __trace_h__

#include <stdio.h>

#include <log/log.h>
#include <log/log_message.h>

class TRACE {
  public:
    TRACE(const char* function, const char* file, unsigned int line, unsigned int level = 0) : _function(function), _file(file), _line(line), _level(level) {
      char* msg = _message;
      
      for(unsigned int i = 0; i < _indent; i++) {
	(*msg) = ' ';
	msg++;
      }
      
      sprintf(msg, "->'%s' in %s:%d", function, file, line);
      LOG::instance()->debug(LOG_MESSAGE(_message, level));
      
      _indent += 2;
    }
    
    ~TRACE() {
      char* msg = _message;
      _indent -= 2;
      
      for(unsigned int i = 0; i < _indent; i++) {
	(*msg) = ' ';
	msg++;
      }
      
      sprintf(msg, "<-'%s' in %s:%d", _function, _file, _line);
      LOG::instance()->debug(LOG_MESSAGE(_message, _level));
    }
    
  private:
    const char* _function;
    const char* _file;
    unsigned int _line;
    unsigned int _level;
    static char _message[1024];
    static unsigned int _indent;
};

#endif // __trace_h__
