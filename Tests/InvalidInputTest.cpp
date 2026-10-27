/**
 * @file InvalidInputTest.cpp
 * @brief Checks that invalid inputs are rejected with an exception instead of causing memory errors or wrong results.
 * @author Sean8819
 * @date 26/10/2026
 */

#include <memory>
#include <stdexcept>
#include <vector>

#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"
#include "Ida/IDA.h"
#include "Matrix/CodingMatrix.h"
#include "Matrix/EncodedMatrix.h"
#include "Matrix/InverseCodingMatrix.h"
#include "Matrix/Matrix.h"
#include "Matrix/TextMatrix.h"
#include "TestUtil.h"

namespace {

/**
 * @brief How an operation ended.
 */
enum class Outcome {
    Accepted,        ///< No exception.
    InvalidArgument, ///< std::invalid_argument thrown.
    OutOfRange       ///< std::out_of_range thrown.
};

/**
 * @brief Computes the inverse of a coding submatrix and tells how it ended.
 * @param A Coding matrix.
 * @param labels Indices of the fragments.
 * @return The exception thrown, or Outcome::Accepted.
 */
[[nodiscard]] Outcome tryGetInverse(const ida::CodingMatrix &A, const std::vector<int> &labels) {

    try {
        static_cast<void>(ida::InverseCodingMatrix::getInverse(A, labels));
    }
    catch (const std::invalid_argument &) {
        return (Outcome::InvalidArgument);
    }
    catch (const std::out_of_range &) {
        return (Outcome::OutOfRange);
    }

    return (Outcome::Accepted);
}

/**
 * @brief Extracts fragments from an encoded matrix and tells how it ended.
 * @param C Encoded matrix.
 * @param labels Indices of the fragments.
 * @return The exception thrown, or Outcome::Accepted.
 */
[[nodiscard]] Outcome tryGetFragments(const ida::EncodedMatrix &C, const std::vector<int> &labels) {

    try {
        static_cast<void>(C.getFragments(labels));
    }
    catch (const std::out_of_range &) {
        return (Outcome::OutOfRange);
    }

    return (Outcome::Accepted);
}

/**
 * @brief Creates an encoded matrix and tells how it ended.
 * @param padding Number of padding characters.
 * @return The exception thrown, or Outcome::Accepted.
 */
[[nodiscard]] Outcome tryCreateEncodedMatrix(int padding) {

    try {
        const ida::EncodedMatrix C{ida::MatrixData{{1, 2}, {3, 4}}, padding};
    }
    catch (const std::invalid_argument &) {
        return (Outcome::InvalidArgument);
    }

    return (Outcome::Accepted);
}

/**
 * @brief Encodes a text matrix and tells how it ended.
 * @param dispersal Object that performs the encoding.
 * @param A Coding matrix.
 * @param B Text matrix.
 * @return The exception thrown, or Outcome::Accepted.
 */
[[nodiscard]] Outcome tryEncode(const ida::IDA &dispersal, const ida::CodingMatrix &A, const ida::TextMatrix &B) {

    try {
        static_cast<void>(dispersal.encode(A, B));
    }
    catch (const std::invalid_argument &) {
        return (Outcome::InvalidArgument);
    }

    return (Outcome::Accepted);
}

/**
 * @brief Computes the steps of an encoding and tells how it ended.
 * @param dispersal Object that performs the encoding.
 * @param A Coding matrix.
 * @param B Text matrix.
 * @return The exception thrown, or Outcome::Accepted.
 */
[[nodiscard]] Outcome tryComputeEncodingSteps(const ida::IDA &dispersal, const ida::CodingMatrix &A,
                                              const ida::TextMatrix &B) {

    try {
        static_cast<void>(dispersal.computeEncodingSteps(A, B));
    }
    catch (const std::invalid_argument &) {
        return (Outcome::InvalidArgument);
    }

    return (Outcome::Accepted);
}

/**
 * @brief Decodes an encoded matrix and tells how it ended.
 * @param dispersal Object that performs the decoding.
 * @param inverse Inverse of the coding submatrix.
 * @param C Encoded matrix.
 * @return The exception thrown, or Outcome::Accepted.
 */
[[nodiscard]] Outcome tryDecode(const ida::IDA &dispersal, const ida::InverseCodingMatrix &inverse,
                                const ida::EncodedMatrix &C) {

    try {
        static_cast<void>(dispersal.decode(inverse, C));
    }
    catch (const std::invalid_argument &) {
        return (Outcome::InvalidArgument);
    }
    catch (const std::out_of_range &) {
        return (Outcome::OutOfRange);
    }

    return (Outcome::Accepted);
}

/**
 * @brief Checks that the inverse rejects labels outside 0 ... n-1, a number of labels other than m and repeated labels.
 */
void testInverseRejectsInvalidLabels() {

    const ida::CodingMatrix A{4, 2, std::make_shared<ida::GaloisField256>()};

    check(tryGetInverse(A, {0, 3}) == Outcome::Accepted, "getInverse accepts labels 0 and 3");
    check(tryGetInverse(A, {0, 4}) == Outcome::OutOfRange, "getInverse rejects a label equal to n");
    check(tryGetInverse(A, {-1, 0}) == Outcome::OutOfRange, "getInverse rejects a negative label");
    check(tryGetInverse(A, {0}) == Outcome::InvalidArgument, "getInverse rejects fewer than m labels");
    check(tryGetInverse(A, {0, 1, 3}) == Outcome::InvalidArgument, "getInverse rejects more than m labels");
    check(tryGetInverse(A, {1, 1}) == Outcome::InvalidArgument, "getInverse rejects a repeated label");
}

/**
 * @brief Checks that the fragments reject labels outside 0 ... n-1.
 */
void testFragmentsRejectInvalidLabels() {

    const auto field{std::make_shared<ida::GaloisField256>()};
    const ida::IDA dispersal{4, 2, field};
    const std::unique_ptr<ida::EncodedMatrix> C{
        dispersal.encode(ida::CodingMatrix{4, 2, field}, ida::TextMatrix{2, "abcdef"})};

    check(tryGetFragments(*C, {0, 3}) == Outcome::Accepted, "getFragments accepts labels 0 and 3");
    check(tryGetFragments(*C, {0, 4}) == Outcome::OutOfRange, "getFragments rejects a label equal to n");
    check(tryGetFragments(*C, {-1, 0}) == Outcome::OutOfRange, "getFragments rejects a negative label");
}

/**
 * @brief Checks that an encoded matrix rejects a negative padding.
 */
void testEncodedMatrixRejectsNegativePadding() {

    check(tryCreateEncodedMatrix(0) == Outcome::Accepted, "an encoded matrix accepts a padding of 0");
    check(tryCreateEncodedMatrix(-1) == Outcome::InvalidArgument, "an encoded matrix rejects a negative padding");
}

/**
 * @brief Checks that encoding rejects a coding matrix with another field or the wrong size, or a wrong text matrix.
 */
void testEncodeRejectsInvalidMatrices() {

    const auto field{std::make_shared<ida::GaloisField256>()};
    const ida::TextMatrix B{2, "abcdef"};
    const ida::IDA dispersal{4, 2, field};

    check(tryEncode(dispersal, ida::CodingMatrix{4, 2, field}, B) == Outcome::Accepted,
          "encode accepts a 4 x 2 coding matrix");
    check(tryEncode(dispersal, ida::CodingMatrix{4, 2, std::make_shared<ida::PrimeField>(257)}, B) ==
              Outcome::InvalidArgument,
          "encode rejects a coding matrix with a different field");
    check(tryEncode(dispersal, ida::CodingMatrix{5, 2, field}, B) == Outcome::InvalidArgument,
          "encode rejects a coding matrix with more than n rows");
    check(tryEncode(dispersal, ida::CodingMatrix{4, 2, field}, ida::TextMatrix{3, "abcdef"}) ==
              Outcome::InvalidArgument,
          "encode rejects a text matrix with a number of rows other than m");
    check(tryComputeEncodingSteps(dispersal, ida::CodingMatrix{5, 2, field}, B) == Outcome::InvalidArgument,
          "computeEncodingSteps rejects a coding matrix with more than n rows");
}

/**
 * @brief Checks that decoding rejects an inverse with another field, the wrong size or labels outside the fragments.
 */
void testDecodeRejectsInvalidInverse() {

    const auto field{std::make_shared<ida::GaloisField256>()};
    const ida::CodingMatrix A{4, 2, field};
    const ida::IDA dispersal{4, 2, field};
    const std::unique_ptr<ida::EncodedMatrix> C{dispersal.encode(A, ida::TextMatrix{2, "abcdef"})};
    const std::vector<int> labels{0, 3};

    check(tryDecode(dispersal, *ida::InverseCodingMatrix::getInverse(A, labels), *C) == Outcome::Accepted,
          "decode accepts the inverse for labels 0 and 3");

    const ida::CodingMatrix otherA{4, 2, std::make_shared<ida::PrimeField>(257)};

    check(tryDecode(dispersal, *ida::InverseCodingMatrix::getInverse(otherA, labels), *C) == Outcome::InvalidArgument,
          "decode rejects an inverse with a different field");

    //The inverse must be m x m: a larger one would read outside the fragments matrix.
    const ida::CodingMatrix wideA{4, 3, field};

    check(tryDecode(dispersal, *ida::InverseCodingMatrix::getInverse(wideA, {0, 1, 3}), *C) == Outcome::InvalidArgument,
          "decode rejects an inverse larger than m x m");

    //An inverse computed for a fragment that the encoded matrix does not have.
    const ida::CodingMatrix tallA{5, 2, field};

    check(tryDecode(dispersal, *ida::InverseCodingMatrix::getInverse(tallA, {0, 4}), *C) == Outcome::OutOfRange,
          "decode rejects an inverse for a fragment that does not exist");
}

} //namespace

/**
 * @brief Runs the invalid input tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    testInverseRejectsInvalidLabels();
    testFragmentsRejectInvalidLabels();
    testEncodedMatrixRejectsNegativePadding();
    testEncodeRejectsInvalidMatrices();
    testDecodeRejectsInvalidInverse();

    return (report("All invalid input tests passed"));
}
