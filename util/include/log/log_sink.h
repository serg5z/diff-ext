/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms of the
 * BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __log_sink__
#define __log_sink__

class LOG_MESSAGE;

class LOG_SINK {
  public:
    virtual ~LOG_SINK() {
    }
  
    virtual void debug(const LOG_MESSAGE& msg) = 0;
    virtual void info(const LOG_MESSAGE& msg) = 0;
    virtual void warning(const LOG_MESSAGE& msg) = 0;
    virtual void error(const LOG_MESSAGE& msg) = 0;
    virtual void failure(const LOG_MESSAGE& msg) = 0;
};

#endif // __log_sink__
