/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBJsonElem& j) {
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   char c = is.get();
   char esc[] = {'\n', '\t', '\r', '\b', '\0', '\\', '\'', ' ', '{', ':'};
   bool inKey = false;
   string keyTemp = "";
   string valueTemp = "";
   while(file.good()){
     vector<char> escape(esc, esc+10);
     vector<char>::iterator it;
     it = find(escape.begin(), escape.end(), c);
     if(c == '"') inKey = !inKey;
     else if(((c == ',') || (c == '}')) && keyTemp != "" && valueTemp != ""){
       int valueTempInt = 0;
       if(myStr2Int(valueTemp, valueTempInt)){
         DBJsonElem elem(keyTemp, valueTempInt);
         j._obj.push_back(elem);
       }
     }
     // find: if c is not in esc[], "it" will reach the end.
     else if(it == escape.end()){
       if(inKey) keyTemp += string(1, c);
       else valueTemp += string(1, c);
     }
     if(c == '}') file.close();
     c = file.get();
   }
   return is;
}

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   os << "{" << endl;
   string comma = "";
   for(auto it = j._obj.begin(); it != j._obj.end(); ++it){
     os << comma << "  " << *it;
     comma = ",\n";
   }
   if(j._obj.begin() != j._obj.end()) cout << endl;
   os << "}" << endl;
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
   return true;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
   return 0.0;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   return  maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0;
   return s;
}
