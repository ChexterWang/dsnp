/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

// use for print -verbose
#ifndef PF
#define PF 8
#endif
#ifndef LEAF
#define LEAF true
#endif

#include <vector>
#include <stack>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   BSTreeNode(const T& d, BSTreeNode<T>* r = NULL, BSTreeNode<T>* l = NULL)
   : data(d), _right(r), _left(l) {}
   T                 data;
   BSTreeNode<T>*    _right;
   BSTreeNode<T>*    _left;
};

template <class T>
class traceNode
{
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   traceNode(BSTreeNode<T>* n, char d): tnode(n), dir(d){}
   BSTreeNode<T>*    tnode;
   char              dir;
   bool operator != (const traceNode& i) const{
      return ((tnode != i.tnode) || (dir != i.dir)) ? true : false;
   }
   bool operator == (const traceNode& i) const{
      return ((tnode == i.tnode) && (dir == i.dir)) ? true : false;
   }
};

template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree(){
      _root = _end = new BSTreeNode<T>(T());
      _size = 0;
   }
   ~BSTree(){}

   class iterator{
      friend class BSTree;
   public:
      iterator(){}
      iterator(BSTreeNode<T>* n) : _node(n) {}
      iterator(int i) : _node(0) {}
      iterator(const iterator& i) : _trace(i._trace), _node(i._node) {}
      ~iterator(){}
      const T& operator * () const { return _node->data; }
      T& operator * (){ return _node->data; }
      iterator& operator ++ (){ next(); return *this; }
      iterator operator ++ (int){ iterator tmp(*this); next(); return tmp; }
      iterator& operator -- (){ prev(); return *this; }
      iterator operator -- (int){ iterator tmp(*this); prev(); return tmp; }
      iterator& operator = (const iterator& i){ _trace=i._trace; _node=i._node; return *this; }
      bool operator != (const iterator& i) const {
         if(_trace.size() && i._trace.size()) return ( _trace.top()!=i._trace.top() )?true:false;
         else return ( _node != i._node )?true:false;
      }
      bool operator == (const iterator& i) const {
         if(_trace.size() && i._trace.size()) return ( _trace.top()==i._trace.top() )?true:false;
         else return ( _node == i._node )?true:false;
      }
   private:
      stack<traceNode<T> > _trace;
      BSTreeNode<T>*    _node;
      void addTrace(BSTreeNode<T>* n, char d){
         _trace.push(move(traceNode<T>(n, d)));
      }
      void next(){
         if(_node->_right){
            addTrace(_node, '>');
            _node = _node->_right;
            for(; _node->_left; _node = _node->_left) addTrace(_node, '<');
         }
         else {
            while(_trace.size() && _trace.top().dir == '>') _trace.pop();
            _node = _trace.top().tnode;
            _trace.pop();
         }
      }
      void prev(){
         if(_node->_left){
            addTrace(_node, '<');
            _node = _node->_left;
            for(; _node->_right; _node = _node->_right) addTrace(_node, '>');
         }
         else {
            while(_trace.size() && _trace.top().dir == '<') _trace.pop();
            if(_trace.size()){ _node = _trace.top().tnode; _trace.pop(); }
            else _node = NULL;
         }
      }

   };

   iterator begin() const {
      if(!_size) return 0;
      iterator it(_root);
      for(; it._node->_left; it._node = it._node->_left)
         it.addTrace(it._node, '<');
      return it;
   }
   iterator end() const {
      if(!_size) return 0;
      iterator it(_root);
      for(; it._node->_right; it._node = it._node->_right)
         it.addTrace(it._node, '>');
      return it;
   }
   void insert(const T& x){
      if(!_size){ _root = new BSTreeNode<T>(x, _end); ++_size; return; }
      iterator it(_root);
      bsfindcandy(_root, x, it);
      if(it._node->data > x)
         it._node->_left = new BSTreeNode<T>(x);
      else if((it._node->data == x) || (it == --end()))
         it._node->_right = new BSTreeNode<T>(x, it._node->_right);
      else it._node->_right = new BSTreeNode<T>(x);
      ++_size;
   }
   iterator find(const T& x) {
      if(!_size) return end();
      iterator it(_root);
      bsfindcandy(_root, x, it);
      if(it._node->data == x) return it;
      return end();
   }
   void clear(){
      deltree(_root);
      _root = _end;
      _size = 0;
   }
   bool erase(iterator pos){
      if( !_size || (pos == end()) ) return false;
      if(!pos._node->_left && !pos._node->_right){
         if(pos._trace.top().dir == '<')
            pos._trace.top().tnode->_left = NULL;
         else pos._trace.top().tnode->_right = NULL;
      }
      else if(!pos._node->_left && pos._node->_right){
         if(!pos._trace.size())
            _root = pos._node->_right;
         else if(pos._trace.top().dir == '>')
            pos._trace.top().tnode->_right = pos._node->_right;
         else pos._trace.top().tnode->_left = pos._node->_right;
      }
      else if(pos._node->_left && !pos._node->_right){
         if(pos._trace.top().dir == '<')
            pos._trace.top().tnode->_left = pos._node->_left;
         else pos._trace.top().tnode->_right = pos._node->_left;
      }
      else if(pos != --end()){
         iterator origin = pos++;
         origin._node->data = pos._node->data;
         if(!pos._trace.size() || (pos._trace.top().dir == '>'))
            origin._node->_right = pos._node->_right;
         else pos._trace.top().tnode->_left = pos._node->_right;
      }
      else if((--pos)._trace.size() && pos._trace.top().dir == '<'){
         iterator prev = pos++;
         pos._node->data = prev._node->data;
         pos._node->_left = prev._node->_left;
         pos._node = prev._node;
      }
      else{
         iterator prev = pos++;
         prev._node->_right = _end;
         if(pos._trace.size())
            pos._trace.top().tnode->_right = pos._node->_left;
         else _root = pos._node->_left;
      }
      delete pos._node;
      --_size;
      if(!_size) _end = _root;
      return true;
   }
   void pop_front(){ erase(begin()); }
   void pop_back(){ erase(--end()); }
   bool erase(const T& x){ return (erase(find(x))) ? true : false; }
   void print() const { bsprint(_root, 0, 'o'); }
   void sort() const {}
   bool empty() const { return (_size) ? false : true; }
   size_t size() const { return _size; }

private:
   BSTreeNode<T>*    _root;
   BSTreeNode<T>*    _end;
   size_t            _size;
   void bsfindcandy(BSTreeNode<T>*& node, const T& x, iterator& it){
      if( (node->_right) && (node->_right != _end) &&
          ((x == node->_right->data) || (x > node->data)) ){
         it.addTrace(node, '>');
         bsfindcandy(node->_right, x, it);
      }
      else if((node->_left) && (x < node->data)){
         it.addTrace(node, '<');
         bsfindcandy(node->_left, x, it);
      }
      else it._node = node;
   }
   void deltree(BSTreeNode<T>*& node){
      if(node == _end) return;
      if(node->_left) { deltree(node->_left); node->_left = NULL; }
      if(node->_right) { deltree(node->_right); node->_right = NULL; }
      delete node;
   }
   void bsprint(BSTreeNode<T>* node, size_t tab, char c) const {
      if(node->_right) bsprint(node->_right, tab+1, '/');
      else if(LEAF)    cout << setw(PF*(tab+1)) << '/'  << "----" <<    '$'     << endl;
      if(node == _end) cout << setw(PF*tab)     <<  c   << "----" << "><(((˚>˚" << endl;
      else             cout << setw(PF*tab)     <<  c   << "----" << node->data << endl;
      if(node->_left)  bsprint(node->_left, tab+1,  '\\');
      else if(LEAF)    cout << setw(PF*(tab+1)) << '\\' << "----" <<    '$'     << endl;
   }

};

#endif // BST_H
