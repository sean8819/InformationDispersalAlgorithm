/**
 * @file FieldTest.cpp
 * @brief Tests for the arithmetic of GaloisField256 and PrimeField.
 * @author Sean8819
 * @date 26/10/2026
 */

#include <stdexcept>
#include <string>

#include "Field/Field.h"
#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"
#include "TestUtil.h"

namespace {

/**
 * @brief Tells whether computing the inverse of an element throws std::invalid_argument.
 * @param field Field used for the arithmetic.
 * @param a Element to invert.
 * @return true if the inverse is rejected.
 */
[[nodiscard]] bool isInverseRejected(const ida::Field &field, long a) {

    try {
        static_cast<void>(field.computeInverse(a));
    }
    catch (const std::invalid_argument &) {
        return (true);
    }

    return (false);
}

/**
 * @brief Tells whether raising 2 to a power throws std::invalid_argument.
 * @param field Field used for the arithmetic.
 * @param exponent Exponent.
 * @return true if the power is rejected.
 */
[[nodiscard]] bool isPowerRejected(const ida::Field &field, int exponent) {

    try {
        static_cast<void>(field.computePower(2, exponent));
    }
    catch (const std::invalid_argument &) {
        return (true);
    }

    return (false);
}

/**
 * @brief Tests GF(2^8) against the examples of the AES standard.
 */
void testGaloisField256Examples() {

    const ida::GaloisField256 field{};

    //Examples from section 4 of FIPS-197 (AES): {57} + {83} = {d4}, {57} · {83} = {c1}, {57} · {13} = {fe}.
    check(field.add(0x57, 0x83) == 0xD4, "GF(2^8): 0x57 + 0x83 = 0xD4");
    check(field.subtract(0x57, 0x83) == 0xD4, "GF(2^8): 0x57 - 0x83 = 0xD4");
    check(field.multiply(0x57, 0x83) == 0xC1, "GF(2^8): 0x57 * 0x83 = 0xC1");
    check(field.multiply(0x57, 0x13) == 0xFE, "GF(2^8): 0x57 * 0x13 = 0xFE");

    //Reduced modulo the AES polynomial, x^8 is x^4 + x^3 + x + 1.
    check(field.computePower(0x02, 8) == 0x1B, "GF(2^8): 0x02^8 = 0x1B");
    check(field.computePower(0x57, 0) == 1, "GF(2^8): 0x57^0 = 1");
    check(field.getSize() == 256, "GF(2^8): 256 elements");
}

/**
 * @brief Checks every product and every inverse of GF(2^8).
 */
void testGaloisField256AllElements() {

    const ida::GaloisField256 field{};

    //Every product must be a byte, and the product must be commutative.
    for (int a = 0; a < 256; a++)
        for (int b = 0; b < 256; b++)
            check(field.multiply(a, b) >= 0 && field.multiply(a, b) < 256 &&
                      field.multiply(a, b) == field.multiply(b, a),
                  "GF(2^8): " + std::to_string(a) + " * " + std::to_string(b) + " is a byte and commutative");

    for (int a = 1; a < 256; a++)
        check(field.multiply(a, field.computeInverse(a)) == 1, "GF(2^8): " + std::to_string(a) + " * inverse = 1");

    //Zero has no inverse, and a negative exponent would need one.
    check(isInverseRejected(field, 0), "GF(2^8): the inverse of 0 is rejected");
    check(isPowerRejected(field, -1), "GF(2^8): a negative exponent is rejected");
}

/**
 * @brief Tests GF(257) on known results and checks every inverse.
 */
void testPrimeField() {

    const ida::PrimeField field{257};

    check(field.add(200, 100) == 43, "GF(257): 200 + 100 = 43");
    check(field.subtract(3, 5) == 255, "GF(257): 3 - 5 = 255");
    check(field.subtract(0, 0) == 0, "GF(257): 0 - 0 = 0");
    check(field.multiply(200, 200) == 40000 % 257, "GF(257): 200 * 200 = 40000 mod 257");
    check(field.computePower(10, 3) == 1000 % 257, "GF(257): 10^3 = 1000 mod 257");

    //Fermat's little theorem: a^(p-1) = 1 for every non-zero a.
    check(field.computePower(3, 256) == 1, "GF(257): 3^256 = 1");
    check(field.getSize() == 257, "GF(257): 257 elements");

    for (int a = 1; a < 257; a++)
        check(field.multiply(a, field.computeInverse(a)) == 1, "GF(257): " + std::to_string(a) + " * inverse = 1");

    //Zero has no inverse, and a negative exponent would need one.
    check(isInverseRejected(field, 0), "GF(257): the inverse of 0 is rejected");
    check(isPowerRejected(field, -1), "GF(257): a negative exponent is rejected");
}

} //namespace

/**
 * @brief Runs the field tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    testGaloisField256Examples();
    testGaloisField256AllElements();
    testPrimeField();

    return (report("All field tests passed"));
}
