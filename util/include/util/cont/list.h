/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __list_h__
#define __list_h__

#include <assert.h>

template <class T>
class LIST {
  public:
    class NODE {
      private:
        friend class LIST<T>;
      
      public:
	NODE() : _next(0) {
	}
      
	NODE(T data, NODE* next) : _data(data), _next(next) {
	}
	
	void link(NODE* next) {
	  _next = next;
	}
	
	NODE* next() {
	  return _next;
	}
	
	T data() {
	  return _data;
	}
	
      private:
	T _data;
	NODE* _next;
    };
    
  public:
    LIST() {
      _tail = &_head;
      _head.link(_tail);
      _count = 0;
    }
    
    void add(T data) {
      NODE* tmp = _tail;
      _tail = new NODE(data, &_head);
      tmp->link(_tail);
      _count++;
    }
    
    void remove(NODE* node) {
      assert(_count > 0);
      NODE* current = &_head;
      bool found = true;
      
      while((current->next() != node) && found) {
	current = current->next();
	if(current == &_head) {
	  found = false;
	}
      }
      
      if(found) {
	current->link(current->next()->next());
	_count--;
      }
    }
    
    NODE* begin() {
      return _head.next();
    }
    
    NODE* end() {
      return _tail;
    }
    
    bool is_last(NODE* node) const {
      return (node == &_head);
    }
    
  private:
    NODE _head;
    NODE* _tail;
    unsigned int _count;
};

#endif // __list_h__
