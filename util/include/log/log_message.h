/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms of the
 * BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __log_message_h__
#define __log_message_h__

#include <util/string.h>

class LOG_MESSAGE {
  public:
    LOG_MESSAGE(STRING message, unsigned int log_level = 0) : _message(message), _log_level(log_level) {
    }
    
    LOG_MESSAGE(const LOG_MESSAGE& msg) : _message(msg._message), _log_level(msg._log_level) {
    }
    
    LOG_MESSAGE& operator=(const LOG_MESSAGE& msg) {
      _message = msg._message;
      _log_level = msg._log_level;
      
      return *this;
    }
    
    unsigned int log_level() const {
      return _log_level;
    }
    
    STRING message() const {
      return _message;
    }
    
  private:
    unsigned int _log_level;
    STRING _message;
};

#endif // __log_message_h__
