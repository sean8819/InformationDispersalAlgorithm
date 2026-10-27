/**
 * @file IntermediateResultsTest.cpp
 * @brief Checks the intermediate results that the IDA object exposes and main prints.
 * @author Sean8819
 * @date 27/10/2026
 */

#include <memory>
#include <string>
#include <vector>

#include "Field/GaloisField256.h"
#include "Ida/EncodingStep.h"
#include "Ida/FieldProduct.h"
#include "Ida/IDA.h"
#include "Matrix/CodingMatrix.h"
#include "Matrix/EncodedMatrix.h"
#include "Matrix/InverseCodingMatrix.h"
#include "Matrix/Matrix.h"
#include "Matrix/TextMatrix.h"
#include "TestUtil.h"

namespace {

/**
 * @brief Tells whether a product has the given factors and value.
 * @param product Product to check.
 * @param expected Expected factors and value.
 * @return true if every field of @p product matches @p expected.
 */
[[nodiscard]] bool isProduct(const ida::FieldProduct &product, const ida::FieldProduct &expected) {
    return (product.leftFactor == expected.leftFactor && product.rightFactor == expected.rightFactor &&
            product.value == expected.value);
}

/**
 * @brief Checks the first and the last step of the encoding of "hello!" over GF(2^8).
 */
void testEncodingSteps() {

    const auto field{std::make_shared<ida::GaloisField256>()};
    const ida::IDA dispersal{4, 2, field};

    const std::vector<ida::EncodingStep> steps{
        dispersal.computeEncodingSteps(ida::CodingMatrix{4, 2, field}, ida::TextMatrix{2, "hello!"})};

    //"hello!" in 2 rows, by column: h l o / e l !. Row 0 of A is (1, 1) and row 3 is (1, 4).
    check(steps.size() == 12, "one step for each value of the 4 x 3 encoded matrix");
    check(steps.front().products.size() == 2, "two products for each value, one for each column of A");
    check(isProduct(steps.front().products[0], {1, 104, 104}), "first product: 1 * 'h' = 104");
    check(isProduct(steps.front().products[1], {1, 101, 101}), "second product: 1 * 'e' = 101");
    check(steps.front().result == (104 ^ 101), "C[0][0] = 104 + 101 in GF(2^8)");
    check(isProduct(steps.back().products[0], {1, 111, 111}), "last step, first product: 1 * 'o' = 111");
    check(isProduct(steps.back().products[1], {4, 33, 132}), "last step, second product: 4 * '!' = 132 in GF(2^8)");
    check(steps.back().result == (111 ^ 132), "C[3][2] = 111 + 132 in GF(2^8)");
}

/**
 * @brief Checks that the result of every step is the matching value of the encoded matrix.
 */
void testEncodingStepsMatchEncodedMatrix() {

    const auto field{std::make_shared<ida::GaloisField256>()};
    const ida::CodingMatrix A{4, 2, field};
    const ida::TextMatrix B{2, "hello!"};
    const ida::IDA dispersal{4, 2, field};

    const std::vector<ida::EncodingStep> steps{dispersal.computeEncodingSteps(A, B)};
    const std::unique_ptr<ida::EncodedMatrix> C{dispersal.encode(A, B)};

    std::vector<long> encodedValues{};

    for (const std::vector<long> &row : C->getValues())
        encodedValues.insert(encodedValues.end(), row.begin(), row.end());

    std::vector<long> stepResults{};

    for (const ida::EncodingStep &step : steps)
        stepResults.push_back(step.result);

    check(stepResults == encodedValues, "the steps give the values of the encoded matrix, row by row");
}

/**
 * @brief Checks the decoded matrix and the text rebuilt from fragments 0 and 3, with and without padding.
 */
void testDecodedValues() {

    const auto field{std::make_shared<ida::GaloisField256>()};
    const ida::CodingMatrix A{4, 2, field};
    const ida::IDA dispersal{4, 2, field};
    const std::unique_ptr<ida::InverseCodingMatrix> inverse{ida::InverseCodingMatrix::getInverse(A, {0, 3})};

    const std::unique_ptr<ida::EncodedMatrix> C{dispersal.encode(A, ida::TextMatrix{2, "hello!"})};

    check(dispersal.computeDecodedValues(*inverse, *C) == ida::MatrixData{{104, 108, 111}, {101, 108, 33}},
          "the decoded matrix holds the codes of \"hello!\" by column");
    check(dispersal.decode(*inverse, *C) == "hello!", "\"hello!\" is rebuilt");

    //"hello" fills a 2 x 3 matrix with one null padding character, removed by decode().
    const std::unique_ptr<ida::EncodedMatrix> paddedC{dispersal.encode(A, ida::TextMatrix{2, "hello"})};

    check(dispersal.computeDecodedValues(*inverse, *paddedC) == ida::MatrixData{{104, 108, 111}, {101, 108, 0}},
          "the decoded matrix keeps the padding");
    check(dispersal.decode(*inverse, *paddedC) == "hello", "\"hello\" is rebuilt without the padding");
}

/**
 * @brief Checks the padding of a text matrix and the reading of a text laid out by column.
 */
void testTextMatrixPadding() {

    const ida::TextMatrix B{2, "hello"};

    check(B.getPadding() == 1, "\"hello\" in 2 rows needs one padding character");
    check(B.getPaddedText() == std::string("hello\0", 6), "the padded text ends with a null character");
    check(B.getValues() == ida::MatrixData{{104, 108, 111}, {101, 108, 0}}, "the codes are laid out by column");
    check(ida::TextMatrix::readText(B.getValues(), B.getPadding()) == "hello", "readText reverses the layout");
}

} //namespace

/**
 * @brief Runs the intermediate results tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    testEncodingSteps();
    testEncodingStepsMatchEncodedMatrix();
    testDecodedValues();
    testTextMatrixPadding();

    return (report("All intermediate results tests passed"));
}
