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
            case 4:
            case 5:
            case 7:
            case 8:
               s.push(tuple<CirGate*, bool, unsigned>(cirMgr->getGate((*p)[1]), p->invChild(1), (unsigned)(i+1)));
               s.push(tuple<CirGate*, bool, unsigned>(cirMgr->getGate((*p)[0]), p->invChild(0), (unsigned)(i+1)));
               break;
            case 1:
            case 2:
               s.push(tuple<CirGate*, bool, unsigned>(cirMgr->getGate((*p)[1]), p->invChild(1), (unsigned)(i+1)));
               break;
            case 3:
            case 6:
               s.push(tuple<CirGate*, bool, unsigned>(cirMgr->getGate((*p)[0]), p->invChild(0), (unsigned)(i+1)));
               break;
            default: break;
         }
      }
      else if(p->getVisited() && p->getType()==AIG_GATE) cout << " (*)";
      cout << '\n';
      p->setVisited(true);
   }
}

void CirGate::reportFanout(int level) const {
   assert (level >= 0);
   cirMgr->setVisited(false);
   stack<tuple<CirGate*, bool, unsigned> > s;
   tuple<CirGate*, bool, unsigned> t;
   CirGate *p, *q;
   unsigned i = 0;
   bool b = false;
   s.push(tuple<CirGate*, bool, unsigned>(cirMgr->getGate(_id), false, (unsigned)0));
   while(s.size()){
      b = false;
      t = s.top();
      p = get<0>(t);
      i = get<2>(t);
      s.pop();
      if(i) cout << setw(i*2) << ' ';
      if(get<1>(t)) cout << '!';
      cout << *p;
      if((i < (unsigned) level) && !p->getVisited() && p->numParent()){
         for(int k = (int)p->numParent()-1; k >= 0; --k){
            q = cirMgr->getGate(p->getParent(k));
            switch(q->haveChild()){
               case 4:
               case 5:
               case 7:
               case 8:
                  if((*q)[0] == _id) b = q->invChild(0);
                  else b = q->invChild(1);
                  break;
               case 1:
               case 2:
                  b = q->invChild(1);
                  break;
               case 3:
               case 6:
                  b = q->invChild(0);
                  break;
               default: break;
            }
            s.push(tuple<CirGate*, bool, unsigned>(q, b, (unsigned)(i+1)));
         }
      }
      else if(p->getVisited() && p->getType()==AIG_GATE) cout << " (*)";
      cout << '\n';
      p->setVisited(true);
   }
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

unsigned CirGate::haveChild() const {
   unsigned i[2]{0, 0};
   CirGate* p = 0;
   for(unsigned k = 0; k < _fanin.size(); ++k){
      p = cirMgr->getGate(_fanin[k].first);
      if(p){
         i[k]++;
         if(p->_visited) i[k]++;
      }
   }
   return 3*i[0]+i[1];
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
