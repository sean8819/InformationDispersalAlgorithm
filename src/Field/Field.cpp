/**
 * @file Field.cpp
 * @brief Implementation of the Field class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Field/Field.h"

long Field::pow(long base, int exponent) {

    long result = 1;

    for (int i = 0; i < exponent; i++)
        result = this->mul(result, base);

    return (result);
}

long Field::inv(long a) {

    if (a == 0) {
        std::cerr << "Zero has no inverse!" << std::endl;
        return (-1);
    }

    //In a field with q elements a^(q-1) = 1, so the inverse of a is a^(q-2).
    return (this->pow(a, this->getSize() - 2));
}
