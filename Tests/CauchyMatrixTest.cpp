/**
 * @file CauchyMatrixTest.cpp
 * @brief Checks the Cauchy coding matrix and the inverse given by Rabin's formula.
 * @author Sean8819
 * @date 29/10/2026
 */

#include <exception>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

#include "Field/Field.h"
#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"
#include "Matrix/CauchyMatrix.h"
#include "Matrix/Matrix.h"
#include "TestUtil.h"

namespace {

/**
 * @brief A field to test, with the name printed on failure.
 */
struct FieldUnderTest {
    std::shared_ptr<const ida::Field> field; ///< Field used for the arithmetic.
    std::string name;                        ///< Name of the field, printed on failure.
};

/**
 * @brief Multiplies two square matrices in a field.
 * @param left Left matrix.
 * @param right Right matrix, of the same size.
 * @param field Field used for the arithmetic.
 * @return The product left · right.
 */
[[nodiscard]] ida::MatrixData multiply(const ida::MatrixData &left, const ida::MatrixData &right,
                                       const ida::Field &field) {

    const int size{static_cast<int>(left.size())};
    ida::MatrixData product(size, std::vector<long>(size, 0));

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            for (int k = 0; k < size; k++)
                product[i][j] = field.add(product[i][j], field.multiply(left[i][k], right[k][j]));

    return (product);
}

/**
 * @brief Tells whether a matrix is the identity.
 * @param matrix Square matrix.
 * @return True if @p matrix has 1 on the diagonal and 0 everywhere else.
 */
[[nodiscard]] bool isIdentity(const ida::MatrixData &matrix) {

    const int size{static_cast<int>(matrix.size())};

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (matrix[i][j] != (i == j ? 1 : 0))
                return (false);

    return (true);
}

/**
 * @brief Tells whether the inverse of some rows, multiplied by those rows, gives the identity.
 * @param A Cauchy matrix.
 * @param labels Indices of m distinct rows.
 * @return True if the rows chosen by @p labels times their inverse is the identity.
 */
[[nodiscard]] bool isInverseOfRows(const ida::CauchyMatrix &A, const std::vector<int> &labels) {
    return (isIdentity(multiply(A.selectRows(labels), A.invertRows(labels), *A.getField())));
}

/**
 * @brief Creates a Cauchy matrix and returns the message of the exception it throws.
 * @param row Number of rows (n).
 * @param column Number of columns (m).
 * @param field Field used for the arithmetic.
 * @return The message of the std::invalid_argument thrown, or no value if the parameters were accepted.
 */
[[nodiscard]] std::optional<std::string> findCauchyMatrixError(int row, int column,
                                                               const std::shared_ptr<const ida::Field> &field) {

    try {
        const ida::CauchyMatrix A{row, column, field};
    }
    catch (const std::invalid_argument &error) {
        return (error.what());
    }

    return (std::nullopt);
}

/**
 * @brief Inverts some rows of a Cauchy matrix and returns the message of the exception it throws.
 * @param A Cauchy matrix.
 * @param labels Indices of the rows.
 * @return The message of the exception thrown, or no value if the labels were accepted.
 */
[[nodiscard]] std::optional<std::string> findInverseError(const ida::CauchyMatrix &A, const std::vector<int> &labels) {

    try {
        static_cast<void>(A.invertRows(labels));
    }
    catch (const std::exception &error) {
        return (error.what());
    }

    return (std::nullopt);
}

/**
 * @brief Checks that every value is 1 / (x_i + y_j), with x_i = i and y_j = -(n + j).
 * @param fieldUnderTest Field used for the arithmetic, with its name.
 */
void testValuesAreOneOverSums(const FieldUnderTest &fieldUnderTest) {

    const ida::Field &field{*fieldUnderTest.field};
    const int n{6};
    const ida::CauchyMatrix A{n, 3, fieldUnderTest.field};
    bool everyValueFits{true};

    for (int i = 0; i < A.getRow(); i++)
        for (int j = 0; j < A.getColumn(); j++)
            if (field.multiply(A.getValues()[i][j], field.subtract(i, n + j)) != 1)
                everyValueFits = false;

    check(everyValueFits, fieldUnderTest.name + ": every value times (x_i + y_j) is 1");
}

/**
 * @brief Checks Rabin's formula: the rows chosen, in any order, times their inverse give the identity.
 * @param fieldUnderTest Field used for the arithmetic, with its name.
 */
void testInverseOfRows(const FieldUnderTest &fieldUnderTest) {

    const ida::CauchyMatrix small{4, 2, fieldUnderTest.field};

    check(isInverseOfRows(small, {0, 3}), fieldUnderTest.name + ": 4 x 2, rows 0 and 3");
    check(isInverseOfRows(small, {3, 0}), fieldUnderTest.name + ": 4 x 2, rows 3 and 0");

    const ida::CauchyMatrix single{3, 1, fieldUnderTest.field};

    check(isInverseOfRows(single, {2}), fieldUnderTest.name + ": 3 x 1, row 2");

    const ida::CauchyMatrix medium{7, 4, fieldUnderTest.field};

    check(isInverseOfRows(medium, {6, 0, 3, 5}), fieldUnderTest.name + ": 7 x 4, rows 6, 0, 3, 5");
    check(isInverseOfRows(medium, {1, 2, 4, 6}), fieldUnderTest.name + ": 7 x 4, rows 1, 2, 4, 6");

    //n + m = 256 is the largest Cauchy matrix GF(2^8) allows.
    const ida::CauchyMatrix large{129, 127, fieldUnderTest.field};
    std::vector<int> lastRows{};

    for (int label = 128; label >= 2; label--)
        lastRows.push_back(label);

    check(isInverseOfRows(large, lastRows), fieldUnderTest.name + ": 129 x 127, the last 127 rows in reverse order");
}

/**
 * @brief Checks that the Cauchy matrix needs n + m distinct elements of the field.
 */
void testParameters() {

    const auto galois{std::make_shared<ida::GaloisField256>()};
    const auto prime{std::make_shared<ida::PrimeField>(257)};

    check(!findCauchyMatrixError(200, 56, galois).has_value(), "GF(2^8) accepts n + m = 256");
    check(findCauchyMatrixError(200, 57, galois) ==
              "The Cauchy matrix needs n + m distinct elements of the field: n + m cannot exceed 256",
          "GF(2^8) rejects n + m = 257");
    check(!findCauchyMatrixError(200, 57, prime).has_value(), "GF(257) accepts n + m = 257");
    check(findCauchyMatrixError(200, 58, prime) ==
              "The Cauchy matrix needs n + m distinct elements of the field: n + m cannot exceed 257",
          "GF(257) rejects n + m = 258");
    check(findCauchyMatrixError(4, 0, galois) == "The coding matrix must have at least one row and one column",
          "the Cauchy matrix rejects 0 columns");
    check(findCauchyMatrixError(4, 2, nullptr) == "The field cannot be null", "the Cauchy matrix rejects a null field");
}

/**
 * @brief Checks that the inverse rejects a number of labels other than m, labels outside 0 ... n-1 and repeated labels.
 */
void testInverseRejectsInvalidLabels() {

    const ida::CauchyMatrix A{4, 2, std::make_shared<ida::GaloisField256>()};

    check(findInverseError(A, {0}) == "There must be exactly m labels, one for each column of the coding matrix!",
          "the inverse rejects fewer than m labels");
    check(findInverseError(A, {0, 4}) == "Fragment label out of range!", "the inverse rejects a label equal to n");
    check(findInverseError(A, {-1, 0}) == "Fragment label out of range!", "the inverse rejects a negative label");
    check(findInverseError(A, {1, 1}) == "The selected matrix is not invertible!",
          "the inverse rejects a repeated label");
}

} //namespace

/**
 * @brief Runs the CauchyMatrix tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    const std::vector<FieldUnderTest> fields{{std::make_shared<ida::GaloisField256>(), "GF(2^8)"},
                                             {std::make_shared<ida::PrimeField>(257), "GF(257)"}};

    for (const FieldUnderTest &fieldUnderTest : fields) {
        testValuesAreOneOverSums(fieldUnderTest);
        testInverseOfRows(fieldUnderTest);
    }

    testParameters();
    testInverseRejectsInvalidLabels();

    return (report("All CauchyMatrix tests passed"));
}
