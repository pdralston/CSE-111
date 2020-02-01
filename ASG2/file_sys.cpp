// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $
// Sasank Madineni (smadinen)
// Perry Ralston (pdralsto)

#include <iostream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

static const string PARENT = "..";
static const string SELF = ".";

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

//operation: <<
//description: overloaded << operator, allows printing
//
ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}


inode_state::inode_state() {
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;
   root->contents->setDefs(root, root);
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}


const string& inode_state::prompt() const { return prompt_; }


void inode_state::prompt(const string& prompt) { prompt_ = prompt + " "; }

void inode_state::mkdir(string& dirname) {
   cwd->contents->mkdir(dirname);
}

void inode_state::pwd() {
   if (cwd == root) {
      cout << "/\n";
      return;
   }
   cwd->contents->printName();
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

inode::inode (file_type type, const string& name) : inode(type) {
   contents->setName(name);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

//function: file_error
//description: calls a runtime_error with the description <what>
file_error::file_error (const string& what):
            runtime_error (what) {
}

//function:
const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

//function: writefile
//description: 
void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::setDefs (const inode_ptr&, const inode_ptr&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::setName (const string&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::printName() {
   throw file_error ("is a " + error_file_type());
}

size_t plain_file::size() const {
   size_t size {0};
   //increment 
   for (auto word : data) {
     size += word.length();
   }
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   for (auto word : words) {
     data.push_back(word);
   }
}

void plain_file::setName (const string& filename) {
   filename_ = filename;
}

directory::~directory() {
   dirents.erase(SELF);
   dirents.erase(PARENT);
   cout << dirents.size();
}

size_t directory::size() const {
   size_t size {dirents.size()};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   if(filename == PARENT) {
      file_error("Unable to delete root directory");
      return;
   }
   if(filename == SELF) {
      file_error("Unable to delete current working directory");
      return;
   }
   dirents.erase(filename);
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   dirents.insert({dirname, make_shared<inode>(file_type::DIRECTORY_TYPE, dirname)});
   return dirents.at(dirname);
}

void directory::setDefs (const inode_ptr& parent, const inode_ptr& self) {
   dirents.insert({PARENT, parent});
   dirents.insert({SELF, self});
}

void directory::setName (const string& dirname) {
   dirname_ = dirname;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   dirents.insert({filename, make_shared<inode>(file_type::PLAIN_TYPE)});
   return dirents.at(filename);
}
