/*
 * $Id$
 */

#include <util/string.h>

STRING
operator+(const STRING& s1, const STRING& s2) {
  STRING tmp(s1);
  
  tmp+=s2;
  
  return tmp;
}
