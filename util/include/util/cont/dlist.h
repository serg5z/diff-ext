/*
 * Copyright (c) 2003,2004 Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __dlist_h__
#define __dlist_h__

#include <assert.h>

template <class T>
class DLIST {
  public:
    class NODE {
      private:
        friend class DLIST<T>;
      
      private:
        NODE(DLIST<T>::NODE* next = 0, DLIST<T>::NODE* previous = 0) : _next(next), _previous(previous) {
        }
        
        NODE(T data, DLIST<T>::NODE* next = 0, DLIST<T>::NODE* previous = 0) : _data(data), _next(next), _previous(previous) {
        }
        
        void append(DLIST<T>::NODE& to) {
          _next = to._next;
          _previous = &to;
          if(to._next != 0) {
            to._next->_previous = this;
          }
          to._next = this;
        }
	
        void append(DLIST<T>::NODE* to) {
          _next = to->_next;
          _previous = to;
          if(to->_next != 0) {
            to->_next->_previous = this;
          }
          to->_next = this;
        }
	
        void prepend(DLIST<T>::NODE& to) {
          _next = &to;
          _previous = to._previous;
          if(to._previous->_next != 0) {
            to._previous->_next = this;
          }
          to._previous = this;
        }
	
        void prepend(DLIST<T>::NODE* to) {
          _next = to;
          _previous = to->_previous;
          if(to->_previous->_next != 0) {
            to->_previous->_next = this;
          }
          to->_previous = this;
        }
        
        void unlink() {
          if(_next != 0) {
            _next->_previous = _previous;
          } 
          
          if(_previous != 0) {
            _previous->_next = _next;
          }
          
          _next = 0;
          _previous = 0;
        }
	
      public:
	T data() {
	  return _data;
	}
        
        DLIST<T>::NODE* next() {
          return _next;
        }
	
        const DLIST<T>::NODE* next() const {
          return _next;
        }
	
        DLIST<T>::NODE* previous() {
          return _previous;
        }
	
        const DLIST<T>::NODE* previous() const {
          return _previous;
        }
	
      private:
	T _data;
	DLIST<T>::NODE* _next;
	DLIST<T>::NODE* _previous;
    };
    
    class ITERATOR {
      private:
        friend class DLIST<T>;
          
      private:
        ITERATOR(DLIST<T>* list, DLIST<T>::NODE* node) {
          _current = node;
          _list = list;
        }

      public:
        ITERATOR(const DLIST<T>::ITERATOR& c) {
          _current = c._current;
          _list = c._list;
        }

        ITERATOR& operator=(const DLIST<T>::ITERATOR& c) {
          _current = c._current;
          _list = c._list;

          return *this;
        }

        bool operator==(const DLIST<T>::ITERATOR& c) const {
          return ((_list == c._list) && (_current == c._current));
        }

        bool operator !=(const DLIST<T>::ITERATOR& c) const {
          return !(*this == c);
        }

        DLIST<T>::ITERATOR operator++() {
          _current = _current->next();

          return *this;
        }

        DLIST<T>::ITERATOR operator++(int) {
          ITERATOR tmp = *this;

          ++(*this);

          return tmp;
        }

        DLIST<T>::ITERATOR operator--() {
          _current = _current->previous();

          return *this;
        }

        DLIST<T>::ITERATOR operator--(int) {
          ITERATOR tmp = *this;

          --(*this);

          return tmp;
        }

        const DLIST<T>::NODE* operator*() const {
          return _current;
        }

        DLIST<T>::NODE* operator*() {
          return _current;
        }
        
        bool done() const {
          return (_current == &(_list->_root));
        }

      private:
        DLIST<T>::NODE* _current;
        DLIST<T>* _list;
    };
    
  public:
    DLIST() {
      _count = 0;
      _root._next = &_root;
      _root._previous = &_root;
    }
    
    void append(T data) {
      append(tail(), data);
    }
    
    void append(DLIST<T>::NODE* node) {
      append(tail(), node);
    }
    
    void prepend(T data) {
      prepend(head(), data);
    }
    
    void prepend(DLIST<T>::NODE* node) {
      prepend(head(), node);
    }
    
    void append(DLIST<T>::ITERATOR pos, T data) {
      DLIST<T>::NODE* node = new NODE(data);
      
      node->append(*pos);
      
      _count++;
    }
    
    void prepend(DLIST<T>::ITERATOR pos, T data) {
      DLIST<T>::NODE* node = new NODE(data);
      
      node->prepend(*pos);
      
      _count++;
    }
    
    void append(DLIST<T>::ITERATOR pos, DLIST<T>::NODE* node) {
      assert(node != 0);
      node->unlink();
      node->append(*pos);
      
      _count++;
    }
    
    void prepend(DLIST<T>::ITERATOR pos, DLIST<T>::NODE* node) {
      assert(node != 0);
      node->unlink();
      node->prepend(*pos);
      
      _count++;
    }
    
    void unlink(DLIST<T>::ITERATOR& pos) {
      DLIST<T>::NODE* node = (*pos);
      pos++;
      node->unlink();
      _count--;
    }
    
    void remove(DLIST<T>::ITERATOR& pos) {
      DLIST<T>::NODE* node = (*pos);
      pos++;
      node->unlink();
      _count--;
      delete node;
    }
    
    unsigned int count() const {
      return _count;
    }
    
    bool empty() const {
      return (_root._next == &_root);
    }
    
    void clear() {
      DLIST<T>::ITERATOR i = head();
      
      while(!i.done()) {
        remove(i);
      }
    }
    
    DLIST<T>::ITERATOR head() {
      return DLIST<T>::ITERATOR(this, _root._next);
    }
    
    DLIST<T>::ITERATOR tail() {
      return DLIST<T>::ITERATOR(this, _root._previous);
    }
    
  private:
    NODE _root;
    unsigned int _count;
};

#endif // __list_h__
