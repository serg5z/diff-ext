/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __string_h__
#define __string_h__

/*
 * $Id$
 */

#include <string.h>
#include <stdlib.h>

class STRING {
  public:
    STRING(const STRING& s) {
      _str = new char[s.length()+1];
      strcpy(_str, s);
    }
    
    STRING(const char* str = "") {
      _str = new char[strlen(str)+1];
      strcpy(_str, str);
    }
    
    ~STRING() {
      delete[] _str;
    }
    
    STRING& operator=(const STRING& s) {
      delete[] _str;
      _str = new char[s.length()+1];
      strcpy(_str, s);
      return *this;
    }
    
    operator char*() {
      return _str;
    }
    
    operator const char*() const {
      return _str;
    }
    
    unsigned int length() const {
      return strlen(_str);
    }
    
    bool operator ==(const STRING& s) const {
      return (strcmp(_str, s) == 0);
    }
    
    STRING& operator +=(const STRING& s) {
      char* str = new char[strlen(_str)+s.length()+1];
		  
      strcpy(str, _str);
      strcat(str, s);
      
      delete[] _str;
      
      _str = str;
      
      return *this;
    }
    
  private:
    char* _str;
};

STRING operator+(const STRING& s1, const STRING& s2);

#endif // __string_h__
