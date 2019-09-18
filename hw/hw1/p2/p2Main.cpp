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

   // Read in the csv file. Do NOT change this part of code.
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
   string mathMode = "AVG/MAX/MIN/SUM";
   cout << "Enter command: ";
   while (getline(cin, cmd, '\n')) {
      if(!cmd.compare("EXIT")) exit(0);
      if(!cmd.compare("HELP")) json.help();
      if(!cmd.compare("PRINT")) json.print();
      if(cmd.find("ADD") == 0) json.add(cmd);
      if(!cmd.compare("AVG")) json.avg();
      if(!cmd.compare("MAX")) json.max();
      if(!cmd.compare("MIN")) json.min();
      if(!cmd.compare("SUM")) json.sum();
      if(cmd.compare("")) cout << "Enter command: ";
    }
}
