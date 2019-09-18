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
#include <algorithm> // find() in vector<>
#include <iomanip>   // setprecision
#include "p2Json.h"

using namespace std;

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
     if(c == '}'){
       file.close();
       return true;
     }
     c = file.get();
   }
   return false;
}

void Json::help() {
  cout << endl;
  cout << "Options:\t" << "Descriptions:" << endl;
  cout << "ADD\t\t"    << "add a temporary pair." << endl;
  cout << "AVG\t\t"    << "show the average of values in the file." << endl;
  cout << "EXIT\t\t"   << "exit." << endl;
  cout << "HELP\t\t"   << "show what I can do for you." << endl;
  cout << "MAX\t\t"    << "show the maximum of values." << endl;
  cout << "MIN\t\t"    << "show the minimum of values." << endl;
  cout << "PRINT\t\t"  << "show the content of the file." << endl;
  cout << "SUM\t\t"    << "show the sum of values." << endl;
  cout << endl;
}

void Json::print() {
  cout << "{" << endl;
  string comma = "";
  for(auto it = _obj.begin(); it != _obj.end(); ++it){
    cout << comma << "  " << *it;
    comma = ",\n";
  }
  if(_obj.begin() != _obj.end()) cout << endl;
  cout << "}" << endl;
}

void Json::add(const string& cmd) {
  string temp = "";
  string str = cmd + " ";
  vector<string> argss;
  for(string::iterator it = str.begin(); it != str.end(); ++it){
    if(*it != ' ') temp += string(1, *it);
    else if(temp != ""){
      if(argss.size() == 1){
        bool repeated = true;
        for(vector<JsonElem>::iterator itt = _obj.begin(); itt <= _obj.end(); ++itt){
          if(temp == (itt->getKey())) break;
          if(itt == _obj.end()) repeated = false;
        }
        if(repeated){
          cout << "Error: Key \"" << temp << "\" is repeated!!" << endl;
          break;
        }
      }
      argss.push_back(temp);
      temp = "";
    }
  }
  if(argss.size() > 2) _obj.push_back(JsonElem(argss.at(1), stoi(argss.at(2))));
}

int JsonElem::getValue() {
  int &temp = _value;
  return temp;
}

string JsonElem::getKey() {
  string &temp = _key;
  return temp;
}

void Json::ave() {
  if(_obj.size() == 0) cout << "Error: No element found!" << endl;
  else {
    float total = 0;
    for(auto it = _obj.begin(); it != _obj.end(); ++it){
      total += float(it->getValue());
    }
    total = total / _obj.size();
    cout << "The average of the values is: ";
    cout << fixed << setprecision(1) << total << "." << endl;
  }
}

void Json::max() {
  if(_obj.size() == 0) cout << "Error: No element found!" << endl;
  else {
    vector<JsonElem>::iterator itTemp = _obj.begin();
    for(auto it = _obj.begin()+1; it != _obj.end(); ++it){
      if((it->getValue()) > (itTemp->getValue())) itTemp = it;
    }
    cout << "The maximum element is: { " << *itTemp << " }." << endl;
  }
}

void Json::min() {
  if(_obj.size() == 0) cout << "Error: No element found!" << endl;
  else {
    vector<JsonElem>::iterator itTemp = _obj.begin();
    for(auto it = _obj.begin()+1; it != _obj.end(); ++it){
      if((it->getValue()) < (itTemp->getValue())) itTemp = it;
    }
    cout << "The minimum element is: { " << *itTemp << " }." << endl;
  }
}

void Json::sum() {
  if(_obj.size() == 0) cout << "Error: No element found!" << endl;
  else {
    int total = 0;
    for(auto it = _obj.begin(); it != _obj.end(); ++it){
      total += it->getValue();
    }
    cout << "The summation of the values is: " << total << "." << endl;
  }
}

void Json::cmd404(const string& cmd) {
  cout << "Error: unknown command: \"" << cmd << "\"" << endl;
}

ostream& operator << (ostream& os, const JsonElem& j) {
   return (os << "\"" << j._key << "\" : " << j._value);
}
