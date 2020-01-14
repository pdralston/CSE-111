// $Id: bigint.cpp,v 1.2 2020-01-06 13:39:55-08 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0){
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_){
                  DEBUGF("~", this << "->" << uvalue)
}

bigint::bigint (const string& that) {
   // '_' signifies that a number is negative
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
  ubigint result; //the magnitude of the result
  boolean neg = false; //the negativity of the result
  if(this.is_negative == that.is_negative) {
    //case 1: A + B where A and B are the same sign.
    result.uvalue = this.uvalue + that.uvalue;
    neg = this.uvalue;
  }
  else {
    //case 2: A + B where either A or B is negative.
    boolean this_greater = this.uvalue > that.uvalue; //true if A > B.
    //making sure that mag(left hand operand) > mag(right hand operand) when doing ubigint subtraction
    result.uvalue = this_greater ? this.uvalue - that.uvalue : that.uvalue - this.uvalue;
    //if mag(left hand operand) > mag(right hand operand), then sign(result) = sign(left hand operator)
    //and vice-versa
    neg = this_greater ? this.is_negative : that.is_negative;
  }
  return new bigint(result, neg)
}

bigint bigint::operator- (const bigint& that) const {
   ubigint result;
   boolean neg = false;
   if(this.is_negative != that.is_negative) {
     //case 1: A - B where A and B are not the same sign.
     result.uvalue = this.uvalue + that.uvalue;
     //since A and B are not the same sign, if A < 0, then B > 0,
     //which means A - B = -(abs(A) + B)
     //and vice-versa which means A - B = A - (-B) = A + B
     neg = this.is_negative;
   }
   else {
     //case 2: A - B where A and B are the same sign.
     boolean this_greater = this.uvalue > that.uvalue;
     if (this.uvalue == that.uvalue) {
       //if A == B, then A - B = 0
       result.uvalue = 0;
       neg = false;
     }
     else if(this_greater) {
       //if mag(A) > mag(B), then abs(A - B) > 0
       result.uvalue = this.uvalue - that.uvalue;
       neg = this.is_negative;
     }
     else { //mag(B) > mag(A)
       //if mag_A < mag_B, then abs(A - B) < 0
       result.uvalue = that.uvalue - this.uvalue;
       neg = that.is_negative;
     }
   }
   return new bigint(result, neg);
}

bigint bigint::operator* (const bigint& that) const {
   bigint result = new bigint();
   result.is_negative = (this.is_negative || that.is_negative) && !(this.is_negative && that.is_negative);
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result = uvalue / that.uvalue;
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << "bigint(" << (that.is_negative ? "-" : "+")
              << "," << that.uvalue << ")";
}
