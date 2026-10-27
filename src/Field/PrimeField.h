/**
 * @file PrimeField.h
 * @brief Prime field GF(p), used by the original version of the IDA.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include "Field/Field.h"

/**
 * @brief Prime field GF(p): the integers modulo a prime p.
 *
 * With p = 257 every byte is an element, but encoded values range from 0 to
 * 256 and do not always fit in a byte.
 */
class PrimeField : public Field {

private:

    int p;  ///< Prime modulus.

public:

    /**
     * @brief Creates the field of the integers modulo @p p.
     *
     * If @p p is not a prime number, prints an error and terminates the program.
     *
     * @param p Prime modulus.
     */
    PrimeField(int p);

    /**
     * @brief Adds two elements modulo p.
     * @param a First element.
     * @param b Second element.
     * @return The sum (a + b) mod p.
     */
    long add(long a, long b) override;

    /**
     * @brief Subtracts two elements modulo p.
     * @param a First element.
     * @param b Element to subtract.
     * @return The difference (a - b) mod p, between 0 and p - 1.
     */
    long sub(long a, long b) override;

    /**
     * @brief Multiplies two elements modulo p.
     * @param a First element.
     * @param b Second element.
     * @return The product (a · b) mod p.
     */
    long mul(long a, long b) override;

    /**
     * @brief Returns the number of elements of the field.
     * @return The modulus p.
     */
    int getSize() const override;

};
