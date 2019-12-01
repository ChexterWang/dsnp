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
#include <regex>
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
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

enum readState{
   OPEN,
   HEADER,
   INPUT,
   OUTPUT,
   AIG,
   SYMBOL,
   COMMENT,
   ERROR
};

ostream& operator << (ostream& os, CirMgr& cm){
   os << cm._max << ", " << cm._input << ", " <<
   cm._output << ", " << cm._aig << endl;
   if(cm.pi.size()){
      os << cm.pi[0];
      for(size_t i = 1; i < cm.pi.size(); ++i)
         os << ", " << cm.pi[i];
      os << endl;
   }
   if(cm.po.size()){
      os << "PO: <" << cm.po[0].first << ", " << cm.po[0].second << ">";
      for(size_t i = 1; i < cm.po.size(); ++i)
         os << ", <" << cm.po[i].first << ", " << cm.po[i].second << ">";
      os << endl;
   }
   if(cm.pa.size()){
      os << "PA: <" << get<0>(cm.pa[0]) << ", " << get<1>(cm.pa[0]) << ", " << get<2>(cm.pa[0]) << ">";
      for(size_t i = 1; i < cm.pa.size(); ++i)
         os << ", <" << get<0>(cm.pa[i]) << ", " << get<1>(cm.pa[i]) << ", " << get<2>(cm.pa[i]) << ">";
      os << endl;
   }
   for(size_t i = 0; i < cm._all.size(); ++i){
      if(cm._all[i]){
         os << i << ", " << cm._all[i]->getTypeStr() << ", " << cm._all[i]->getLineNo() << endl;
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
   // cout << *this << endl;
   return true;
}

bool CirMgr::readHeader(ifstream& fs){
   string str;
   getline(fs, str);
   if(regex_match(str, regex("^(aag )[0-9]+( )[0-9]+( )[0-9]+( )[0-9]+( )[0-9]+$"))){
      string tok;
      int i;
      size_t pos = myStrGetTok(str, tok);
      pos = myStrGetTok(str, tok, pos);
      myStr2Int(tok, i);
      _max = unsigned(i);
      pos = myStrGetTok(str, tok, pos);
      myStr2Int(tok, i);
      _input = unsigned(i);
      pos = myStrGetTok(str, tok, pos);
      pos = myStrGetTok(str, tok, pos);
      myStr2Int(tok, i);
      _output = unsigned(i);
      pos = myStrGetTok(str, tok, pos);
      myStr2Int(tok, i);
      _aig = unsigned(i);
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

bool CirMgr::checkLiteral(int i, bool out){
   if((unsigned)i > _max*2+1){
      colNo = 0;
      errMsg = "Number of variables";
      errInt = i;
      return parseError(MAX_LIT_ID);
   }
   else if(!out){
      if(!(i/2)){
         colNo = 0;
         errInt = i;
         return parseError(REDEF_CONST);
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
   for(unsigned j = 0; j < _input; ++j){
      string str;
      getline(fs, str);
      if(regex_match(str, regex("^[0-9]*[02468]+$"))){
         string tok;
         int i;
         myStrGetTok(str, tok);
         myStr2Int(tok, i);
         if(!checkLiteral(i, false)) return false;
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
   for(unsigned j = 0; j < _output; ++j){
      string str;
      getline(fs, str);
      if(regex_match(str, regex("^[0-9]+$"))){
         string tok;
         int i;
         myStrGetTok(str, tok);
         myStr2Int(tok, i);
         if(!checkLiteral(i, true)) return false;
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

bool CirMgr::readAIG(ifstream& fs){
   for(unsigned j = 0; j < _aig; ++j){
      string str;
      getline(fs, str);
      if(regex_match(str, regex("^[0-9]*[02468]+( )[0-9]+( )[0-9]+$"))){
         string tok;
         int i[3];
         size_t pos = myStrGetTok(str, tok);
         myStr2Int(tok, i[0]);
         if(!checkLiteral(i[0], false)) return false;
         pos = myStrGetTok(str, tok, pos);
         myStr2Int(tok, i[1]);
         myStrGetTok(str, tok, pos);
         myStr2Int(tok, i[2]);
         _all[i[0]/2] = new AndGate(i[0]/2, lineNo+1, i[1]/2, i[1]%2, i[2]/2, i[2]%2);
         pa.push_back(tuple<unsigned, unsigned, unsigned>(i[0]/2, i[1]/2, i[2]/2));
         ++lineNo;
      }
      else{
         return false;
      }
   }
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
   cout << endl;
   cout << "Circuit Statistics" << endl;
   cout << "==================" << endl;
   cout << "  PI" << setw(12) << pi.size() << endl;
   cout << "  PO" << setw(12) << po.size() << endl;
   cout << "  AIG" << setw(11) << pa.size() << endl;
   cout << "------------------" << endl;
   cout << "  Total" << setw(9) << pi.size()+po.size()+pa.size() << endl;
}

void
CirMgr::printNetlist() const
{
   lineNo = 0;
   cout << endl;
   for(size_t i = 0; i < po.size(); ++i){
      _all[_max+i+1]->printGate(lineNo);
   }
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(size_t i = 0; i < pi.size(); ++i)
      cout << " " << pi[i];
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(size_t i = 0; i < po.size(); ++i)
      cout << " " << po[i].first;
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
}

void
CirMgr::writeAag(ostream& outfile) const
{
}
