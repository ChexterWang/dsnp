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
#include <tuple>

using namespace std;

#include "cirDef.h"
#include "cirGate.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){
      CirGate* const0 = new ConstGate(0);
      _all.push_back(const0);
   }
   ~CirMgr() {
      for(size_t i = 0; i < _all.size(); ++i) delete _all[i];
      _all.clear();
   }

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const{
      return (_all[gid])?_all[gid]:0;
   }

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates();
   void writeAag(ostream&) const;

   friend ostream& operator << (ostream& os, CirMgr& cm);

private:
   bool _sorted = false;
   unsigned _max = 0,  _input = 0, _output = 0, _aig = 0;
   vector<unsigned> pi, floin, flout;
   vector<pair<unsigned, unsigned> > po;
   vector<tuple<unsigned,unsigned,unsigned> > pa;
   vector<CirGate*> _all;
   bool readHeader(ifstream& fs);
   bool checkLiteral(int i, bool out);
   bool readInput(ifstream& fs);
   bool readOutput(ifstream& fs);
   void connectOut();
   bool readAIG(ifstream& fs);
   bool readSymbol(ifstream& fs);
   bool readComment(ifstream& fs);
   void findFloat();
   void clearRead();
};


#endif // CIR_MGR_H
