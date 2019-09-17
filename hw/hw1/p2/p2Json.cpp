/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include <ifstream>

#include "p2Json.h"

using namespace std;

// Implement member functions of class Row and Table here
bool Json::read(const string& jsonFile){
   ifstream file(jsonFile);
   char c = file.get();
   while(file.good()){
     cout << c;
     c = file.get();
   }
   file.close();
   return true;
}

void Json::help(){
  cout << endl;
  cout << "Options:\t" << "Descriptions:" << endl;
  cout << "AVG\t\t"    << "show the average of values in the file." << endl;
  cout << "HELP\t\t"   << "show what I can do for you." << endl;
  cout << "MAX\t\t"    << "show the maximum of values." << endl;
  cout << "MIN\t\t"    << "show the minimum of values." << endl;
  cout << "PRINT\t\t"  << "show the content of the file." << endl;
  cout << "SUM\t\t"    << "show the sum of values." << endl;
  cout << endl;
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}
