/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms of the
 * BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __log_h__
#define __log_h__

#include <util/string.h>
#include <util/cont/list.h>

class LOG_SINK;
class LOG_MESSAGE;

class LOG {
  typedef void (LOG_SINK::*log_function)(const STRING&) const;
  
  public:
    LOG* instance() {
      static LOG* instance = 0;
      
      if(instance == 0) {
	instance = new LOG;
      }
      
      return instance;
    }
    
    void info(const LOG_MESSAGE& msg) const;
    void warning(const LOG_MESSAGE& msg) const;
    void error(const LOG_MESSAGE& msg) const;
    void faluire(const LOG_MESSAGE& msg) const;
    
  private:
    LOG() {}
    LOG(const LOG&) {}
      
    void write_message(log_function func, const STRING& msg) const;
      
    LIST<LOG_SINK*> 
};

#endif __log_h__