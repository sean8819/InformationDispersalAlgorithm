/**
 * @file VandermondeMatrix.cpp
 * @brief Implementation of the VandermondeMatrix class.
 * @author Sean8819
 * @date 29/10/2026
 */

#include "Matrix/VandermondeMatrix.h"

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Field/Field.h"

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
 * @throws std::logic_error If the column holds only zeros from the diagonal down: the rows are not independent.
 */
[[nodiscard]] int findPivotRow(const MatrixData &augmented, int column) {

    const int size{static_cast<int>(augmented.size())};

    for (int row = column; row < size; row++)
        if (augmented[row][column] != 0)
            return (row);

    //Distinct rows of a Vandermonde matrix are always independent: only a bug can get here.
    throw std::logic_error("Gauss-Jordan elimination found no pivot: the rows are not linearly independent");
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
 */
[[nodiscard]] MatrixData invert(const MatrixData &matrix, const Field &field) {

    const int size{static_cast<int>(matrix.size())};
    MatrixData augmented{appendIdentity(matrix)};

    for (int column = 0; column < size; column++)
        reduceColumn(augmented, column, field);

    return (extractRightHalf(augmented));
}

} //namespace

//The field is a const reference and the base copies it: moving it here could empty it before computeValues reads
//it, since the order in which the two arguments are evaluated is unspecified.
VandermondeMatrix::VandermondeMatrix(int row, int column, const std::shared_ptr<const Field> &field)
    : CodingMatrix(VandermondeMatrix::computeValues(row, column, field), field) {}

MatrixData VandermondeMatrix::computeValues(int row, int column, const std::shared_ptr<const Field> &field) {

    CodingMatrix::checkParameters(row, column, field);

    MatrixData values(row, std::vector<long>(column, 0));

    for (int i = 0; i < row; i++) {
        values[i][0] = 1;

        for (int j = 1; j < column; j++)
            values[i][j] = field->computePower(i + 1, j);
    }

    return (values);
}

MatrixData VandermondeMatrix::computeInverseOfRows(const std::vector<int> &labels) const {
    return (invert(this->selectRows(labels), *this->getField()));
}

} //namespace ida
