/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <stack>
#include <tuple>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
string CirGate::getTypeStr() const {
   switch(_type){
      case PI_GATE: return "PI";
      case PO_GATE: return "PO";
      case AIG_GATE: return "AIG";
      case CONST_GATE: return "CONST";
      default: return "UNDEF";
   }
}

void CirGate::reportGate() const{
   stringstream ss;
   cout << "==================================================" << '\n';
   cout << "= " << setw(47) << left;
   ss << getTypeStr() << "(" << _id << ")";
   if(_sb.compare("")) ss << "\"" << _sb << "\"";
   ss << ", line " << _lineNo;
   cout << ss.str() << "=" << '\n';
   cout << "==================================================" << '\n';
}

ostream& operator << (ostream& os, CirGate& cg){
   os << cg.getTypeStr() << " " << cg.getId();
   return os;
}

void CirGate::reportFanin(int level) const {
   assert (level >= 0);
   cirMgr->setVisited(false);
   stack<tuple<CirGate*, bool, unsigned> > s;
   tuple<CirGate*, bool, unsigned> t;
   CirGate* p;
   unsigned i = 0;
   s.push(tuple<CirGate*, bool, unsigned>(cirMgr->getGate(_id), false, (unsigned)0));
   while(s.size()){
      t = s.top();
      p = get<0>(t);
      i = get<2>(t);
      s.pop();
      if(i) cout << setw(i*2) << ' ';
      if(get<1>(t)) cout << '!';
      cout << *p;
      if((i < (unsigned) level) && !p->getVisited()){
         switch(p->haveChild()){
            case 3:
               s.push(tuple<CirGate*, bool, unsigned>(cirMgr->getGate((*p)[1]), p->invChild(1), (unsigned)(i+1)));
               s.push(tuple<CirGate*, bool, unsigned>(cirMgr->getGate((*p)[0]), p->invChild(0), (unsigned)(i+1)));
            case 2:
               s.push(tuple<CirGate*, bool, unsigned>(cirMgr->getGate((*p)[1]), p->invChild(1), (unsigned)(i+1)));
            case 1:
               s.push(tuple<CirGate*, bool, unsigned>(cirMgr->getGate((*p)[0]), p->invChild(0), (unsigned)(i+1)));
            default: break;
         }
      }
      else if(p->getVisited()) cout << " (*)";
      cout << '\n';
      p->setVisited(true);
   }
}

void CirGate::reportFanout(int level) const {
   assert (level >= 0);
}

void PiGate::printGate(unsigned& line, stringstream& ss){
   if(!_visited){
      ss << "[" << line << "] " << "PI  " << _id;
      if(_sb.compare("")) ss << " (" << _sb << ")";
      cout << ss.str() << '\n';
      _visited = true;
      ++line;
   }
}

void PoGate::printGate(unsigned& line, stringstream& ss){
   ss << "[" << line << "] " << "PO  " << _id << " ";
   if(!cirMgr->getGate(_fanin[0].first)) ss << "*";
   if(_fanin[0].second) ss << "!";
   ss << _fanin[0].first;
   if(_sb.compare("")) ss << " (" << _sb << ")";
   cout << ss.str() << '\n';
   ++line;
}

void AndGate::printGate(unsigned& line, stringstream& ss){
   if(!_visited){
      ss << "[" << line << "] " << "AIG " << _id << " ";
      if(!cirMgr->getGate(_fanin[0].first)) ss << "*";
      if(_fanin[0].second) ss << "!";
      ss << _fanin[0].first << " ";
      if(!cirMgr->getGate(_fanin[1].first)) ss << "*";
      if(_fanin[1].second) ss << "!";
      ss << _fanin[1].first;
      if(_sb.compare("")) ss << " (" << _sb << ")";
      cout << ss.str() << '\n';
      _visited = true;
      ++line;
   }
}

void ConstGate::printGate(unsigned& line, stringstream& ss){
   if(!_visited){
      cout << "[" << line << "] " << "CONST0" << '\n';
      _visited = true;
      ++line;
   }
}

bool CirGate::haveParent() const {
   switch(_type){
      case PI_GATE:
      case AIG_GATE: return (_fanout.size())?true:false;
      default: return true;
   }
}

unsigned CirGate::haveChild() const {
   switch(_type){
      case PO_GATE:{
         CirGate* p = cirMgr->getGate(_fanin[0].first);
         if(p && !p->_visited) return 1;
         else return 0;
      }
      case AIG_GATE:{
         CirGate* p = cirMgr->getGate(_fanin[0].first);
         CirGate* q = cirMgr->getGate(_fanin[1].first);
         if(p && !p->_visited){
            if(q && !q->_visited) return 3;
            else return 1;
         }
         else if(q && !q->_visited) return 2;
         else return 0;
      }
      default: return 0;
   }
}

bool CirGate::haveFloatIn() const {
   switch(_type){
      case PO_GATE:
         return (!cirMgr->getGate(_fanin[0].first))?true:false;
      case AIG_GATE:
         return (!cirMgr->getGate(_fanin[0].first) ||
                 !cirMgr->getGate(_fanin[1].first))?true:false;
      default: return false;
   }
}

void PoGate::getFloatIn(stringstream& ss){
   CirGate* p = cirMgr->getGate(_fanin[0].first);
   if(p) p->getFloatIn(ss);
   else ss << _id << " ";
   if(!_visited) _visited = true;
}

void AndGate::getFloatIn(stringstream& ss){
   if(!_visited){
      bool appended = false;
      CirGate* p = cirMgr->getGate(_fanin[0].first);
      if(p) p->getFloatIn(ss);
      else{ ss << _id << " "; appended = true; }
      p = cirMgr->getGate(_fanin[1].first);
      if(p) p->getFloatIn(ss);
      else if(!appended) ss << _id << " ";
      _visited = true;
   }
}