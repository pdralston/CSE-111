// $Id: debug.cpp,v 1.14 2019-10-16 15:17:26-07 - - $
// Sasank Madineni (smadinen)
// Perry Ralston (pdralsto)

#include <climits>
#include <iostream>
#include <vector>

using namespace std;

#include "debug.h"
#include "util.h"

debugflags::flagset_ debugflags::flags_ {};

//function: debugflags
//descriptions: allows printing debug statements given proper flags
//parameters: initflags - the flags passed in via the cli
//options: y shell -@@ - turns on debug w/ all options
//         y shell -@ - turns on debug w/ no other options
void debugflags::setflags (const string& initflags) {
   for (const unsigned char flag: initflags) {
      if (flag == '@') flags_.set();
                  else flags_.set (flag, true);
   }
}

// getflag -
//    Check to see if a certain flag is on.

bool debugflags::getflag (char flag) {
   // WARNING: Don't TRACE this function or the stack will blow up.
   return flags_.test (static_cast<unsigned char> (flag));
}

void debugflags::where (char flag, const char* file, int line,
                        const char* pretty_function) {
   cout << exec::execname() << ": DEBUG(" << flag << ") "
        << file << "[" << line << "] " << pretty_function << endl;
}
