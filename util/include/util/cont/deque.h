/*
 * Copyright (c) 2003, Sergey Zorin. All rights reserved.
 *
 * This software is distributable under the BSD license. See the terms
 * of the BSD license in the LICENSE file provided with this software.
 *
 */

#ifndef __deque_h__
#define __deque_h__

#include <assert.h>

template <class T>
class DEQUE {
  public:
    class CURSOR {
        friend class DEQUE<T>;

      public:
        CURSOR(const CURSOR& c) {
          _current = c._current;
          _owner = c._owner;
        }

        CURSOR& operator=(const CURSOR& c) {
          _current = c._current;
          _owner = c._owner;

          return *this;
        }

        bool operator ==(const CURSOR& c) const {
          assert(compatible_with(c));
          return (compatible_with(c) && (_current == c._current));
        }

        bool operator !=(const CURSOR& c) const {
          return !(*this == c);
        }

        bool compatible_with(const CURSOR& c) const {
          return (_owner == c._owner);
        }

        CURSOR operator++() {
          _current++;
          if(_current >= _owner->_size)
            _current = 0;

          return *this;
        }

        CURSOR operator++(int) {
          CURSOR tmp = *this;

          ++(*this);

          return tmp;
        }

        const T& operator*() const {
          return _owner->item(*this);
        }

        bool done() const {
          return _owner->done(*this);
        }

        bool valid() const {
          return _owner->valid(*this);
        }

      private:
        CURSOR(const DEQUE<T>* d, unsigned int pos) {
          _current = pos;
          _owner = d;
        }

      private:
        unsigned int _current;
        const DEQUE<T>* _owner;
    };

    friend class DEQUE<T>::CURSOR;

    DEQUE(const DEQUE<T>& d) {
      _container = new T[d._size];
      _size = d._size;
      _begin = d._begin;
      _end = d._end;

      for(unsigned int i = 0; i < _size; i++) {
        _container[i] = d._container[i];
      }
    }

    DEQUE(unsigned int size=0) {
      _container = new T[size+1];
      _size = size+1;
      _begin = 0;
      _end = 1;
    }

    ~DEQUE() {
      delete [] _container;
    }

    DEQUE<T>& operator=(const DEQUE<T>& d) {
      delete [] _container;

      _container = new T[d._size];
      _size = d._size;
      _begin = d._begin;
      _end = d._end;

      for(unsigned int i = 0; i < _size; i++) {
        _container[i] = d._container[i];
      }

      return *this;
    }

    void clear() {
      _begin = 0;
      _end = 1;
    }

    bool full() const {
      return (_begin == _end);
    }

    bool empty() const {
      return (count() == 0);
    }

    unsigned int size() const {
      return _size-1;
    }

    unsigned int count() const {
      unsigned int size;
      if(_begin >= _end) {
        size = _size+_end-_begin-1;
      } else {
        size = _end-_begin-1;
      }

      return size;
    }

    DEQUE<T>::CURSOR begin() const {
      unsigned int begin;

      if(_begin == _size-1) {
        begin = 0;
      } else {
        begin = _begin+1;
      }

      return CURSOR(this, begin);
    }

    DEQUE<T>::CURSOR end() const {
      unsigned int end;

      if(_end == 0) {
        end = _size-1;
      } else {
        end = _end-1;
      }

      return CURSOR(this, end);
    }

    T& front() {
      assert(!empty());
      unsigned int begin;

      if(_begin == _size-1) {
        begin = 0;
      } else {
        begin = _begin+1;
      }

      return _container[begin];
    }

    const T& front() const {
      assert(!empty());
      unsigned int begin;

      if(_begin == _size-1) {
        begin = 0;
      } else {
        begin = _begin+1;
      }

      return _container[begin];
    }

    T& back() {
      assert(!empty());
      unsigned int end;

      if(_end == 0) {
        end = _size-1;
      } else {
        end = _end-1;
      }

      return _container[end];
    }

    const T& back() const {
      assert(!empty());
      unsigned int end;

      if(_end == 0) {
        end = size-1;
      } else {
        end = _end-1;
      }

      return _container[end];
    }

    void push_back(const T& d) {
      if(!full()) {
        _container[_end] = d;

        unsigned int new_end = _end+1;

        if(new_end >= _size) {
          new_end = 0;
        }

        _end = new_end;
      }
    }

    void push_front(const T& d) {
      if(!full()) {
        _container[_begin] = d;

        unsigned int new_begin;

        if(_begin == 0) {
          new_begin = _size-1;
        } else {
          new_begin = _begin-1;
        }

        _begin = new_begin;
      }
    }

    void pop_back() {
      unsigned int new_end;

      if(_end == 0) {
        new_end = _size-1;
      } else {
        new_end  = _end-1;
      }

      if(new_end != _begin) {
        _end = new_end;
      }
    }

    void pop_front() {
      unsigned int new_begin = _begin+1;

      if(new_begin >= _size) {
        new_begin = 0;
      }

      if(new_begin != _end) {
        _begin = new_begin;
      }
    }

    bool owner(const DEQUE<T>::CURSOR& c) const {
      return (c._owner == this);
    }

    bool valid(const DEQUE<T>::CURSOR& c) const {
      return !done(c);
    }

    void next(DEQUE<T>::CURSOR& c) const {
      assert(owner(c));
      assert(!done(c));
      c._current++;

      if(c._current == _size) {
        c._current = 0;
      }
    }

    void previous(DEQUE<T>::CURSOR& c) const {
      assert(owner(c));
      assert(!done(c));
      if(c._current == 0) {
        c._current = _size-1;
      } else {
        c._current--;
      }
    }

    bool done(const DEQUE<T>::CURSOR& c) const {
      assert(owner(c));
      return ((c._current == _begin) || (c._current == _end));
    }

    T& item(const DEQUE<T>::CURSOR& c) {
      assert(owner(c));
      assert(!done(c));
      return _container[c._current];
    }

    const T& item(const DEQUE<T>::CURSOR& c) const {
      assert(owner(c));
      assert(!done(c));
      return _container[c._current];
    }

  private:
    T* _container;
    unsigned int _size;
    unsigned int _begin;
    unsigned int _end;
};

#endif //__deque_h__
