// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $
// Sasank Madineni (smadinen)
// Perry Ralston (pdralsto)

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <sstream>
#include <iterator>

using namespace std;

static const string PARENT = "..";
static const string ROOT = "/\n";
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

void inode_state::make(wordvec& pathname, wordvec& data, bool relToRoot = false){
   string filename = pathname.back();
   inode_ptr temp = cwd;
   pathname.pop_back();
   cd(pathname, relToRoot);
   (cwd->contents->mkfile(filename))->contents->writefile(data);
   cwd = temp;
}

void inode_state::mkdir(string& dirname) {
   cwd->contents->mkdir(dirname);
}

void inode_state::cd(wordvec& pathname, bool relToRoot = false) {
   if (relToRoot) {
      cwd = root;
      relToRoot = false;
   }
   try {
      for (auto direc :  pathname) {
         cwd = cwd->contents->getdir(direc);
      }
   } catch (int){
      stringstream pathString;
      copy(pathname.begin(), pathname.end(), ostream_iterator<string>(pathString, "/"));
      throw file_error (pathname[0] + " is not a valid directory");
   }
   
}

const string& inode_state::pwd() const {
   if (cwd == root) {
      return ROOT;
   }
   return cwd->contents->getName();
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

const string& base_file::getName() const{
   throw file_error ("is a " + error_file_type());
}

const inode_ptr base_file::getdir(const string&) {
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
   dirents.clear();
}

size_t directory::size() const {
   size_t size {dirents.size()};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   if(filename == PARENT) {
      throw file_error("Unable to delete root directory");
   }
   if(filename == SELF) {
      throw file_error("Unable to delete current working directory");
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

const inode_ptr directory::getdir(const string& dirname) {
      return dirents.at(dirname);
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   dirents.insert({filename, make_shared<inode>(file_type::PLAIN_TYPE)});
   return dirents.at(filename);
}
