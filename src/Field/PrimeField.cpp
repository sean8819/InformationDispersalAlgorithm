/**
 * @file PrimeField.cpp
 * @brief Implementation of the PrimeField class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Field/PrimeField.h"

PrimeField::PrimeField(int p) {

    //p must be prime, otherwise the integers modulo p are not a field and some elements have no inverse.
    bool prime = p >= 2;

    for (int divisor = 2; divisor * divisor <= p && prime; divisor++)
        if (p % divisor == 0)
            prime = false;

    if (!prime) {
        std::cerr << "The modulus p must be a prime number" << std::endl;
        exit(EXIT_FAILURE);
    }

    this->p = p;
}

long PrimeField::add(long a, long b) {
    return ((a + b) % this->p);
}

long PrimeField::sub(long a, long b) {

    //Add p before the last modulo so that the result is never negative.
    return ((((a - b) % this->p) + this->p) % this->p);
}

long PrimeField::mul(long a, long b) {
    return ((a * b) % this->p);
}

int PrimeField::getSize() const {
    return (this->p);
}
