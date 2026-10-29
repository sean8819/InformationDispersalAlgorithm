/**
 * @file VandermondeMatrix.h
 * @brief Vandermonde coding matrix, inverted with Gauss-Jordan elimination.
 * @author Sean8819
 * @date 29/10/2026
 */

#pragma once

#include <memory>
#include <vector>

#include "Matrix/CodingMatrix.h"
#include "Matrix/Matrix.h"

namespace ida {

class Field;

/**
 * @brief n x m Vandermonde coding matrix: row i is (1, i+1, (i+1)^2, ..., (i+1)^(m-1)), with the powers in the field.
 *
 * The points i+1 are distinct, so any m rows are linearly independent
 * (Rabin, 1989, section 2: a_i = (1, alpha_i, ..., alpha_i^(m-1))). The
 * inverse of m rows is computed with Gauss-Jordan elimination: row
 * operations turn [A | I] into [I | A^-1], in O(m^3) operations.
 *
 * @invariant The field is not null and the matrix has at least one row and one column.
 */
class VandermondeMatrix final : public CodingMatrix {

private:

    /**
     * @brief Computes the values of the Vandermonde matrix.
     *
     * It checks its parameters first because it runs in the member
     * initializer list, before the body of the constructor.
     *
     * @param row Number of rows (n).
     * @param column Number of columns (m).
     * @param field Field used for the arithmetic.
     * @return The values of the matrix.
     * @throws std::invalid_argument If @p field is null, or @p row or @p column is less than 1.
     */
    [[nodiscard]] static MatrixData computeValues(int row, int column, const std::shared_ptr<const Field> &field);

    /**
     * @brief Inverts the square submatrix made of m distinct rows with Gauss-Jordan elimination.
     * @param labels Indices of m distinct rows of this matrix.
     * @return The values of the inverse.
     */
    [[nodiscard]] MatrixData computeInverseOfRows(const std::vector<int> &labels) const override;

public:

    /**
     * @brief Creates the coding matrix and fills it with the Vandermonde values.
     * @param row Number of rows, i.e. of fragments generated (n).
     * @param column Number of columns, i.e. of fragments needed for reconstruction (m).
     * @param field Field used for the arithmetic.
     * @throws std::invalid_argument If @p field is null, or @p row or @p column is less than 1.
     */
    VandermondeMatrix(int row, int column, const std::shared_ptr<const Field> &field);
};

} //namespace ida
