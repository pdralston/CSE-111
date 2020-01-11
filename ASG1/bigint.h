// $Id: bigint.h,v 1.2 2020-01-06 13:39:55-08 - - $

#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"


//Big Integer Class Definition
class bigint {
   friend ostream& operator<< (ostream&, const bigint&);
   private:
      ubigint uvalue; //magnitude of bigint
      bool is_negative {false}; //sign of bigint
   public:

      bigint() = default; // Needed or will be suppressed.
      bigint (long);
      bigint (const ubigint&, bool is_negative = false);
      explicit bigint (const string&);

      //unary operation, used to determine if bigint is positive.
      bigint operator+() const;
      //unary operation, used to determine if bigint is negative.
      bigint operator-() const;

      //binary operation, used to sum two bigints and return a bigint.
      bigint operator+ (const bigint&) const;
      //binary operation, used to subtract two bigints and return a bigint.
      bigint operator- (const bigint&) const;
      //binary operation, used to multiply two bigints and return a bigint.
      bigint operator* (const bigint&) const;
      //binary operation, used to divide two bigints and return a bigint.
      bigint operator/ (const bigint&) const;
      //binary operation, used to divide two bigints and return the remainder as a bigint.
      bigint operator% (const bigint&) const;

      //binary operation, used to check the equality of two bigints and return the result as a boolean.
      bool operator== (const bigint&) const;
      //binary operation, used to check if one bigint is less than another bigint and return the result as a boolean.
      bool operator<  (const bigint&) const;
};

#endif
