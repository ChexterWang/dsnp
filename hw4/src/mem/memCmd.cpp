/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{
   cout << setw(15) << left << "MTReset: "
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   vector<string> tokens;
   int n, m;
   bool s = false;

   CmdExec::lexOptions(option, tokens);

   if(tokens.size() < 1)
      return CmdExec::errorOption(CMD_OPT_MISSING, "");
   // single token: {"legal"}
   else if((tokens.size() == 1) && myStr2Int(tokens[0], n) && (n > 0)) {
      mtest.newObjs(n);
      return CMD_EXEC_DONE;
   }
   // tokens[0] == -a
   else if((tokens.size() >= 1) && !myStrNCmp("-Array", tokens[0], 2)) {
      // NO array size: {"-a"}
      if(tokens.size() == 1)
         return CmdExec::errorOption(CMD_OPT_MISSING, tokens[0]);
      // WRONG array size: {"-a", "illegal", ...}
      else if(!myStr2Int(tokens[1], m) || (m < 1))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[1]);
      // A0_WITH_SIZE
      else{
         // NO number of arrays: {"-a", "legal"}
         if(tokens.size() == 2)
            return CmdExec::errorOption(CMD_OPT_MISSING, "");
         // WRONG number of arrays: {"-a", "legal", "illegal", ...}
         else if(!myStr2Int(tokens[2], n) || (n < 1))
            return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
         // right A0 M N form: {"-a", "legal", "legal"}
         else if(tokens.size() == 3) s = true;
         // A0 M N [extra opt]: {"-a", "legal", "legal", ...}
         else return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[3]);
      }
   }
   // tokens[0] is illegal: {"illegal", ...}
   else if(!s && (!myStr2Int(tokens[0], n) || (n < 1)))
      return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[0]);
   // tokens[0] is legal and tokens[1] is -a
   else if((tokens.size() >= 2) && !myStrNCmp("-Array", tokens[1], 2)){
      // NO array size: {"legal", "-a"}
      if(tokens.size() == 2)
         return CmdExec::errorOption(CMD_OPT_MISSING, "-a");
      // WRONG array size: {"legal", "-a", "illegal", ...}
      else if(!myStr2Int(tokens[2], m) || (m < 1))
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, tokens[2]);
      // right N A1 M form: {"legal", "-a", "legal"}
      else if(tokens.size() == 3) s = true;
      // N A1 M [extra opt]: {"legal", "-a", "legal", ...}
      else return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[3]);
   }
   // {"legal", "legal", ...}
   else return CmdExec::errorOption(CMD_OPT_EXTRA, tokens[1]);

   if(s){
      try{
         mtest.newArrs(n, m);
         return CMD_EXEC_DONE;
      }
      catch(bad_alloc){
         return CMD_EXEC_ERROR;
      }
      return CMD_EXEC_DONE;
   }

   // Use try-catch to catch the bad_alloc exception
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{
   cout << setw(15) << left << "MTNew: "
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO

   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{
   cout << setw(15) << left << "MTDelete: "
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{
   cout << setw(15) << left << "MTPrint: "
        << "(memory test) print memory manager info" << endl;
}
