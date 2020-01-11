// $Id: ubigint.h,v 1.4 2020-01-06 13:39:55-08 - - $

#ifndef __UBIGINT_H__
#define __UBIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
using namespace std;

#include "debug.h"
#include "relops.h"

//Unsigned Big Integer Class
class ubigint {
   friend ostream& operator<< (ostream&, const ubigint&);
   private:
      using unumber = unsigned long;
      unumber uvalue {}; //magnitude of ubigint
   public:
      //function used to multiply by 2 (bitshift left)
      void multiply_by_2();
      //function used to divide by 2 (bitshift right)
      void divide_by_2();

      ubigint() = default; // Need default ctor as well.
      ubigint (unsigned long);
      ubigint (const string&);

      ubigint operator+ (const ubigint&) const;
      ubigint operator- (const ubigint&) const;
      ubigint operator* (const ubigint&) const;
      ubigint operator/ (const ubigint&) const;
      ubigint operator% (const ubigint&) const;

      bool operator== (const ubigint&) const;
      bool operator<  (const ubigint&) const;
};

#endif
