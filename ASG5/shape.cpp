// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $
// Sasank Madineni (smadinen)
// Perry Ralston (pdralsto)

#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"
#include <math.h>
#include <map>

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

static int shape_count = 0;
static int selected_shape;
static GLfloat border = 4;
static rgbcolor default_color ("red");


ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

void shape::object_select(GLubyte select) {
   selected_shape = select;
}

void shape::set_color(rgbcolor color) {
   default_color = color;
}

void shape::set_thickness(GLfloat thickness) {
  border = thickness;
}

void draw_label(const vertex& center) {
   ostringstream text;
   text << shape_count;
   DEBUGF ('c', "draw label has been called and '" << text.str()
      << "' should be printed on the shape");
   void* font = GLUT_BITMAP_HELVETICA_18;
   glColor3ubv (default_color.ubvec);
   glRasterPos2i (center.xpos, center.ypos);
   auto ubytes = reinterpret_cast<const GLubyte*>
                 (text.str().c_str());
   glutBitmapString (font, ubytes);
   ++shape_count;
}

shape::shape() {
   DEBUGF ('c', this);
}

void shape::reset_counter() {
   shape_count = 0;
}

text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

text::text (string glut_bitmap_font_, const string& textdata_) {
  DEBUGF ('c', this);
  try {
     glut_bitmap_font = fontcode.at(glut_bitmap_font_);
  }
  catch (const exception &err) {
     throw runtime_error("invalid bitmap font");
  }
  textdata = textdata_;
}

ellipse::ellipse (GLfloat width, GLfloat height):
   dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}

polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   vertex temp;
   temp.xpos = width/2;
   temp.ypos = height/2;
   vertices.push_back(temp);
   temp.xpos = -width/2;
   vertices.push_back(temp);
   temp.ypos = -height/2;
   vertices.push_back(temp);
   temp.xpos = width/2;
   vertices.push_back(temp);
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond (GLfloat width, GLfloat height):
            polygon({}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
   vertex temp;
   temp.ypos = height/2;
   vertices.push_back(temp);
   temp.ypos = 0;
   temp.xpos = width/2;
   vertices.push_back(temp);
   temp.ypos = -height/2;
   temp.xpos = 0;
   vertices.push_back(temp);
   temp.ypos = 0;
   temp.xpos = -width/2;
   vertices.push_back(temp);
}

triangle::triangle (const vertex_list& vertices_): polygon(vertices_) {
   DEBUGF ('c', this);
}

equilateral::equilateral (GLfloat width) : triangle({}) {
   DEBUGF ('c', this << "(" << width << ")");
   vertex temp;
   GLfloat height = sqrt((width * width) - (width * width) / 4);
   temp.ypos = height/2;
   vertices.push_back(temp);
   temp.ypos = -height/2;
   temp.xpos = width/2;
   vertices.push_back(temp);
   temp.xpos = -width/2;
   vertices.push_back(temp);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   shape_count++;

   if (selected_shape == shape_count - 1) {
      glBegin(GL_LINE_LOOP);
      glColor3ubv(default_color.ubvec);
      glLineWidth(border);
      glVertex2f(center.xpos + 10, center.ypos + 10);
      glVertex2f(center.xpos - 10, center.ypos + 10);
      glVertex2f(center.xpos - 10, center.ypos - 10);
      glVertex2f(center.xpos + 10, center.ypos - 10);
      glEnd();
   }

   auto text_data = reinterpret_cast<const GLubyte*> (textdata.c_str());
   glColor3ubv(color.ubvec);
   glRasterPos2f (center.xpos, center.ypos);
   glutBitmapString (glut_bitmap_font, text_data);
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   const float delta = 2 * M_PI / 32;
   glColor3ubv(color.ubvec);
   for (float theta = 0; theta < 2 * M_PI; theta += delta) {
      float xpos = dimension.xpos * cos (theta) + center.xpos;
      float ypos = dimension.ypos * sin (theta) + center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();
   if (shape_count == selected_shape) {
      glBegin(GL_LINE_LOOP);
      glColor3ubv(default_color.ubvec);
      glLineWidth(border);
      for (float theta = 0; theta < 2 * M_PI; theta += delta) {
         float xpos = dimension.xpos * cos (theta) + center.xpos;
         float ypos = dimension.ypos * sin (theta) + center.ypos;
         glVertex2f (xpos, ypos);
      }
      glEnd();
   }
   draw_label(center);
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   glBegin(GL_POLYGON);
   glColor3ubv(color.ubvec);
   vertex average {0, 0};
   int count = 0;
   //normalizing points
   for (const vertex&point : vertices) {
     average.xpos += point.xpos;
     average.ypos += point.ypos;
     count++;
   }

   average.ypos /= count;
   average.xpos /= count;

   for (const vertex& point : vertices) {
      vertex temp {0, 0};
      temp.xpos = point.xpos - average.xpos + center.xpos;
      temp.ypos = point.ypos - average.xpos + center.ypos;
      DEBUGF('d', this << " xpos: " << temp.xpos
         << ", ypos: " << temp.ypos);
      glVertex2f(temp.xpos, temp.ypos);
   }
   glEnd();

   if (selected_shape == shape_count) {
      glBegin(GL_LINE_LOOP);
      glColor3ubv(default_color.ubvec);
      glLineWidth(border);
      for (const vertex& point : vertices) {
         vertex temp {0, 0};
         temp.xpos = point.xpos - average.xpos + center.xpos;
         temp.ypos = point.ypos - average.xpos + center.ypos;
         DEBUGF('d', this << " xpos: " << temp.xpos
            << ", ypos: " << temp.ypos);
         glVertex2f(temp.xpos, temp.ypos);
      }
      glEnd();
   }
   draw_label(center);
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}
