/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms of the
 * BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __file_sink__
#define __file_sink__

#include <stdio.h>
#include <tchar.h>

#include <log/log_sink.h>

class LOG_MESSAGE;

class FILE_SINK : public LOG_SINK {
  public:
    FILE_SINK(const TCHAR* name, unsigned int log_level = 0);
    virtual ~FILE_SINK();
  
    virtual void debug(const LOG_MESSAGE& msg);
    virtual void info(const LOG_MESSAGE& msg);
    virtual void warning(const LOG_MESSAGE& msg);
    virtual void error(const LOG_MESSAGE& msg);
    virtual void failure(const LOG_MESSAGE& msg);
  
  private:
    FILE* _file;
    unsigned int _log_level;
};

#endif // __file_sink__
