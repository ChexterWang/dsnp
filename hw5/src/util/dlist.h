/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList(): _isSorted(false) {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () {
        _node = _node->_next;
        return *this;
      }
      iterator operator ++ (int) {
        iterator tmp(*this);
        _node = _node->_next;
        return tmp;
      }
      iterator& operator -- () {
        _node = _node->_prev;
        return *this;
      }
      iterator operator -- (int) {
        iterator tmp(*this);
        _node = _node->_prev;
        return tmp;
      }

      iterator& operator = (const iterator& i) {
         _node = i._node;
         return *this;
      }

      bool operator != (const iterator& i) const {
         if(i._node != _node) return true;
         else return false;
      }
      bool operator == (const iterator& i) const {
         if(i._node == _node) return true;
         else return false;
      }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return _head; }
   iterator end() const { return _head->_prev; }
   bool empty() const {
      if(_head == _head->_prev) return true;
      else return false;
   }
   size_t size() const {
      size_t s = 0;
      for(DListNode<T>* now = _head; now != _head->_prev;
         now = now->_next) ++s;
      return s;
   }
   void push_back(const T& x) {
      DListNode<T>* tmp = new DListNode<T>(x, _head->_prev->_prev, _head->_prev);
      if(_head != _head->_prev){
         _head->_prev->_prev = tmp;
         tmp->_prev->_next = tmp;
      }
      else _head = _head->_next = _head->_prev = tmp;
      _isSorted = false;
   }
   void pop_front() {
      if(size()){
          DListNode<T>* tmp = _head->_next;
          _head->_prev->_next = tmp;
          tmp->_prev = _head->_prev;
          delete _head;
          _head = tmp;
      }
   }
   void pop_back() {
     if(size() > (size_t) 1){
       // tmp = last elem after pop
       DListNode<T>* last = _head->_prev->_prev->_prev;
       delete last->_next;
       last->_next = _head->_prev;
       _head->_prev->_prev = last;
     }
     else if(size() == (size_t) 1) pop_front();
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      if(empty() || pos == end()) return false;
      else if(pos == begin()) pop_front();
      else if(pos == --end()) pop_back();
      else{
         pos._node->_prev->_next = pos._node->_next;
         pos._node->_next->_prev = pos._node->_prev;
         delete pos._node;
      }
      return true;
   }
   bool erase(const T& x) {
      if(erase(find(x))) return true;
      else return false;
   }

   iterator find(const T& x) {
      for(iterator it = begin(); it != end(); ++it){
         if(*it == x) return it;
      }
      return end();
   }

   void clear() {
      DListNode<T>* tmp = _head;
      for(; tmp != _head->_prev; tmp = tmp->_next)
         delete tmp;
      _head = tmp;
      _head->_prev = _head->_next = _head;
   }  // delete all nodes except for the dummy node

   void sort() const {
      if(_isSorted) return;
      for(iterator it = begin(); it != end(); ++it){
         iterator tmp = it;
         for(iterator itt = it; itt != end(); ++itt)
            tmp = (*itt < *tmp) ? itt : tmp;
         std::swap(it._node->_data, tmp._node->_data);
      }
      _isSorted = true;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H
