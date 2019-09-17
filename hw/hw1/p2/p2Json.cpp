/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here
bool Json::read(const string& jsonFile) {
   ifstream file(jsonFile);
   char c = file.get();
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
       JsonElem elem(keyTemp, stoi(valueTemp));
       _obj.push_back(elem);
     }
     // find: if c is not in esc[], "it" will reach the end.
     else if(it == escape.end()){
       if(inKey) keyTemp += string(1, c);
       else valueTemp += string(1, c);
     }
     if(c == '}') return true;
     c = file.get();
   }
   file.close();
   return true;
}

void Json::help() {
  cout << endl;
  cout << "Options:\t" << "Descriptions:" << endl;
  cout << "AVG\t\t"    << "show the average of values in the file." << endl;
  cout << "EXIT\t\t"   << "exit." << endl;
  cout << "HELP\t\t"   << "show what I can do for you." << endl;
  cout << "MAX\t\t"    << "show the maximum of values." << endl;
  cout << "MIN\t\t"    << "show the minimum of values." << endl;
  cout << "PRINT\t\t"  << "show the content of the file." << endl;
  cout << "SUM\t\t"    << "show the sum of values." << endl;
  cout << endl;
}

void elem_str(JsonElem elem) {
  cout << "  \"" << elem.k_str() << "\" : " << elem.v_str() << "," << endl;
}

void lastElem_str(JsonElem elem) {
  cout << "  \"" << elem.k_str() << "\" : " << elem.v_str() << endl;
}

void Json::print() {
  cout << "{" << endl;
  if(_obj.size() > 0){
    for_each(_obj.begin(), _obj.end()-1, elem_str);
    for_each(_obj.end()-1, _obj.end()  , lastElem_str);
  }
  cout << "}" << endl;
}



ostream& operator << (ostream& os, const JsonElem& j) {
   return (os << "\"" << j._key << "\" : " << j._value);
}
