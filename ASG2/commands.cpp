// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $
// Sasank Madineni (smadinen)
// Perry Ralston (pdralsto)

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

//function: exit_status_message (int)
//description: prints out current [exec::status()] and exits
int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

//function: fn_cat
//description: prints out contents of passed in file
//parameters: state -
//            words -
void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //cout << state.read_file();
}

//function: fn_cd
//description: changes directory to subdirectory <>
//             or goes to directory <> if / is the first character
//parameters: state -
//            words -
void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //state.set_dir(wordvec(word_range(words.cbegin() + 1, words.cend())));
}

//function: fn_echo
//description: outputs <words> to sysout
//parameters: state -
//            words - string to print
void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}

//function: fn_exit
//description: exits the program
//parameters: state -
//            words -
void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

//function: fn_ls
//description: lists all files in the current dir in the file_sys
//parameters: state -
//            words -
void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //state.get_dir_contents()
}

//function: fn_lsr
//description: recursively show directories and subdirectories
//parameters: state -
//            words -
void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //inode_state current_dir = state.get_dir()
   //vector<string> dir;
   //<recursively loop through every directory>
   //   fn_ls(current_state, words)
}

//function: fn_make
//description: makes a file with name <> containing <>
//parameters: state -
//            words -
void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //string filename = state.
}

//function: fn_mkdir
//description: makes a new subdirectory from the current directory
//             in the filesys
//parameters: state -
//            words -
void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //string dir_name = wordvec(words.cbegin() + 1, words.cend());
   //state.create_child(dir_name);
}

//function: fn_prompt
//description: changes the prompt to the following string
//             default prompt is '%'
//parameters: state -
//            words -
void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //state.set_prompt(words.cbegin() + 1, words.cend());
}

//functions: fn_pwd
//description: prints out current directory path
//parameters: state -
//            words -
void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //string directory_path = ""
   //current_node = state.get_node()
   //while current_node.get_dir() != current_node.get_root():
   //   directory_path = current_node.get_path "/" + directory_path
   //cout << directory_path
}

//function: fn_rm
//description: removes file based on given filename
//parameters: state -
//            words -
void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //filename = wordvec(words.cbegin() + 1, words.cend())
   //state.remove_file()
}

//function: fn_rmr
//description: remove files recursively in given directory
//parameters: state -
//            words -
void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   //implemented similar to lsr, but rm instead.
   //I guess I just call a recuirsive function that you implement
}
