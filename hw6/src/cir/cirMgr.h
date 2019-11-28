/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr() {}

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { return 0; }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void writeAag(ostream&) const;

   friend ostream& operator << (ostream& os, CirMgr& cm){
      os << cm._max << ", " << cm._input << ", " <<
      cm._output << ", " << cm._aig << endl;
   }

private:
   unsigned _max = 0,  _input = 0, _output = 0, _aig = 0;
   IdList pi;
   bool readHeader(ifstream& fs);
   bool readInput(ifstream& fs);
   bool readOutput(ifstream& fs);
   bool readAIG(ifstream& fs);
   bool readSymbol(ifstream& fs);
   bool readComment(ifstream& fs);
   void clearRead();
};


#endif // CIR_MGR_H
