/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include <sstream>
#include <regex>
#include <stack>
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
// static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << '\n';
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << '\n';
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << '\n';
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << '\n';
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << '\n';
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << '\n';
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << '\n';
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << '\n';
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << '\n';
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << '\n';
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << '\n';
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << '\n';
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << '\n';
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << '\n';
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << '\n';
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << '\n';
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << '\n';
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << '\n';
         break;
      default: break;
   }
   return false;
}

ostream& operator << (ostream& os, CirMgr& cm){
   os << cm._max << ", " << cm._input << ", " <<
   cm._output << ", " << cm._aig << '\n';
   if(cm.pi.size()){
      os << cm.pi[0];
      for(size_t i = 1; i < cm.pi.size(); ++i)
         os << ", " << cm.pi[i];
      os << '\n';
   }
   if(cm.po.size()){
      os << "PO: <" << cm.po[0].first << ", " << cm.po[0].second << ">";
      for(size_t i = 1; i < cm.po.size(); ++i)
         os << ", <" << cm.po[i].first << ", " << cm.po[i].second << ">";
      os << '\n';
   }
   if(cm.pa.size()){
      os << "PA: <" << get<0>(cm.pa[0]) << ", " << get<1>(cm.pa[0]) << ", " << get<2>(cm.pa[0]) << ">";
      for(size_t i = 1; i < cm.pa.size(); ++i)
         os << ", <" << get<0>(cm.pa[i]) << ", " << get<1>(cm.pa[i]) << ", " << get<2>(cm.pa[i]) << ">";
      os << '\n';
   }
   for(size_t i = 0; i < cm._all.size(); ++i){
      if(cm._all[i]){
         os << i << ", " << cm._all[i]->getTypeStr() << ", " << cm._all[i]->getLineNo() << '\n';
      }
   }
   return os;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream ifs(fileName);
   readState s = OPEN;
   while(ifs.good()){
      switch(s){
         case OPEN:
            if(readHeader(ifs)) s = HEADER;
            else {s = ERROR;} break;
         case HEADER:
            if(readInput(ifs)) s = INPUT;
            else {s = ERROR;} break;
         case INPUT:
            if(readOutput(ifs)) s = OUTPUT;
            else {s = ERROR;} break;
         case OUTPUT:
            if(readAIG(ifs)) s = AIG;
            else {s = ERROR;} break;
         case AIG:
            if(readSymbol(ifs)) s = SYMBOL;
            else {s = ERROR;} break;
         case SYMBOL:
            if(readComment(ifs)) s = COMMENT;
            else {s = ERROR;} break;
         case COMMENT: break;
         case ERROR: clearRead(); return false;
      }
      if(s == COMMENT) break;
   }
   ifs.close();
   // cout << *this << '\n';
   return true;
}

bool CirMgr::readHeader(ifstream& fs){
   string str;
   regex p("^(aag )[0-9]+( )[0-9]+( )[0-9]+( )[0-9]+( )[0-9]+$");
   getline(fs, str);
   if(regex_match(str, p)){
      string tok;
      size_t pos = myStrGetTok(str, tok);
      pos = myStrGetTok(str, tok, pos);
      _max = unsigned(stoi(tok));
      pos = myStrGetTok(str, tok, pos);
      _input = unsigned(stoi(tok));
      pos = myStrGetTok(str, tok, pos);
      pos = myStrGetTok(str, tok, pos);
      _output = unsigned(stoi(tok));
      pos = myStrGetTok(str, tok, pos);
      _aig = unsigned(stoi(tok));
      if(_max >= _input + _aig){
         _all.resize(_max+_output+1);
         lineNo = 1;
         return true;
      }
      else{
         lineNo = 0;
         errMsg = "Number of variables";
         errInt = _max;
         return parseError(NUM_TOO_SMALL);
      }
   }
   else{
      return false;
   }
}

bool CirMgr::checkLiteral(int i, readState s){
   if((unsigned)i > _max*2+1){
      colNo = 0;
      errMsg = "Number of variables";
      errInt = i;
      return parseError(MAX_LIT_ID);
   }
   else if(s != INPUT){
      if(!(i/2)){
         colNo = 0;
         errInt = i;
         return parseError(REDEF_CONST);
      }
      else if(i%2){
         colNo = 0;
         errInt = i;
         switch(s){
            case HEADER: errMsg = "PI"; break;
            case OUTPUT: errMsg = "AIG gate"; break;
            default: break;
         }
         return parseError(CANNOT_INVERTED);
      }
      else if(_all[i/2]){
         errInt = i;
         errGate = _all[i/2];
         return parseError(REDEF_GATE);
      }
   }
   return true;
}

bool CirMgr::readInput(ifstream& fs){
   string str, tok;
   int i;
   regex p("^[0-9]+$");
   for(unsigned j = 0; j < _input; ++j){
      getline(fs, str);
      if(regex_match(str, p)){
         myStrGetTok(str, tok);
         i = stoi(tok);
         if(!checkLiteral(i, HEADER)) return false;
         i /= 2;
         _all[i] = new PiGate(i, lineNo+1);
         pi.push_back(move(unsigned(i)));
         ++lineNo;
      }
      else{
         return false;
      }
   }
   return true;
}

bool CirMgr::readOutput(ifstream& fs){
   string str, tok;
   int i;
   regex p("^[0-9]+$");
   for(unsigned j = 0; j < _output; ++j){
      getline(fs, str);
      if(regex_match(str, p)){
         myStrGetTok(str, tok);
         i = stoi(tok);
         if(!checkLiteral(i, INPUT)) return false;
         _all[_max+j+1] = new PoGate(_max+j+1, lineNo+1, i/2, i%2);
         po.push_back(pair<unsigned, unsigned>(_max+j+1, i/2));
         ++lineNo;
      }
      else{
         return false;
      }
   }
   return true;
}

void CirMgr::connectOut(){
   CirGate* p = 0;
   for(size_t i = 0; i < po.size(); ++i){
      p = getGate(po[i].second);
      if(p) p->addOut(po[i].first);
   }
   for(size_t i = 0; i < pa.size(); ++i){
      p = getGate(get<1>(pa[i]));
      if(p) p->addOut(get<0>(pa[i]));
      p = getGate(get<2>(pa[i]));
      if(p) p->addOut(get<0>(pa[i]));
   }
}

bool CirMgr::readAIG(ifstream& fs){
   string str, tok;
   int i[3];
   size_t pos = 0;
   regex p("^[0-9]+( )[0-9]+( )[0-9]+$");
   for(unsigned j = 0; j < _aig; ++j){
      getline(fs, str);
      if(regex_match(str, p)){
         pos = myStrGetTok(str, tok);
         i[0] = stoi(tok);
         if(!checkLiteral(i[0], OUTPUT)) return false;
         pos = myStrGetTok(str, tok, pos);
         i[1] = stoi(tok);
         myStrGetTok(str, tok, pos);
         i[2] = stoi(tok);
         _all[i[0]/2] = new AndGate(i[0]/2, lineNo+1, i[1]/2, i[1]%2, i[2]/2, i[2]%2);
         pa.push_back(tuple<unsigned, unsigned, unsigned>(i[0]/2, i[1]/2, i[2]/2));
         ++lineNo;
      }
      else{
         return false;
      }
   }
   connectOut();
   return true;
}
bool CirMgr::readSymbol(ifstream& fs){
   return true;
}
bool CirMgr::readComment(ifstream& fs){
   return true;
}
void CirMgr::clearRead(){
   _max = _input = _output = _aig = 0;
   lineNo = colNo = 0;
   for(size_t i = 1; i < _all.size(); ++i){
      if(_all[i]) delete _all[i];
   }
   _all.resize(1);
   pi.clear();
   po.clear();
   pa.clear();
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << '\n';
   cout << "Circuit Statistics" << '\n';
   cout << "==================" << '\n';
   cout << "  PI" << setw(12) << pi.size() << '\n';
   cout << "  PO" << setw(12) << po.size() << '\n';
   cout << "  AIG" << setw(11) << pa.size() << '\n';
   cout << "------------------" << '\n';
   cout << "  Total" << setw(9) << pi.size()+po.size()+pa.size() << '\n';
}

void CirMgr::setVisited(bool b) const {
   for(size_t i = 0; i < _all.size(); ++i){
      if(_all[i]) _all[i]->setVisited(b);
   }
}

void
CirMgr::printNetlist() const
{
   setVisited(false);
   lineNo = 0;
   cout << '\n';
   stringstream ss;
   stack<CirGate*> s;
   CirGate* current;
   unsigned j = 0;
   for(size_t i = 0; i < po.size(); ++i){
      s.push(_all[_max+i+1]);
      while(s.size()){
         current = s.top();
         j = current->haveChild();
         switch(j){
            case 0:
            case 2:
            case 6:
            case 8:
               ss.str("");
               current->printGate(lineNo, ss);
               s.pop();
               break;
            case 3:
            case 5:
               s.push(getGate((*current)[0]));
               break;
            case 1:
            case 7:
               s.push(getGate((*current)[1]));
               break;
            case 4:
               s.push(getGate((*current)[1]));
               s.push(getGate((*current)[0]));
               break;
            default: break;
         }
      }
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(size_t i = 0; i < pi.size(); ++i)
      cout << " " << pi[i];
   cout << '\n';
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(size_t i = 0; i < po.size(); ++i)
      cout << " " << po[i].first;
   cout << '\n';
}

void CirMgr::findFloat(){
   for(size_t i = 0; i < _max + _output + 1; ++i){
      if(_all[i]){
         if(!_all[i]->numParent()) flout.push_back(i);
         if(_all[i]->haveFloatIn()) floin.push_back(i);
      }
   }
}

void
CirMgr::printFloatGates()
{
   if(!_sorted){
      findFloat();
      sort(floin.begin(), floin.end());
      sort(flout.begin(), flout.end());
      _sorted = true;
   }
   if(floin.size()){
      cout << "Gates with floating fanin(s):";
      for(size_t i = 0; i < floin.size(); ++i)
         cout << " " << floin[i];
      cout << '\n';
   }
   if(flout.size()){
      cout << "Gates defined but not used  :";
      for(size_t i = 0; i < flout.size(); ++i)
         cout << " " << flout[i];
      cout << '\n';
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
}
