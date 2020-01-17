// $Id: ubigint.cpp,v 1.8 2020-01-06 13:39:55-08 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <cmath>
#include <algorithm>
using namespace std;

#include "ubigint.h"
#include "debug.h"

const int BASE = 10;
const int MAX_DIGIT = 9;
const int MIN_DIGIT = 0;

/** Constructor
 *  Constructor takes an unsigned long and stores it as a vector
 *  of unsigned char values.
 *  @param that an unsigned long representing the numeric value of the
 *   ubigint
 */
ubigint::ubigint (unsigned long that) : carry(0){
   //DEBUGF ('~', this << " -> " << ubig_value)
   if (that == 0) return;
   ubig_value.push_back(that % BASE);
   for(uint i = BASE; i <= that; i*=BASE) {
      ubig_value.push_back(that / i % BASE);
   }
}

/** Constructor
 *  Constructor takes a string representation and stores it as a vector
 *  of unsigned char values equal to the digit values of the number. i.e.
 *  a '6' would be stored as 6 in the vector.
 *  @param that a string representation of the numeric value of the
 *   ubigint
 */
ubigint::ubigint (const string& that) : carry(0){
   DEBUGF ('~', "that = \"" << that << "\"");
   for_each(that.rbegin(), that.rend(), [this, that] (char const &digit) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.push_back(digit - '0');
   });
}

/** Operator+=
 *  Add two ubigints in place.
 *  @param that ubigint to be added to this
 */
void ubigint::operator+= (const ubigint& that) {
   unsigned int index = 0;
   //iterate from LSB to MSB of this and add corresponding digits of that.
   for (; index < ubig_value.size(); index++) {
      ubig_value[index] += that.ubig_value.size() < index ? carry + that.ubig_value[index] : carry;
      carry = 0;
      if (ubig_value[index] > MAX_DIGIT) {
         carry = 1;
         ubig_value[index] -= BASE;
      }
   }
   //deal with the case where that has more digits than this
   while (index < that.ubig_value.size()) {
      ubig_value.push_back(carry + that.ubig_value[index]);
      carry = 0;
      index++;
   }
   //deal with dangling carry over
   if (carry != 0) {
      ubig_value.push_back(carry);
   }
}

/** Operator-=
 *  Subtract two ubigints in place.
 *  @param that ubigint to be subtracted from this
 */
void ubigint::operator-= (const ubigint& that) {
   unsigned int index = 0;
   int temp;
   //iterate from LSB to MSB of this and subtract corresponding digits of that.
   for (; index < ubig_value.size(); index++) {
      temp = that.ubig_value.size() < index ? 
         ubig_value[index] - that.ubig_value[index] - carry : -carry;
      carry = 0;
      if (temp < 0) {
         carry = 1;
         temp += BASE;
      }
      ubig_value[index] = temp;
   }
   //dangling carry is not be possible since this is unsigned arithmetic
   //and the caller is responsible for not calling this function A -= B
   // where A > B
   
   //deal with case of leading zeroes
   this->clearZeroes();
}

ubigint ubigint::operator+ (const ubigint& that) const {
   ubigint sum;
   unsigned int index = 0;
   for (; index < ubig_value.size(); index++) {
      sum.ubig_value.push_back(that.ubig_value.size() < index ?
                    sum.carry + ubig_value[index] + that.ubig_value[index] :
                    sum.carry + ubig_value[index]);
      sum.carry = 0;
      if (ubig_value[index] > MAX_DIGIT) {
         sum.carry = 1;
         sum.ubig_value[index] -= BASE;
      }
   }
   //deal with the case where that has more digits than this
   while (index < that.ubig_value.size()) {
      sum.ubig_value.push_back(sum.carry + that.ubig_value[index]);
      sum.carry = 0;
      index++;
   }
   //deal with dangling carry over
   if (sum.carry != 0) {
      sum.ubig_value.push_back(sum.carry);
   }
   return sum;
}

ubigint ubigint::operator- (const ubigint& that) const {
//TODO define operator< and uncomment   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigint diff;
   unsigned int index = 0;
   for (; index < ubig_value.size(); index++) {
      diff.ubig_value.push_back(that.ubig_value.size() < index ?
                    diff.carry + ubig_value[index] + that.ubig_value[index] :
                    diff.carry + ubig_value[index]);
      diff.carry = 0;
      if (ubig_value[index] > MAX_DIGIT) {
         diff.carry = 1;
         diff.ubig_value[index] -= BASE;
      }
   }
   
   //dangling carry is not be possible since this is unsigned arithmetic
   //and the caller is responsible for not calling this function C = A - B
   // where A > B
   diff.clearZeroes();
   return diff;
}

/*
ubigint ubigint::operator* (const ubigint& that) const {
   return ubigint (ubig_value * that.ubig_value);
}

void ubigint::multiply_by_2() {
   ubig_value *= 2;
}

void ubigint::divide_by_2() {
   ubig_value /= 2;
}

struct quo_rem { ubigint quotient; ubigint remainder; };
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   //this is defined for vectors and works as expected
   return ubig_value == that.ubig_value;
}

bool ubigint::operator< (const ubigint& that) const {
   bool isLess = ubig_value.size() <= that.ubig_value.size()
                 and not(*this == that);
   if (isLess) {
      if (ubig_value.size() == that.ubig_value.size()) {
         using charIter = vector<unsigned char>::const_reverse_iterator;
         for (pair<charIter, charIter> i(ubig_value.crbegin(), that.ubig_value.crbegin()); 
              i.first != ubig_value.crend(); ++i.first, ++i.second) {
            if (*(i.second) < *(i.first)) {
               isLess = false;
               break;
            }
         }
      }
   }
}

ostream& operator<< (ostream& out, const ubigint& that) {
   for (auto it = that.ubig_value.crbegin(); it != that.ubig_value.crend(); ++it) {
      out << static_cast<unsigned>(*it);
   }
   return out;
}

void ubigint::clearZeroes() {
  while (ubig_value.size() > 0 and ubig_value.back() == 0) {
    ubig_value.pop_back();
  }
}
