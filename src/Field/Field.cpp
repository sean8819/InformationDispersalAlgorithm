/**
 * @file Field.cpp
 * @brief Implementation of the Field class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Field/Field.h"

#include <stdexcept>

namespace ida {

long Field::computePower(long base, int exponent) const {

    if (exponent < 0)
        throw std::invalid_argument("The exponent cannot be negative");

    long power{1};

    for (int i = 0; i < exponent; i++)
        power = this->multiply(power, base);

    return (power);
}

long Field::computeInverse(long a) const {

    if (a == 0)
        throw std::invalid_argument("Zero has no inverse!");

    //In a field with q elements a^(q-1) = 1, so the inverse of a is a^(q-2).
    return (this->computePower(a, this->getSize() - 2));
}

} //namespace ida
