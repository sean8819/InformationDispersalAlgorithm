/**
 * @file PrimeField.cpp
 * @brief Implementation of the PrimeField class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Field/PrimeField.h"

#include <limits>
#include <stdexcept>

namespace ida {

namespace {

/**
 * @brief Tells whether a number is prime, by trial division.
 * @param number Number to test.
 * @return true if @p number is a prime number.
 */
[[nodiscard]] bool isPrime(int number) {

    if (number < 2)
        return (false);

    //The condition divisor <= number / divisor is divisor * divisor <= number, without its overflow near the
    //largest int.
    for (int divisor = 2; divisor <= number / divisor; divisor++)
        if (number % divisor == 0)
            return (false);

    return (true);
}

} //namespace

PrimeField::PrimeField(int p) : p(p) {

    //The modulus must be prime, otherwise the integers modulo p are not a field and some elements have no inverse.
    if (!isPrime(p))
        throw std::invalid_argument("The modulus p must be a prime number");
}

long PrimeField::add(long a, long b) const {
    return ((a + b) % this->p);
}

long PrimeField::subtract(long a, long b) const {

    //Add p before the last modulo so that the result is never negative.
    return ((((a - b) % this->p) + this->p) % this->p);
}

long PrimeField::multiply(long a, long b) const {

    //Two elements are below p, which is an int: their product must fit in a long.
    static_assert(std::numeric_limits<long>::digits >= 2 * std::numeric_limits<int>::digits,
                  "The product of two elements of GF(p) must fit in a long");

    return ((a * b) % this->p);
}

int PrimeField::getSize() const {
    return (this->p);
}

} //namespace ida
