/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */
#ifndef __trace_h__
#define __trace_h__

#include <tchar.h>
#include <stdio.h>

#include <log/log.h>
#include <log/log_message.h>

class TRACE {
  public:
    TRACE(const TCHAR* function, const TCHAR* file, unsigned int line, unsigned int level = 3) : _function(function), _file(file), _line(line), _level(level) {
      TCHAR* msg = _message;
      
      for(unsigned int i = 0; i < _indent; i++) {
	(*msg) = ' ';
	msg++;
      }
      
      _stprintf(msg, TEXT("->'%s' in %s:%d"), function, file, line);
      LOG::instance()->debug(LOG_MESSAGE(_message, level));
      
      _indent += 2;
    }
    
    ~TRACE() {
      TCHAR* msg = _message;
      _indent -= 2;
      
      for(unsigned int i = 0; i < _indent; i++) {
	(*msg) = ' ';
	msg++;
      }
      
      _stprintf(msg, TEXT("<-'%s' in %s:%d"), _function, _file, _line);
      LOG::instance()->debug(LOG_MESSAGE(_message, _level));
    }
    
  private:
    const TCHAR* _function;
    const TCHAR* _file;
    unsigned int _line;
    unsigned int _level;
    static TCHAR _message[1024];
    static unsigned int _indent;
};

#endif // __trace_h__
