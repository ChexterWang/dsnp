/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

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
   : data(d), right(r), left(l) {}
   T                 data;
   BSTreeNode<T>*    right;
   BSTreeNode<T>*    left;
};

template <class T>
class itNode
{
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
   itNode(BSTreeNode<T>* n, char d): node(n), dir(d){}
   BSTreeNode<T>*    node;
   char              dir;
   bool operator != (const itNode& i) const{
      return ((node != i.node) || (dir != i.dir)) ? true : false;
   }
   bool operator == (const itNode& i) const{
      return ((node == i.node) && (dir == i.dir)) ? true : false;
   }
};

template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
   BSTree(){
      root = _end = new BSTreeNode<T>(T());
      _size = 0;
   }
   ~BSTree(){}

   class iterator{
      friend class BSTree;
   public:
      iterator(){}
      iterator(BSTreeNode<T>* n) : mynode(n) {}
      iterator(int i) : mynode(0) {}
      iterator(const iterator& i) : trace(i.trace), mynode(i.mynode) {}
      ~iterator(){}
      const T& operator * () const { return mynode->data; }
      T& operator * (){ return mynode->data; }
      iterator& operator ++ (){ next(); return *this; }
      iterator operator ++ (int){ iterator tmp(*this); next(); return tmp; }
      iterator& operator -- (){ prev(); return *this; }
      iterator operator -- (int){ iterator tmp(*this); prev(); return tmp; }
      iterator& operator = (const iterator& i){ trace=i.trace; mynode=i.mynode; return *this; }
      bool operator != (const iterator& i) const {
         if(trace.size()) return ( trace.top()!=i.trace.top() )?true:false;
         else return ( mynode != i.mynode )?true:false;
      }
      bool operator == (const iterator& i) const {
         if(trace.size()) return ( trace.top()==i.trace.top() )?true:false;
         else return ( mynode == i.mynode )?true:false;
      }
   private:
      stack<itNode<T> > trace;
      BSTreeNode<T>*    mynode;
      void addTrace(BSTreeNode<T>* n, char d){
         trace.push(move(itNode<T>(n, d)));
      }
      void next(){
         if(mynode->right){
            addTrace(mynode, '>');
            mynode = mynode->right;
            for(; mynode->left; mynode = mynode->left)
               addTrace(mynode, '<');
         } else {
            while(trace.size() && trace.top().dir == '>')
               trace.pop();
            mynode = trace.top().node;
            trace.pop();
         }
      }
      void prev(){
         if(mynode->left){
            addTrace(mynode, '<');
            mynode = mynode->left;
            for(; mynode->right; mynode = mynode->right)
               addTrace(mynode, '>');
         } else {
            while(trace.size() && trace.top().dir == '<')
               trace.pop();
            if(trace.size()){
               mynode = trace.top().node;
               trace.pop();
            }
            else mynode = NULL;
         }
      }
      void reset(){
         while(trace.size()) trace.pop();
         mynode = 0;
      }

   };

   iterator begin() const {
      if(!_size) return 0;
      iterator it(root);
      for(; it.mynode->left; it.mynode = it.mynode->left)
         it.addTrace(it.mynode, '<');
      return it;
   }
   iterator end() const {
      if(!_size) return 0;
      iterator it(root);
      for(; it.mynode->right; it.mynode = it.mynode->right)
         it.addTrace(it.mynode, '>');
      return it;
   }
   void insert(const T& x){
      if(!_size){ root = new BSTreeNode<T>(x, _end); ++_size; return; }
      itt.reset();
      bsfindcandy(root, x);
      if(itt == end()) --itt;
      if( (itt.mynode->data == x) || (itt == --end()) )
         itt.mynode->right = new BSTreeNode<T>(x, itt.mynode->right);
      else if(itt.mynode->data > x)
         itt.mynode->left = new BSTreeNode<T>(x);
      else itt.mynode->right = new BSTreeNode<T>(x);
      ++_size;
   }
   iterator find(const T& x) {
      if(!_size) return end();
      itt.reset();
      bsfindcandy(root, x);
      if(itt.mynode->data == x) return itt;
      return end();
   }
   void clear(){
      deltree(root);
      root = _end = new BSTreeNode<T>(T());
      _size = 0;
      itt.reset();
   }
   bool erase(iterator pos){
      if( !_size || (pos == end()) ) return false;
      if(!pos.mynode->left && !pos.mynode->right){
         if(pos.trace.top().dir == '<')
            pos.trace.top().node->left = NULL;
         else pos.trace.top().node->right = NULL;
      }
      else if(!pos.mynode->left && pos.mynode->right){
         if(!pos.trace.size())
            root = pos.mynode->right;
         else if(pos.trace.top().dir == '>')
            pos.trace.top().node->right = pos.mynode->right;
         else pos.trace.top().node->left = pos.mynode->right;
      }
      else if(pos.mynode->left && !pos.mynode->right){
         if(pos.trace.top().dir == '<')
            pos.trace.top().node->left = pos.mynode->left;
         else pos.trace.top().node->right = pos.mynode->left;
      }
      else if(pos != --end()){
         iterator origin = pos++;
         origin.mynode->data = pos.mynode->data;
         if(!pos.trace.size() || (pos.trace.top().dir == '>'))
            origin.mynode->right = pos.mynode->right;
         else pos.trace.top().node->left = pos.mynode->right;
      }
      else{
         iterator origin = pos--;
         origin.mynode->data = pos.mynode->data;
         if(pos.trace.top().dir == '<')
            origin.mynode->left = pos.mynode->left;
         else{
            pos.trace.top().node->right = _end;
            if(origin.trace.size())
                  origin.trace.top().node->right = origin.mynode->left;
            else root = origin.mynode->left;
         }
      }
      delete pos.mynode;
      --_size;
      if(!_size) _end = root;
      return true;
   }
   void pop_front(){ erase(begin()); }
   void pop_back(){ erase(--end()); }
   bool erase(const T& x){ return (erase(find(x))) ? true : false; }
   void print() const {
      bsprint(root, 0);
   }
   void sort() const {}
   bool empty() const { return (_size) ? false : true; }
   size_t size() const { return _size; }

private:
   BSTreeNode<T>*    root;
   BSTreeNode<T>*    _end;
   size_t            _size;
   iterator          itt;
   void bsfindcandy(BSTreeNode<T>* node, const T& x){
      if( (node->right) && ((x == node->right->data) || (x > node->data)) ){
         itt.addTrace(node, '>');
         bsfindcandy(node->right, x);
      }
      else if((x < node->data) && (node->left)){
         itt.addTrace(node, '<');
         bsfindcandy(node->left, x);
      }
      else itt.mynode = node;
   }
   void deltree(BSTreeNode<T>* node){
      if(node->left) deltree(node->left);
      if(!node->left && !node->right) { delete node; return; }
      if(node->right) deltree(node->right);
      if(!node->left && !node->right) { delete node; return; }
   }
   void bsprint(BSTreeNode<T>* node, size_t tab) const {
      if(node->right) bsprint(node->right, tab+1);
      if(!node->right)
         cout << setw(12*(tab+1)) << "----" << "$" << endl;
      cout << setw(12*tab) << "---" << node->data << endl;
      if(!node->left)
         cout << setw(12*(tab+1)) << "----" << "$" << endl;
      if(node->left) bsprint(node->left, tab+1);
   }

};

#endif // BST_H
