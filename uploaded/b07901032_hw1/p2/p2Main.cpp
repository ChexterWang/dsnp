/****************************************************************************
  FileName     [ p2Main.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define main() function ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2016-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"

using namespace std;

int main()
{
   Json json;

   string jsonFile;
   cout << "Please enter the file name: ";
   cin >> jsonFile;
   if (json.read(jsonFile))
      cout << "File \"" << jsonFile << "\" was read in successfully." << endl;
   else {
      cerr << "Failed to read in file \"" << jsonFile << "\"!" << endl;
      exit(-1); // jsonFile does not exist.
   }

   // TODO read and execute commands
   string cmd = "";
   while (getline(cin, cmd, '\n')) {
     if(cmd.compare("")){
        if(!cmd.compare("EXIT"))       exit(0);
        else if(!cmd.compare("HELP"))  json.help();
        else if(!cmd.compare("PRINT")) json.print();
        else if(cmd.find("ADD ") == 0)  json.add(cmd);
        else if(!cmd.compare("AVE"))   json.ave();
        else if(!cmd.compare("MAX"))   json.max();
        else if(!cmd.compare("MIN"))   json.min();
        else if(!cmd.compare("SUM"))   json.sum();
        else json.cmd404(cmd);
      }
      cout << "Enter command: ";
    }
}
