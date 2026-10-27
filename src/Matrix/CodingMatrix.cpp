/**
 * @file CodingMatrix.cpp
 * @brief Implementation of the CodingMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/CodingMatrix.h"

#include <stdexcept>
#include <utility>
#include <vector>

#include "Field/Field.h"

namespace ida {

namespace {

/**
 * @brief Computes the Vandermonde matrix: row i is (1, i+1, (i+1)^2, ..., (i+1)^(column-1)) in the field.
 *
 * It checks its parameters first because it runs in the member initializer
 * list of CodingMatrix, before the body of the constructor.
 *
 * @param row Number of rows.
 * @param column Number of columns.
 * @param field Field used for the arithmetic.
 * @return The values of the matrix.
 * @throws std::invalid_argument If @p field is null, or @p row or @p column is less than 1.
 */
[[nodiscard]] MatrixData computeVandermondeValues(int row, int column, const std::shared_ptr<const Field> &field) {

    if (field == nullptr)
        throw std::invalid_argument("The field cannot be null");

    if (row < 1 || column < 1)
        throw std::invalid_argument("The coding matrix must have at least one row and one column");

    MatrixData values(row, std::vector<long>(column, 0));

    for (int i = 0; i < row; i++) {
        values[i][0] = 1;

        for (int j = 1; j < column; j++)
            values[i][j] = field->computePower(i + 1, j);
    }

    return (values);
}

} //namespace

CodingMatrix::CodingMatrix(int row, int column, std::shared_ptr<const Field> field)
    : Matrix(computeVandermondeValues(row, column, field)), field(std::move(field)) {}

const std::shared_ptr<const Field> &CodingMatrix::getField() const {
    return (this->field);
}

} //namespace ida
