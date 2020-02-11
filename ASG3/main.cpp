// $Id: main.cpp,v 1.11 2018-01-25 14:19:29-08 - - $

#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <regex>

using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

/*
* function: scan_options
* description: gets arguments from cli and activated debug options
* arguments: argc - the number of cli arguments
             argv - pointer to arguments being passed
*/
void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         default:
            complain() << "-" << char (optopt) << ": invalid option"
                       << endl;
            break;
      }
   }
}

/*
* function: main
* description: takes in input filenames and parses it
               performing a specific action based on what
               value is passed
* arguments: argc - the number of cli arguments
             argv = pointer to the arguments being passed
* TODO: - regex search to determine line type
        - call correct function based on line type
*/

int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);

   str_str_map test;
   for (char** argp = &argv[optind]; argp != &argv[argc]; ++argp) {
      str_str_pair pair (*argp, to_string<int> (argp - argv));
      cout << "Before insert: " << pair << endl;
      test.insert(pair);
   }

   cout << "test is empty?: " << test.empty() << endl;

  regex comment_regex {R"(^\s*(#.*)?$)"};
  regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
  regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};

  //TEST
  for (str_str_map::iterator itor = test.begin(); itor != test.end(); ++itor) {
    cout << (*itor).first << endl;
  }

  std::ifstream testin ("Testfiles/test1.in");
  while(!testin.eof()) {
    string line;
    std::getline(testin, line);
    cout << line << endl;
  }

  for (str_str_map::iterator itor = test.begin();
       itor != test.end(); ++itor) {
     string filename = (*itor).first;
     std::ifstream contents (filename);

     while (!contents.eof())  {
       string line;
       std::getline(contents, line);
       if (line == "-") {
         std::getline (cin, line);
       }
       cout << endl << "input: \"" << line << "\"" << endl;
       smatch result;
       if (regex_search (line, result, comment_regex)) {
          cout << "Comment or empty line." << endl;
          continue;
       }
       if (regex_search (line, result, key_value_regex)) {
          cout << "key  : \"" << result[1] << "\"" << endl;
          cout << "value: \"" << result[2] << "\"" << endl;
       }
       else if (regex_search (line, result, trimmed_regex)) {
          cout << "query: \"" << result[1] << "\"" << endl;
       }
       else {
          cout << "ERROR: invalid option" << endl; //TODO: change to error throw
       }
     }
  }

   for (str_str_map::iterator itor = test.begin();
        itor != test.end(); ++itor) {
      cout << "During iteration: " << *itor << endl;
   }

   str_str_map::iterator itor = test.begin();
   test.erase (itor);

   cout << "EXIT_SUCCESS" << endl;
   return EXIT_SUCCESS;
}
