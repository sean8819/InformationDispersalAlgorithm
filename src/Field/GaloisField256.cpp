/**
 * @file GaloisField256.cpp
 * @brief Implementation of the GaloisField256 class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Field/GaloisField256.h"

namespace ida {

long GaloisField256::add(long a, long b) const {
    return (a ^ b);
}

long GaloisField256::subtract(long a, long b) const {
    return (a ^ b);
}

long GaloisField256::multiply(long a, long b) const {

    //A single XOR reduces an element of degree 8 only if the polynomial has degree exactly 8.
    static_assert(GaloisField256::polynomial >= GaloisField256::overflowBit &&
                      GaloisField256::polynomial < 2 * GaloisField256::overflowBit,
                  "The polynomial of GF(2^8) must have degree 8");

    long product{0};
    long shiftedA{a};
    long remainingB{b};

    //For each bit of b set to 1, add (XOR) a shifted by the position of that bit.
    while (remainingB > 0) {

        if ((remainingB & 1) == 1)
            product ^= shiftedA;

        remainingB >>= 1;
        shiftedA <<= 1;

        //If the shifted a reaches degree 8, reduce it modulo the irreducible polynomial.
        if ((shiftedA & GaloisField256::overflowBit) != 0)
            shiftedA ^= GaloisField256::polynomial;
    }

    return (product);
}

int GaloisField256::getSize() const {
    return (GaloisField256::elementCount);
}

} //namespace ida
