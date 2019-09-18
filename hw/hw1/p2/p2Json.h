/****************************************************************************
  FileName     [ p2Json.h]
  PackageName  [ p2 ]
  Synopsis     [ Header file for class Json JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef P2_JSON_H
#define P2_JSON_H

#include <vector>
#include <string>
#include <unordered_set>

using namespace std;

class JsonElem
{
public:

   JsonElem() {}
   JsonElem(const string& k, int v): _key(k), _value(v) {}
   int getValue();
   string getKey();

   friend ostream& operator << (ostream&, const JsonElem&);

private:
   string  _key;   // DO NOT change this definition. Use it to store key.
   int     _value; // DO NOT change this definition. Use it to store value.
};

class Json
{
public:

   bool read(const string&);
   void help();
   void print();
   void add(const string&);
   void avg();
   void max();
   void min();
   void sum();

private:
  
  // DO NOT change this definition; Use it to store JSON elements.
   vector<JsonElem>       _obj;
};

#endif // P2_TABLE_H
