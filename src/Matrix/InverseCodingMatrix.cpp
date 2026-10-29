/**
 * @file InverseCodingMatrix.cpp
 * @brief Implementation of the InverseCodingMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/InverseCodingMatrix.h"

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Field/Field.h"
#include "Matrix/CodingMatrix.h"

namespace ida {

namespace {

/**
 * @brief Builds the augmented matrix [M | I]: each row of a square matrix followed by the same row of the identity.
 * @param matrix Square matrix.
 * @return The size x (2 · size) augmented matrix.
 */
[[nodiscard]] MatrixData appendIdentity(const MatrixData &matrix) {

    const int size{static_cast<int>(matrix.size())};
    const std::size_t width{2 * matrix.size()};
    MatrixData augmented{matrix};

    for (int i = 0; i < size; i++) {
        augmented[i].resize(width, 0);
        augmented[i][size + i] = 1;
    }

    return (augmented);
}

/**
 * @brief Finds the pivot of a column: the first row, from the diagonal down, with a non-zero value in that column.
 * @param augmented Augmented matrix whose columns before @p column are already reduced.
 * @param column Column to reduce.
 * @return The index of the pivot row.
 * @throws std::invalid_argument If the column holds only zeros from the diagonal down: the matrix is not invertible.
 */
[[nodiscard]] int findPivotRow(const MatrixData &augmented, int column) {

    const int size{static_cast<int>(augmented.size())};

    for (int row = column; row < size; row++)
        if (augmented[row][column] != 0)
            return (row);

    throw std::invalid_argument("The selected matrix is not invertible!");
}

/**
 * @brief Puts zero in a column on every row but the pivot row, by subtracting multiples of the pivot row.
 * @param augmented Augmented matrix whose pivot row @p column has 1 on the diagonal.
 * @param column Column to clear.
 * @param field Field used for the arithmetic.
 */
void eliminateColumn(MatrixData &augmented, int column, const Field &field) {

    const int size{static_cast<int>(augmented.size())};
    const int width{static_cast<int>(augmented[column].size())};

    for (int row = 0; row < size; row++) {
        const long factor{augmented[row][column]};

        if (row == column || factor == 0)
            continue;

        //row = row - factor · pivotRow: the value in the column becomes factor - factor · 1 = 0.
        for (int j = 0; j < width; j++)
            augmented[row][j] = field.subtract(augmented[row][j], field.multiply(factor, augmented[column][j]));
    }
}

/**
 * @brief Reduces a column: moves a pivot to the diagonal, scales the pivot row to get 1 there and clears the column.
 * @param augmented Augmented matrix whose columns before @p column are already reduced.
 * @param column Column to reduce.
 * @param field Field used for the arithmetic.
 * @throws std::invalid_argument If the matrix is not invertible.
 */
void reduceColumn(MatrixData &augmented, int column, const Field &field) {

    std::swap(augmented[column], augmented[findPivotRow(augmented, column)]);

    const long pivotInverse{field.computeInverse(augmented[column][column])};

    for (long &value : augmented[column])
        value = field.multiply(value, pivotInverse);

    eliminateColumn(augmented, column, field);
}

/**
 * @brief Copies the right half of an augmented matrix.
 * @param augmented Augmented matrix with size rows and 2 · size columns.
 * @return The size x size matrix made of the last size columns.
 */
[[nodiscard]] MatrixData extractRightHalf(const MatrixData &augmented) {

    const int size{static_cast<int>(augmented.size())};
    MatrixData rightHalf{};

    for (const std::vector<long> &row : augmented)
        rightHalf.emplace_back(row.begin() + size, row.end());

    return (rightHalf);
}

/**
 * @brief Inverts a square matrix with Gauss-Jordan elimination: row operations turn [M | I] into [I | M^-1].
 * @param matrix Square matrix.
 * @param field Field used for the arithmetic.
 * @return The inverse of @p matrix.
 * @throws std::invalid_argument If @p matrix is not invertible.
 */
[[nodiscard]] MatrixData invert(const MatrixData &matrix, const Field &field) {

    const int size{static_cast<int>(matrix.size())};
    MatrixData augmented{appendIdentity(matrix)};

    for (int column = 0; column < size; column++)
        reduceColumn(augmented, column, field);

    return (extractRightHalf(augmented));
}

/**
 * @brief Computes the values of the inverse of the coding submatrix chosen by the labels.
 *
 * It checks the labels first because it runs in the member initializer list
 * of InverseCodingMatrix, before the body of the constructor.
 *
 * @param A Full coding matrix.
 * @param labels Indices of the m rows (fragments) chosen for reconstruction.
 * @return The values of the inverse.
 * @throws std::invalid_argument If @p labels does not hold m indices, or the submatrix is not invertible.
 * @throws std::out_of_range If a label is not the index of a row of @p A.
 */
[[nodiscard]] MatrixData computeInverseValues(const CodingMatrix &A, const std::vector<int> &labels) {

    if (static_cast<int>(labels.size()) != A.getColumn())
        throw std::invalid_argument("There must be exactly m labels, one for each column of the coding matrix!");

    //The rows of A chosen by the labels form the square submatrix to invert.
    return (invert(A.selectRows(labels), *A.getField()));
}

} //namespace

InverseCodingMatrix::InverseCodingMatrix(const CodingMatrix &A, const std::vector<int> &labels)
    : Matrix(computeInverseValues(A, labels)), field(A.getField()), labels(labels) {}

const std::shared_ptr<const Field> &InverseCodingMatrix::getField() const {
    return (this->field);
}

const std::vector<int> &InverseCodingMatrix::getLabels() const {
    return (this->labels);
}

} //namespace ida
