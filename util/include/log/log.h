/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms of the
 * BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __log_h__
#define __log_h__

#include <util/cont/list.h>

class LOG_SINK;
class LOG_MESSAGE;

class LOG {
  typedef void (LOG_SINK::*LOG_FUNCTION)(const STRING&);
  
  public:
    LOG* instance() {
      static LOG* instance = 0;
      
      if(instance == 0) {
	instance = new LOG;
      }
      
      return instance;
    }
    
    void debug(const LOG_MESSAGE& msg);
    void info(const LOG_MESSAGE& msg);
    void warning(const LOG_MESSAGE& msg);
    void error(const LOG_MESSAGE& msg);
    void faluire(const LOG_MESSAGE& msg);
    
    void add_sink(LOG_SINK* sink);
    void remove_sink(LOG_SINK* sink);
    
  private:
    LOG() {}
    LOG(const LOG&) {}
      
    void write_message(LOG_FUNCTION func, const STRING& msg);
      
    LIST<LOG_SINK*> _sinks;
};

#endif // __log_h__