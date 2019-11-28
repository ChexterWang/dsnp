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
#include "cirGate.h"
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
   cout << *this << endl;
   return true;
}

bool CirMgr::readHeader(ifstream& fs){
   regex pattern("^(aag )[0-9]+( )[0-9]+( )[0-9]+( )[0-9]+( )[0-9]+$");
   string str;
   getline(fs, str);
   if(regex_match(str, pattern)){
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
      return true;
   }
   else{
      return false;
   }
}
bool CirMgr::readInput(ifstream& fs){
   for(int i = 0; i < cm._input; ++i){
      regex pattern("^[0-9]+$");
      string str;
      getline(fs, str);
      if(regex_match(str, pattern)){
         string tok;
         int i;
         size_t pos = myStrGetTok(str, tok);
         myStr2Int(tok, i);
         pi.push_back(move(unsigned(i)));
      }
      else{
         return false;
      }
   }
   return true;
}
bool CirMgr::readOutput(ifstream& fs){
   return true;
}
bool CirMgr::readAIG(ifstream& fs){
   return true;
}
bool CirMgr::readSymbol(ifstream& fs){
   return true;
}
bool CirMgr::readComment(ifstream& fs){
   return true;
}
void CirMgr::clearRead(){
   return;
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
}

void
CirMgr::printNetlist() const
{
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
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
