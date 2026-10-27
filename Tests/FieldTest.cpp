/**
 * @file FieldTest.cpp
 * @brief Tests for the arithmetic of GaloisField256 and PrimeField.
 * @author Sean8819
 * @date 26/10/2026
 */

#include <string>
#include <memory>
#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"
#include "TestUtil.h"

/**
 * @brief Tests GF(2^8) against the examples of the AES standard and checks every product and inverse.
 */
void testGaloisField256() {

    GaloisField256 field;

    //Examples from section 4 of FIPS-197 (AES): {57} + {83} = {d4}, {57} · {83} = {c1}, {57} · {13} = {fe}.
    check(field.add(0x57, 0x83) == 0xD4, "GF(2^8): 0x57 + 0x83 = 0xD4");
    check(field.sub(0x57, 0x83) == 0xD4, "GF(2^8): 0x57 - 0x83 = 0xD4");
    check(field.mul(0x57, 0x83) == 0xC1, "GF(2^8): 0x57 * 0x83 = 0xC1");
    check(field.mul(0x57, 0x13) == 0xFE, "GF(2^8): 0x57 * 0x13 = 0xFE");

    //x^8 reduced modulo the AES polynomial is x^4 + x^3 + x + 1.
    check(field.pow(0x02, 8) == 0x1B, "GF(2^8): 0x02^8 = 0x1B");
    check(field.pow(0x57, 0) == 1, "GF(2^8): 0x57^0 = 1");
    check(field.getSize() == 256, "GF(2^8): 256 elements");

    //Every product must be a byte, and the product must be commutative.
    for (int a = 0; a < 256; a++)
        for (int b = 0; b < 256; b++)
            check(field.mul(a, b) >= 0 && field.mul(a, b) < 256 && field.mul(a, b) == field.mul(b, a),
                  "GF(2^8): " + std::to_string(a) + " * " + std::to_string(b) + " is a byte and commutative");

    for (int a = 1; a < 256; a++)
        check(field.mul(a, field.inv(a)) == 1, "GF(2^8): " + std::to_string(a) + " * inverse = 1");

    //Zero has no inverse: inv() prints an error and returns -1.
    check(field.inv(0) == -1, "GF(2^8): inverse of 0 = -1");
}

/**
 * @brief Tests GF(257) on known results and checks every inverse.
 */
void testPrimeField() {

    PrimeField field(257);

    check(field.add(200, 100) == 43, "GF(257): 200 + 100 = 43");
    check(field.sub(3, 5) == 255, "GF(257): 3 - 5 = 255");
    check(field.sub(0, 0) == 0, "GF(257): 0 - 0 = 0");
    check(field.mul(200, 200) == 40000 % 257, "GF(257): 200 * 200 = 40000 mod 257");
    check(field.pow(10, 3) == 1000 % 257, "GF(257): 10^3 = 1000 mod 257");

    //Fermat's little theorem: a^(p-1) = 1 for every non-zero a.
    check(field.pow(3, 256) == 1, "GF(257): 3^256 = 1");
    check(field.getSize() == 257, "GF(257): 257 elements");

    for (int a = 1; a < 257; a++)
        check(field.mul(a, field.inv(a)) == 1, "GF(257): " + std::to_string(a) + " * inverse = 1");

    //Zero has no inverse: inv() prints an error and returns -1.
    check(field.inv(0) == -1, "GF(257): inverse of 0 = -1");
}

/**
 * @brief Runs the field tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    testGaloisField256();
    testPrimeField();

    return report("All field tests passed");
}
