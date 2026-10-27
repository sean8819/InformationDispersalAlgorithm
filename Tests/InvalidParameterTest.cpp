/**
 * @file InvalidParameterTest.cpp
 * @brief Checks that invalid constructor parameters are rejected with an exception and the right message.
 * @author Sean8819
 * @date 26/10/2026
 */

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

#include "Field/Field.h"
#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"
#include "Ida/IDA.h"
#include "Matrix/CodingMatrix.h"
#include "Matrix/TextMatrix.h"
#include "TestUtil.h"

namespace {

/**
 * @brief Creates an IDA object and returns the message of the exception it throws.
 * @param n Number of fragments generated.
 * @param m Number of fragments needed for reconstruction.
 * @param field Field used for the arithmetic.
 * @return The message of the std::invalid_argument thrown, or no value if the parameters were accepted.
 */
[[nodiscard]] std::optional<std::string> findIdaError(int n, int m, const std::shared_ptr<const ida::Field> &field) {

    try {
        const ida::IDA dispersal{n, m, field};
    }
    catch (const std::invalid_argument &error) {
        return (error.what());
    }

    return (std::nullopt);
}

/**
 * @brief Creates a prime field and returns the message of the exception it throws.
 * @param p Modulus of the field.
 * @return The message of the std::invalid_argument thrown, or no value if the modulus was accepted.
 */
[[nodiscard]] std::optional<std::string> findPrimeFieldError(int p) {

    try {
        const ida::PrimeField field{p};
    }
    catch (const std::invalid_argument &error) {
        return (error.what());
    }

    return (std::nullopt);
}

/**
 * @brief Creates a coding matrix and returns the message of the exception it throws.
 * @param row Number of rows.
 * @param column Number of columns.
 * @param field Field used for the arithmetic.
 * @return The message of the std::invalid_argument thrown, or no value if the parameters were accepted.
 */
[[nodiscard]] std::optional<std::string> findCodingMatrixError(int row, int column,
                                                               const std::shared_ptr<const ida::Field> &field) {

    try {
        const ida::CodingMatrix A{row, column, field};
    }
    catch (const std::invalid_argument &error) {
        return (error.what());
    }

    return (std::nullopt);
}

/**
 * @brief Creates a text matrix and returns the message of the exception it throws.
 * @param row Number of rows.
 * @param text Text to encode.
 * @return The message of the std::invalid_argument thrown, or no value if the parameters were accepted.
 */
[[nodiscard]] std::optional<std::string> findTextMatrixError(int row, const std::string &text) {

    try {
        const ida::TextMatrix B{row, text};
    }
    catch (const std::invalid_argument &error) {
        return (error.what());
    }

    return (std::nullopt);
}

/**
 * @brief Checks that the IDA constructor rejects every invalid parameter with its message.
 */
void testIdaParameters() {

    const auto field{std::make_shared<ida::GaloisField256>()};

    check(!findIdaError(4, 2, field).has_value(), "IDA accepts n = 4, m = 2 over GF(2^8)");
    check(findIdaError(4, 2, nullptr) == "The field cannot be null", "IDA rejects a null field");
    check(findIdaError(4, 0, field) == "The parameter m must be at least 1", "IDA rejects m = 0");
    check(findIdaError(4, 4, field) == "The parameter m must be strictly less than n", "IDA rejects m = n");
    check(findIdaError(256, 2, field) == "The parameter n must be less than the number of elements of the field",
          "IDA rejects n equal to the size of the field");
    check(findIdaError(4, 2, std::make_shared<ida::PrimeField>(13)) ==
              "The field must have at least 256 elements, one for each byte value",
          "IDA rejects a field with fewer than 256 elements");
}

/**
 * @brief Checks that the prime field rejects a modulus that is not prime.
 */
void testPrimeFieldParameters() {

    check(!findPrimeFieldError(257).has_value(), "GF(p) accepts p = 257");
    check(findPrimeFieldError(256) == "The modulus p must be a prime number", "GF(p) rejects p = 256");
    check(findPrimeFieldError(0) == "The modulus p must be a prime number", "GF(p) rejects p = 0");
}

/**
 * @brief Checks that the coding matrix rejects a null field and a missing dimension.
 */
void testCodingMatrixParameters() {

    const auto field{std::make_shared<ida::GaloisField256>()};

    check(!findCodingMatrixError(4, 2, field).has_value(), "the coding matrix accepts 4 x 2");
    check(findCodingMatrixError(4, 0, field) == "The coding matrix must have at least one row and one column",
          "the coding matrix rejects 0 columns");
    check(findCodingMatrixError(4, 2, nullptr) == "The field cannot be null", "the coding matrix rejects a null field");
}

/**
 * @brief Checks that the text matrix rejects 0 rows.
 */
void testTextMatrixParameters() {

    check(!findTextMatrixError(2, "hello").has_value(), "the text matrix accepts 2 rows");
    check(findTextMatrixError(0, "hello") == "The text matrix must have at least one row",
          "the text matrix rejects 0 rows");
}

} //namespace

/**
 * @brief Runs the invalid parameter tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    testIdaParameters();
    testPrimeFieldParameters();
    testCodingMatrixParameters();
    testTextMatrixParameters();

    return (report("All invalid parameter tests passed"));
}
