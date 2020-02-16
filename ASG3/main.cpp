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

void analyze_string(string line, str_str_map *value_map) {

  regex comment_regex {R"(^\s*(#.*)?\s*$)"};
  regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
  regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};

  cout << endl << "input: " << line << endl;
  smatch result;
  if (regex_search (line, result, comment_regex)) {
     cout << "Comment or empty line." << endl;
     // continue;
  }
  if (regex_search (line, result, key_value_regex)) {
    if(result[1] == "" && result[2] == "") {
      cout << "Printing out listmap" << endl;
      for (str_str_map::iterator vitor = value_map->begin();
        vitor != value_map->end(); ++vitor) {
           cout << (*vitor) << endl;
      }
      cout << endl;
      // continue;
    }
    else if (result[1] == "") {
      cout << "Printing out listmap with values \'" << result[2]
         << "\'" << endl;
      for (str_str_map::iterator vitor = value_map->begin();
        vitor != value_map->end(); ++vitor) {
           if((*vitor).second == result[2] )
              cout << (*vitor) << endl;
      }
      cout << endl;
      // continue;
    }
     cout << "key  : \"" << result[1] << "\"" << endl;
     cout << "value: \"" << result[2] << "\"" << endl;
     str_str_pair pair (result[1], result[2]);
     cout << "adding "<< pair << " to value_map" << endl;
     value_map->insert(pair);
  }
  else if (regex_search (line, result, trimmed_regex)) {
     cout << "query: \"" << result[1] << "\"" << endl;
     str_str_map::iterator value_find = value_map->find(result[1]);
     if (value_find == value_map->end()) {
       cerr << "ERROR: Could not find key \'" << result[1]
         << "\' in listmap." << endl;
     }
     else {
       cout << "Found value: " << (*value_find) << endl;
     }
  }
  else {
     cerr << "ERROR: invalid option" << endl;
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

  regex comment_regex {R"(^\s*(#.*)?\s*$)"};
  regex key_value_regex {R"(^\s*(.*?)\s*=\s*(.*?)\s*$)"};
  regex trimmed_regex {R"(^\s*([^=]+?)\s*$)"};

  str_str_map value_map;
  for (str_str_map::iterator itor = test.begin();
       itor != test.end(); ++itor) {
     string filename = (*itor).first;

     if (((*itor).first).compare("-") == 0) {
       string line;
       cout << "Enter input line: " << endl;
       std::getline(cin, line);
       analyze_string(line, &value_map);
     }


     std::ifstream contents (filename);
     if(!contents.good()) {
       cerr << "ERROR: File not found" << endl;
       continue;
     }

     while (!contents.eof())  {
       string line;
       std::getline(contents, line);
       analyze_string(line, &value_map);
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
