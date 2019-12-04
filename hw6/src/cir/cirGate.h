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
   unsigned getId() const { return _id; }
   bool getVisited() const { return _visited; }
   unsigned numParent() const { return _fanout.size(); }
   unsigned haveChild() const;
   bool haveFloatIn() const;
   unsigned operator [] (unsigned i) const { return _fanin[i].first; }
   bool invChild(unsigned i) const { return _fanin[i].second; }
   unsigned getParent(unsigned i) const { return _fanout[i]; }

   // Printing functions
   virtual void getFloatIn(stringstream& ss){}
   virtual void printGate(unsigned& line, stringstream& ss){}
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   friend ostream& operator << (ostream& os, CirGate& cg);

   void setSymbol(string str){ _sb = str; }
   void setVisited(bool b){ _visited = b; }
   void addOut(unsigned i){ _fanout.push_back(move(i)); };

private:

protected:
   unsigned _id, _lineNo;
   GateType _type;
   bool _visited = false;
   string _sb = "";
   vector<pair<unsigned, bool> > _fanin;
   vector<unsigned> _fanout;

};

class PiGate: public CirGate
{
public:
   PiGate(unsigned id, unsigned line): CirGate(id, line, PI_GATE){}
   ~PiGate(){}
   void printGate(unsigned& line, stringstream& ss);
   void getFloatIn(stringstream& ss){ if(!_visited) _visited = true; }
private:

};

class PoGate: public CirGate
{
public:
   PoGate(unsigned id, unsigned line, unsigned in, bool inv):
   CirGate(id, line, PO_GATE){ _fanin.push_back(pair<unsigned, bool>(in, inv)); }
   ~PoGate(){}
   void printGate(unsigned& line, stringstream& ss);
   void getFloatIn(stringstream& ss);
private:

};

class AndGate: public CirGate{
public:
   AndGate(unsigned id, unsigned line, unsigned in0, bool inv0, unsigned in1, bool inv1):
   CirGate(id, line, AIG_GATE){
      _fanin.push_back(pair<unsigned, bool>(in0, inv0));
      _fanin.push_back(pair<unsigned, bool>(in1, inv1));
   }
   void printGate(unsigned& line, stringstream& ss);
   void getFloatIn(stringstream& ss);
private:

};

class ConstGate: public CirGate
{
public:
   ConstGate(unsigned id): CirGate(id, 0, CONST_GATE){}
   ~ConstGate(){}
   void printGate(unsigned& line, stringstream& ss);
   void getFloatIn(stringstream& ss){ if(!_visited) _visited = true; }
};

#endif // CIR_GATE_H
