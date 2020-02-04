// $Id: file_sys.h,v 1.7 2019-07-09 14:05:44-07 - - $
// Sasank Madineni (smadinen)
// Perry Ralston (pdralsto)

#ifndef __INODE_H__
#define __INODE_H__

#include <exception>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
#include <sstream>
using namespace std;

#include "util.h"

// inode_t -
//    An inode is either a directory or a plain file.

enum class file_type {PLAIN_TYPE, DIRECTORY_TYPE};
class inode;
class base_file;
class plain_file;
class directory;
using inode_ptr = shared_ptr<inode>;
using base_file_ptr = shared_ptr<base_file>;
ostream& operator<< (ostream&, file_type);

// inode_state -
//    A small convenient class to maintain the state of the simulated
//    process:  the root (/), the current directory (.), and the
//    prompt.

class inode_state {
   friend class inode;
   friend ostream& operator<< (ostream& out, const inode_state&);
   private:
      inode_ptr root;
      inode_ptr cwd;
      string prompt_ {"% "};
   public:
      inode_state (const inode_state&) = delete; // copy ctor
      ~inode_state();
      inode_state& operator= (const inode_state&) = delete; // op=
      inode_state();
      const string& prompt() const;
      void prompt(const string& prompt);
      void make(wordvec&, wordvec&, bool = false, bool = false);
      void cd(wordvec&, bool = false, bool = false);
      const string pwd() const;
      const wordvec& cat(wordvec&, bool);
      const stringstream ls(wordvec&, bool);
      void rm(wordvec&, bool = false, bool = false);
};

// class inode -
// inode ctor -
//    Create a new inode of the given type.
// get_inode_nr -
//    Retrieves the serial number of the inode.  Inode numbers are
//    allocated in sequence by small integer.
// size -
//    Returns the size of an inode.  For a directory, this is the
//    number of dirents.  For a text file, the number of characters
//    when printed (the sum of the lengths of each word, plus the
//    number of words.
//

class inode {
   friend class inode_state;
   private:
      static int next_inode_nr;
      int inode_nr;
      base_file_ptr contents;
   public:
      inode (file_type);
      inode (file_type, const string&);
      int get_inode_nr() const;
      void invalidate(){contents = NULL;}
      int getSize();
      const string& getName();
      bool isDirectory();
};


// class base_file -
// Just a base class at which an inode can point.  No data or
// functions.  Makes the synthesized members useable only from
// the derived classes.

class file_error: public runtime_error {
   public:
      explicit file_error (const string& what);
};

class base_file {
   protected:
      base_file() = default;
      virtual const string error_file_type() const = 0;
   public:
      virtual ~base_file() = default;
      base_file (const base_file&) = delete;
      base_file& operator= (const base_file&) = delete;
      virtual size_t size() const = 0;
      virtual const wordvec& readfile() const;
      virtual void writefile (const wordvec& newdata);
      virtual void remove (const string& filename);
      virtual inode_ptr mkdir (const string& dirname);
      virtual inode_ptr mkfile (const string& filename);
      virtual void setDefs (const inode_ptr&, const inode_ptr&);
      virtual void setName (const string&);
      virtual const string& getName() const;
      virtual const inode_ptr& getEntry(const string&) const;
      virtual const string ls() const;
      virtual bool isDirectory() {return false;}
      virtual void rm(const string&);
};

// class plain_file -
// Used to hold data.
// synthesized default ctor -
//    Default vector<string> is a an empty vector.
// readfile -
//    Returns a copy of the contents of the wordvec in the file.
// writefile -
//    Replaces the contents of a file with new contents.

class plain_file: public base_file {
   private:
      wordvec data;
      virtual const string error_file_type() const override {
         return "plain file";
      }
      string filename_;
   public:
      virtual size_t size() const override;
      virtual const wordvec& readfile() const override;
      virtual void writefile (const wordvec& newdata) override;
      virtual void setName (const string&) override;
      virtual const string& getName() const override {return filename_;}
      virtual const string ls() const override;
};

// class directory -
// Used to map filenames onto inode pointers.
// default ctor -
//    Creates a new map with keys "." and "..".
// remove -
//    Removes the file or subdirectory from the current inode.
//    Throws an file_error if this is not a directory, the file
//    does not exist, or the subdirectory is not empty.
//    Here empty means the only entries are dot (.) and dotdot (..).
// mkdir -
//    Creates a new directory under the current directory and
//    immediately adds the directories dot (.) and dotdot (..) to it.
//    Note that the parent (..) of / is / itself.  It is an error
//    if the entry already exists.
// mkfile -
//    Create a new empty text file with the given name.  Error if
//    a dirent with that name exists.

class directory: public base_file {
   private:
      // Must be a map, not unordered_map, so printing is lexicographic
      map<string,inode_ptr> dirents;
      virtual const string error_file_type() const override {
         return "directory";
      }
      string dirname_;
   public:
      ~directory();
      virtual size_t size() const override;
      virtual void remove (const string& filename) override;
      virtual inode_ptr mkdir (const string& dirname) override;
      virtual inode_ptr mkfile (const string& filename) override;
      virtual void setDefs (const inode_ptr&, const inode_ptr&) override;
      virtual void setName (const string&) override;
      virtual const string& getName() const override {return dirname_;}
      virtual const inode_ptr& getEntry(const string&) const override;
      virtual const string ls() const override;
      virtual bool isDirectory() override {return true;}
      virtual void rm(const string&) override;
};

#endif
