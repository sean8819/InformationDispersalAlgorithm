/**
 * @file CauchyMatrix.h
 * @brief Cauchy coding matrix, inverted with Rabin's formula in O(m^2) operations.
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
 * @brief n x m Cauchy coding matrix: a_ij = 1 / (x_i + y_j), with x_i = i and y_j = -(n + j) in the field.
 *
 * Rabin (1989, section 2, "The Independence Condition") asks for distinct
 * x_i, distinct y_j and x_i + y_j never zero. With these points
 * x_i + y_j = i - (n + j), which is never zero because i < n <= n + j, as
 * long as the field has at least n + m elements. The determinant of any m
 * rows is then non-zero, and their inverse has a closed form that takes
 * O(m^2) operations.
 *
 * @invariant The field is not null, the matrix has at least one row and one column, and n + m does not exceed the
 *            number of elements of the field.
 */
class CauchyMatrix final : public CodingMatrix {

private:

    /**
     * @brief Computes the values of the Cauchy matrix.
     *
     * It checks its parameters first because it runs in the member
     * initializer list, before the body of the constructor.
     *
     * @param row Number of rows (n).
     * @param column Number of columns (m).
     * @param field Field used for the arithmetic.
     * @return The values of the matrix.
     * @throws std::invalid_argument If @p field is null, @p row or @p column is less than 1, or @p row + @p column
     *         exceeds the number of elements of the field.
     */
    [[nodiscard]] static MatrixData computeValues(int row, int column, const std::shared_ptr<const Field> &field);

    /**
     * @brief Inverts the square submatrix made of m distinct rows with Rabin's formula, in O(m^2) operations.
     * @param labels Indices of m distinct rows of this matrix.
     * @return The values of the inverse.
     */
    [[nodiscard]] MatrixData computeInverseOfRows(const std::vector<int> &labels) const override;

public:

    /**
     * @brief Creates the coding matrix and fills it with the Cauchy values.
     * @param row Number of rows, i.e. of fragments generated (n).
     * @param column Number of columns, i.e. of fragments needed for reconstruction (m).
     * @param field Field used for the arithmetic.
     * @throws std::invalid_argument If @p field is null, @p row or @p column is less than 1, or @p row + @p column
     *         exceeds the number of elements of the field.
     */
    CauchyMatrix(int row, int column, const std::shared_ptr<const Field> &field);
};

} //namespace ida
