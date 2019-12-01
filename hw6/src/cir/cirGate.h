/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate(){}
   CirGate(GateType t): _type(t){}
   CirGate(unsigned id, unsigned line, GateType t):
      _id(id), _lineNo(line), _type(t){}
   virtual ~CirGate() {}

   // Basic access methods
   string getTypeStr() const;
   GateType getType() const { return _type; }
   unsigned getLineNo() const { return _lineNo; }

   // Printing functions
   virtual void printGate(unsigned& line){}
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;

   void setSymbol(string str){ _sb = str; }

   unsigned operator [] (unsigned i){ return _fanin[i].first; }
   unsigned operator [] (unsigned i) const { return _fanin[i].first; }

private:

protected:
   unsigned _id, _lineNo;
   GateType _type;
   bool _visited = false;
   string _sb = "";
   vector<pair<unsigned, bool> > _fanin;
};

class PiGate: public CirGate
{
public:
   PiGate(unsigned id, unsigned line): CirGate(id, line, PI_GATE){}
   ~PiGate(){}
   void printGate(unsigned& line);
private:

};

class PoGate: public CirGate
{
public:
   PoGate(unsigned id, unsigned line, unsigned in, bool inv):
   CirGate(id, line, PO_GATE){ _fanin.push_back(pair<unsigned, bool>(in, inv)); }
   ~PoGate(){}
   void printGate(unsigned& line);
private:

};

class AndGate: public CirGate{
public:
   AndGate(unsigned id, unsigned line, unsigned in0, bool inv0, unsigned in1, bool inv1):
   CirGate(id, line, AIG_GATE){
      _fanin.push_back(pair<unsigned, bool>(in0, inv0));
      _fanin.push_back(pair<unsigned, bool>(in1, inv1));
   }
   void printGate(unsigned& line);
private:

};

class ConstGate: public CirGate
{
public:
   ConstGate(unsigned id): CirGate(id, 0, CONST_GATE){}
   ~ConstGate(){}
   void printGate(unsigned& line);
};

#endif // CIR_GATE_H
