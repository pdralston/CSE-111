// $Id: rgbcolor.h,v 1.3 2019-03-19 15:53:52-07 - - $
// Sasank Madineni (smadinen)
// Perry Ralston (pdralsto)

#ifndef __RGBCOLOR_H__
#define __RGBCOLOR_H__

#include <string>
#include <unordered_map>
using namespace std;

#include <GL/freeglut.h>

struct rgbcolor {
   union {
      GLubyte ubvec[3];
      struct {
         GLubyte red {};
         GLubyte green {};
         GLubyte blue {};
      } rgb {};
   };
   explicit rgbcolor() = default;
   explicit rgbcolor (GLubyte red, GLubyte green, GLubyte blue):
               rgb ({red, green,blue}) {};
   explicit rgbcolor (const string&);
   operator string() const;
};

ostream& operator<< (ostream&, const rgbcolor&);

extern const std::unordered_map<std::string,rgbcolor> color_names;

#endif

