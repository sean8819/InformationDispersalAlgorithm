/**
 * @file GaloisField256.cpp
 * @brief Implementation of the GaloisField256 class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Field/GaloisField256.h"

long GaloisField256::add(long a, long b) {
    return (a ^ b);
}

long GaloisField256::sub(long a, long b) {
    return (a ^ b);
}

long GaloisField256::mul(long a, long b) {

    long result = 0;

    //For each bit of b set to 1, add (XOR) a shifted by the position of that bit.
    while (b > 0) {

        if ((b & 1) == 1)
            result ^= a;

        b >>= 1;
        a <<= 1;

        //If a reaches degree 8, reduce it modulo the irreducible polynomial.
        if ((a & 0x100) != 0)
            a ^= this->polynomial;
    }

    return (result);
}

int GaloisField256::getSize() const {
    return (256);
}
