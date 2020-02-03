// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $
// Sasank Madineni (smadinen)
// Perry Ralston (pdralsto)

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <sstream>
#include <iterator>
#include <iomanip>

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

inode_state::~inode_state() {
   root->invalidate();
}


const string& inode_state::prompt() const { return prompt_; }


void inode_state::prompt(const string& prompt) { prompt_ = prompt; }


void inode_state::make(wordvec& pathname, wordvec& data, bool relToRoot, bool makeDir){
   string toMake = pathname.back();
   inode_ptr temp = cwd;
   pathname.pop_back();
   cd(pathname, relToRoot);
   if (makeDir) {
      inode_ptr newDir = cwd->contents->mkdir(toMake);
      newDir->contents->setDefs(cwd, newDir);
      newDir->contents->setName(toMake);
   } else {
      inode_ptr newFile = cwd->contents->mkfile(toMake);
      newFile->contents->writefile(data);
      newFile->contents->setName(toMake);
   }
   cwd = temp;
}

void inode_state::cd(wordvec& pathname, bool relToRoot, bool fileOk) {
   if (pathname.size() == 0) {
      cwd = root;
      return;
   }
   inode_ptr temp = cwd;
   if (relToRoot) {
      cwd = root;
      relToRoot = false;
   }
   try {
      for (auto direc :  pathname) {
         cwd = cwd->contents->getEntry(direc);
         if (!fileOk && !cwd->contents->isDirectory()) {
            throw file_error (pathname[0] + " is not a valid directory");
         }
      }
   } catch (...){//TODO fix this error catching design, its bad
      //restore the cwd when cd fails.
      cwd = temp;
      throw file_error (pathname[0] + " is not a valid directory");
   }
}

const wordvec& inode_state::cat(wordvec& pathname, bool relToRoot) {
   string filename = pathname.back();
   inode_ptr fileNode, temp = cwd;
   pathname.pop_back();
   try {
      cd(pathname, relToRoot);
   } catch (file_error& error) {
      throw error;
   }
   try {
      fileNode = cwd->contents->getEntry(filename);
      cwd = temp;
      return fileNode->contents->readfile();
   } catch(int) {
      cwd = temp;
      throw file_error (filename + " does not exist.");
   }
}

const stringstream inode_state::ls(wordvec& pathname, bool relToRoot) {
   stringstream lsStream;
   inode_ptr temp = cwd;
   cd(pathname, relToRoot, true);
   lsStream << cwd->contents->ls();
   cwd = temp;
   return lsStream;
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

int inode::getSize() {
   return contents->size();
}

const string& inode::getName() {
   return contents->getName();
}

bool inode::isDirectory() {
   return contents->isDirectory();
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

const inode_ptr& base_file::getEntry(const string&) const {
   throw file_error ("is a " + error_file_type());
}

const string base_file::ls() const {
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

const string plain_file::ls() const {
   stringstream fileListing;
   fileListing << "  " << setw(6) << right << size() << "  " << getName() << endl;
   return fileListing.str();
}

directory::~directory() {
   for (auto dirEntry : dirents) {
      dirEntry.second->invalidate();
   }
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
   dirname_ = dirname + "/";
}

const inode_ptr& directory::getEntry(const string& dirname) const{
      return dirents.at(dirname);
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   dirents.insert({filename, make_shared<inode>(file_type::PLAIN_TYPE)});
   return dirents.at(filename);
}

const string directory::ls() const {
   stringstream entries;
   for (auto entry: dirents) {
      entries << setw(6) << right << entry.second->get_inode_nr()
              << "  " << setw(6) << right << entry.second->getSize()
              << "  " << entry.first;
      if (entry.second->isDirectory()){
         entries << "/";
      }
      entries << endl;
   }
   return entries.str();
}
