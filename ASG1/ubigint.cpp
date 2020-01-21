// $Id: ubigint.cpp,v 1.8 2020-01-06 13:39:55-08 - - $
// Sasank Madineni (smadinen)
// Perry Ralston (pdralsto)
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
 *  of udigit_t values.
 *  @param that an unsigned long representing the numeric value of the
 *   ubigint
 */
ubigint::ubigint (unsigned long that){
   //DEBUGF ('~', this << " -> " << ubig_value)
   if (that == 0) return;
   ubig_value.push_back(that % BASE);
   for(uint iter = BASE; iter <= that; iter*=BASE) {
      ubig_value.push_back(that / iter % BASE);
   }
}

/** Constructor
 *  Constructor takes a string representation and stores it as a vector
 *  of udigit_t values equal to the digit values of the number. i.e.
 *  a '6' would be stored as 6 in the vector.
 *  @param that a string representation of the numeric value of the
 *   ubigint
 */
ubigint::ubigint (const string& that){
   DEBUGF ('~', "that = \"" << that << "\"");
   for_each(that.rbegin(), that.rend(), [this, that]
           (char const &digit) {
              if (not isdigit (digit)) {
                 throw invalid_argument (
                  "ubigint::ubigint(" + that + ")");
              }
              ubig_value.push_back(digit - '0');
           });
}

/** Operator*
 *  Returns the result of multiplying two unsigned bigints
 * @param big into to multiply this by
 * @return a new bigint representing the product of this and that
 */
ubigint ubigint::operator* (const ubigint& that) const {
   ubigint product;
   //An empty vector signifies a value of 0.
   if (ubig_value.size() > 0 and that.ubig_value.size() > 0) {
      udigit_t carry, interimProd;
      //fill product vector with 0's equal in number
      //to sum of num digits in args
      fill_n(back_inserter(product.ubig_value),
             ubig_value.size() + that.ubig_value.size(), 0);
      for(unsigned iter = 0; iter < ubig_value.size(); ++iter) {
         carry = 0;
         for(unsigned jiter = 0; jiter < that.ubig_value.size();
             ++jiter) {
                interimProd = product.ubig_value[iter+jiter]
                            + ubig_value[iter]
                            * that.ubig_value[jiter] + carry;
                product.ubig_value[iter+jiter] = interimProd % BASE;
                carry = interimProd / BASE;
             }
         //carry will always be less than MAX_DIGIT
         //since 9*9 = 81 and 81 / 10 = 8
         product.ubig_value[iter+that.ubig_value.size()] = carry;
      }
   }
   product.clearZeroes();
   return product;
}

/** multiply_by_2
 *  multiply this in place by 2
 */
void ubigint::multiply_by_2() {
   udigit_t carry = 0;
   for (auto digit = ubig_value.begin();
        digit != ubig_value.end(); ++digit) {
            *digit = *digit * 2 + carry;
            carry = *digit / BASE;
            *digit %= BASE;
         }
   if (carry != 0) {
      ubig_value.push_back(carry);
   }
}

/** divide_by_2
 *  divide this in place by 2
 */
void ubigint::divide_by_2() {
   udigit_t carry = 0;
   for (auto digit = ubig_value.begin();
        digit != ubig_value.end(); ++digit) {
        //*digit.next % 2 is 1 (true) when the next digit is odd.
        auto nextDigit = next(digit);
        if (nextDigit < ubig_value.end() and *nextDigit % 2) {
           carry = 5;
        }
        *digit = *digit / 2 + carry;
        carry = 0;
    }
   clearZeroes();
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

/** Operator/
 *  Returns the quotient result of dividing two unsigned bigints
 * @param that bigint into to divide this by
 * @return a new bigint representing the quotient of this and that
 */
ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

/** Operator%
 *  Returns the remainder result of dividing two unsigned bigints
 * @param that bigint to divide this by
 * @return a new bigint representing the remainder of this / that
 */
ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

/** Operator+=
 *  Add two ubigints in place.
 *  @param that ubigint to be added to this
 */
void ubigint::operator+= (const ubigint& that) {
   unsigned int index = 0;
   int carry = 0;
   //iterate from LSB to MSB of this and
   //add corresponding digits of that.
   for (; index < ubig_value.size(); index++) {
      ubig_value[index] += that.ubig_value.size() < index ?
                           carry + that.ubig_value[index] : carry;
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
   int carry = 0;
   int temp;
   //iterate from LSB to MSB of this and
   //subtract corresponding digits of that.
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
   //where A > B

   //deal with case of leading zeroes
   this->clearZeroes();
}

ubigint ubigint::operator+ (const ubigint& that) const {
   ubigint sum;
   unsigned int index = 0;
   udigit_t carry {};
   for (; index < ubig_value.size(); index++) {
      sum.ubig_value.push_back(index < that.ubig_value.size() ?
                    carry + ubig_value[index] + that.ubig_value[index] :
                    carry + ubig_value[index]);
      carry = 0;
      if (ubig_value[index] > MAX_DIGIT) {
         carry = 1;
         sum.ubig_value[index] -= BASE;
      }
   }
   //deal with the case where that has more digits than this
   while (index < that.ubig_value.size()) {
      sum.ubig_value.push_back(carry + that.ubig_value[index]);
      carry = 0;
      index++;
   }
   //deal with dangling carry over
   if (carry != 0) {
      sum.ubig_value.push_back(carry);
   }
   return sum;
}

ubigint ubigint::operator- (const ubigint& that) const {
   //if (*this < that) throw
   //domain_error ("ubigint::operator-(a<b)");
   ubigint diff;
   unsigned int index = 0;
   udigit_t carry {};
   for (; index < ubig_value.size(); index++) {
      diff.ubig_value.push_back(index < that.ubig_value.size() ?
                    ubig_value[index] - that.ubig_value[index] - carry :
                    ubig_value[index] - carry);
      carry = 0;
      if (ubig_value[index] > MAX_DIGIT) {
         carry = 1;
         diff.ubig_value[index] -= BASE;
      }
   }

   //dangling carry is not be possible since this is unsigned arithmetic
   //and the caller is responsible for not calling this function
   //C = A - B where A > B
   diff.clearZeroes();
   return diff;
}

bool ubigint::operator== (const ubigint& that) const {
   //this is defined for vectors and works as expected
   return ubig_value == that.ubig_value;
}

bool ubigint::operator< (const ubigint& that) const {
   bool isLess = ubig_value.size() <= that.ubig_value.size() && *this != that;
   if (isLess) {
      if (ubig_value.size() == that.ubig_value.size()) {
         using charIter = vector<udigit_t>::const_reverse_iterator;
         for (pair<charIter, charIter> iterPair(ubig_value.crbegin(),
                                         that.ubig_value.crbegin());
              iterPair.first != ubig_value.crend(); 
              ++iterPair.first, ++iterPair.second) {
            if (*iterPair.second > *iterPair.first) {
               isLess = true;
               break;
            }
            if (*iterPair.first > *iterPair.second) {
               isLess = false;
               break;
            }
         }
      }
   }
   cout << *this << " < " << that << " = " << boolalpha << isLess << endl;
   return isLess;
}

ostream& operator<< (ostream& out, const ubigint& that) {
   int count = 1;
   if (that.ubig_value.size() == 0) {
      out << 0;
   }
   for (auto it = that.ubig_value.crbegin();
      it != that.ubig_value.crend(); ++it) {
      if (count == 70) { out << "/\n"; count = 1;}
         out << static_cast<unsigned>(*it);
         count++;
   }
   return out;
}

void ubigint::clearZeroes() {
  while (ubig_value.size() > 0 and ubig_value.back() == 0) {
    ubig_value.pop_back();
  }
}
