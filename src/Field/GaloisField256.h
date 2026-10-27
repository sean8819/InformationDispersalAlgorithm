/**
 * @file GaloisField256.h
 * @brief Galois field GF(2^8), whose elements are exactly the 256 byte values.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include "Field/Field.h"

/**
 * @brief Galois field GF(2^8).
 *
 * Each element is a byte, read as a polynomial of degree below 8 with
 * coefficients in GF(2): bit k is the coefficient of x^k. Addition and
 * subtraction are both XOR. Multiplication is polynomial multiplication
 * reduced modulo the AES polynomial x^8 + x^4 + x^3 + x + 1.
 */
class GaloisField256 : public Field {

private:

    const int polynomial = 0x11B;   ///< Irreducible polynomial x^8 + x^4 + x^3 + x + 1.

public:

    /**
     * @brief Adds two elements.
     * @param a First element.
     * @param b Second element.
     * @return The sum a XOR b.
     */
    long add(long a, long b) override;

    /**
     * @brief Subtracts two elements; in GF(2^8) this is the same as adding them.
     * @param a First element.
     * @param b Element to subtract.
     * @return The difference a XOR b.
     */
    long sub(long a, long b) override;

    /**
     * @brief Multiplies two elements with the shift-and-XOR method.
     * @param a First element.
     * @param b Second element.
     * @return The product of the two polynomials modulo #polynomial.
     */
    long mul(long a, long b) override;

    /**
     * @brief Returns the number of elements of the field.
     * @return Always 256.
     */
    int getSize() const override;

};
