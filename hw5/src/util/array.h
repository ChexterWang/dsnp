/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>
#include <type_traits>
#include <iostream>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0), _isSorted(false) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*_node); }
      T& operator * () { return (*_node); }
      iterator& operator ++ () {
         ++_node;
         return (*this);
      }
      iterator operator ++ (int) {
         iterator tmp(*this);
         ++_node;
         return tmp;
      }
      iterator& operator -- () {
         --_node;
         return (*this);
      }
      iterator operator -- (int) {
         iterator tmp(*this);
         --_node;
         return tmp;
      }

      iterator operator + (int i) const {
         iterator tmp(*this);
         tmp._node += i;
         return tmp;
      }
      iterator& operator += (int i) {
         _node += i;
         return (*this);
      }

      iterator& operator = (const iterator& i) {
         _node = i._node;
         return *this;
      }

      bool operator != (const iterator& i) const {
         (_node != i._node)?({return true;}):({return false;});
      }
      bool operator == (const iterator& i) const {
         (_node == i._node)?({return true;}):({return false;});
      }

   private:
      T*    _node;
   };

   // TODO: implement these functions
   iterator begin() const {
      if(_capacity) return _data;
      else return 0;
   }
   iterator end() const {
      if(_capacity) return _data + _size;
      else return 0;
   }
   bool empty() const {
      if(_size) return false;
      else return true;
   }
   size_t size() const { return _size; }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
      if(_size == _capacity) expand();
      _data[_size] = x;
      ++_size;
      _isSorted = false;
   }
   void pop_front() {
      if(_size){
         if(_size-1) _data[0] = _data[_size-1];
         --_size;
         _isSorted = false;
      }
   }
   void pop_back() {
      if(_size) --_size;
   }

   bool erase(iterator pos) {
      if(empty() || pos == end()) return false;
      if(_size-1) *pos = _data[_size-1];
      --_size;
      if(pos != begin()) _isSorted = false;
      return true;
   }
   bool erase(const T& x) {
      if(erase(find(x))) return true;
      else return false;
   }

   iterator find(const T& x) {
      if(_size && _isSorted){
         int t = es(x);
         for(;t > 0; --t){
            if(_data[t-1] != x) break;
         }
         if(t != -1) return (_data + t);
      }
      else if(_size){
         for(iterator it = begin(); it != end(); ++it){
            if(*it == x) return it;
         }
      }
      return end();
   }

   void clear() {
      _size = 0;
      _isSorted = false;
   }

   // [Optional TODO] Feel free to change, but DO NOT change ::sort()
   void sort() const {
      if (!empty() && !_isSorted) ::sort(_data, _data+_size);
      _isSorted = true;
   }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
   void expand(){
      if(!_capacity){
         _capacity = 1;
         _data = new T[1];
      }
      else{
         _capacity *= 2;
         T* tmp = new T[_capacity];
         for(size_t i = 0; i < _size; ++i)
            tmp[i] = _data[i];
         delete []_data;
         _data = tmp;
      }
      // malloc/free for T that store pointer and calling new?
   }

   int bs(size_t b, size_t e, const T& x){
      if(e >= b){
         int mid = b + (e-b)/2;
         if(_data[mid] == x) return mid;
         if(_data[mid] < x) return bs(mid+1, e, x);
         return bs(b, mid-1, x);
      }
      return -1;
   }

   int es(const T& x){
      if(_data[0] == x) return 0;
      size_t a = 1;
      while(a < _size && _data[a] <= x){
         a *= 2;
      }
      return bs(a/2, min(a, _size), x);
   }

};

#endif // ARRAY_H
