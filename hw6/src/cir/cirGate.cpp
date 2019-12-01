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
   cout << "==================================================" << endl;
   cout << "= " << setw(47) << left;
   ss << getTypeStr() << "(" << _id << ")";
   if(_sb.compare("")) ss << "\"" << _sb << "\"";
   ss << ", line " << _lineNo;
   cout << ss.str() << "=" << endl;
   cout << "==================================================" << endl;
}

void CirGate::reportFanin(int level) const {
   assert (level >= 0);
}

void CirGate::reportFanout(int level) const {
   assert (level >= 0);
}

void PiGate::printGate(unsigned& line){
   if(!_visited){
      stringstream ss;
      ss << "[" << line << "] " << "PI  " << _id;
      if(_sb.compare("")) ss << " (" << _sb << ")";
      cout << ss.str() << endl;
      _visited = true;
      ++line;
   }
}

void PoGate::printGate(unsigned& line){
   CirGate* p = cirMgr->getGate(_fanin[0].first);
   if(p) p->printGate(line);
   stringstream ss;
   ss << "[" << line << "] " << "PO  " << _id << " ";
   if(!cirMgr->getGate(_fanin[0].first)) ss << "*";
   if(_fanin[0].second) ss << "!";
   ss << _fanin[0].first;
   if(_sb.compare("")) ss << " (" << _sb << ")";
   cout << ss.str() << endl;
   ++line;
}

void AndGate::printGate(unsigned& line){
   CirGate* p = cirMgr->getGate(_fanin[0].first);
   if(p) p->printGate(line);
   p = cirMgr->getGate(_fanin[1].first);
   if(p) p->printGate(line);
   if(!_visited){
      stringstream ss;
      ss << "[" << line << "] " << "AIG " << _id << " ";
      if(!cirMgr->getGate(_fanin[0].first)) ss << "*";
      if(_fanin[0].second) ss << "!";
      ss << _fanin[0].first << " ";
      if(!cirMgr->getGate(_fanin[1].first)) ss << "*";
      if(_fanin[1].second) ss << "!";
      ss << _fanin[1].first;
      if(_sb.compare("")) ss << " (" << _sb << ")";
      cout << ss.str() << endl;
      _visited = true;
      ++line;
   }
}

void ConstGate::printGate(unsigned& line){
   if(!_visited){
      cout << "[" << line << "] " << "CONST0" << endl;
      _visited = true;
      ++line;
   }
}
