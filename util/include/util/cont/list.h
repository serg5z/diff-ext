/*
 * Copyright (c) 2003-2004, Sergey Zorin. All rights reserved.
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
        friend class LIST<T>::ITERATOR;
      
      private:
        NODE() : _next(0) {
        }
      
        NODE(T data, typename LIST<T>::NODE* next) : _data(data), _next(next) {
        }

        void link(typename LIST<T>::NODE* next) {
          _next = next;
        }

        typename LIST<T>::NODE* next() {
          return _next;
        }

        const typename LIST<T>::NODE* next() const {
          return _next;
        }

      public:
        T data() {
          return _data;
        }

      private:
        T _data;
        typename LIST<T>::NODE* _next;
    };
    
    class ITERATOR {
      private:
        friend class LIST<T>;
          
      public:
        ITERATOR(LIST<T>& list) {
          _current = list._head.next();
          _owner = &list;
        }

        ITERATOR(const LIST<T>::ITERATOR& c) {
          _current = c._current;
          _owner = c._owner;
        }

        ITERATOR& operator=(const LIST<T>::ITERATOR& c) {
          _current = c._current;
          _owner = c._owner;

          return *this;
        }

        bool operator==(const LIST<T>::ITERATOR& c) const {
          assert(compatible_with(c));
          return (compatible_with(c) && (_current == c._current));
        }

        bool operator !=(const LIST<T>::ITERATOR& c) const {
          return !(*this == c);
        }

        bool compatible_with(const LIST<T>::ITERATOR& c) const {
          return (_owner == c._owner);
        }

        typename LIST<T>::ITERATOR operator++() {
          _current = _current->next();

          return *this;
        }

        typename LIST<T>::ITERATOR operator++(int) {
          ITERATOR tmp = *this;

          ++(*this);

          return tmp;
        }

        typename LIST<T>::ITERATOR operator--() {
          typename LIST<T>::NODE* tmp = _current;
          _current = _owner._head.next();
          
          while(_current->next() != tmp) {
            _current = _current->next();
          }

          return *this;
        }

        typename LIST<T>::ITERATOR operator--(int) {
          ITERATOR tmp = *this;

          --(*this);

          return tmp;
        }

        const typename LIST<T>::NODE* operator*() const {
          return _current;
        }

        typename LIST<T>::NODE* operator*() {
          return _current;
        }

        bool done() const {
          return (_current == &(_owner->_head));
        }

        bool valid() const {
          return true; // TODO: make sure that this cursos points to a list element
        }

      private:
        typename LIST<T>::NODE* _current;
        LIST<T>* _owner;
    };
    
  private:
    friend class LIST<T>::ITERATOR;
          
  public:
    LIST() {
      _head.link(&_head);
      _count = 0;
    }
    
    void prepend(T data) {
      insert(&_head, data);
    }
    
    void prepend(LIST<T>::NODE* node) {
      insert(&_head, node);
    }
    
    void append(T data) {
      typename LIST<T>::NODE* current = &_head;
      
      while(current->next() != &_head) {
        current = current->next();
      }
      
      insert(current, data);
    }
    
    void append(LIST<T>::NODE* node) {
      typename LIST<T>::NODE* current = &_head;
      
      while(current->next() != &_head) {
        current = current->next();
      }
      
      insert(current, node);
    }
    
    void insert(LIST<T>::NODE* position, T data) {
      typename LIST<T>::NODE* node = new NODE(data, position->next());
      insert(position, node);
    }
    
    void insert(LIST<T>::NODE* position, LIST<T>::NODE* node) {
      node->link(position->next());
      position->link(node);
      _count++;
    }
    
    void remove(LIST<T>::NODE* node) {
      unlink(node);
      delete node;
    }
    
    void unlink(LIST<T>::NODE* node) {
      assert(_count > 0);
      assert(node != &_head);
      typename LIST<T>::NODE* current = &_head;
      bool found = true;
      
      while((current->next() != node) && found) {
        current = current->next();
        found = (current != &_head);
      }
      
      assert(found);
      
      if(found) {
        current->link(current->next()->next());
        _count--;
      }
    }
    
    bool empty() const {
      return (_head.next() == &_head);
    }
    
    void clear() {
      typename LIST<T>::NODE* current;
      
      while((current = _head.hext()) != &_head) {
        remove(current);
      }
      
      _count = 0;
    }
    
    unsigned int count() const {
      return _count;
    }
    
  private:
    typename LIST<T>::NODE _head;
    unsigned int _count;
};

#endif // __list_h__
