/**
 * @file CodingMatrix.h
 * @brief Base class of the coding matrices used by the IDA.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include <memory>
#include <vector>

#include "Matrix/Matrix.h"

namespace ida {

class Field;

/**
 * @brief n x m coding matrix: any m of its rows are linearly independent, so any m fragments rebuild the data.
 *
 * Each subclass builds the matrix in its own way and inverts the square
 * submatrix made of m of its rows with the method that suits it.
 *
 * @invariant The field is not null and the matrix has at least one row and one column.
 */
class CodingMatrix : public Matrix {

private:

    std::shared_ptr<const Field> field; ///< Field used for the arithmetic, shared with the other objects that use it.

    /**
     * @brief Computes the inverse of the square submatrix made of m distinct rows.
     * @param labels Indices of m distinct rows of this matrix.
     * @return The values of the inverse.
     */
    [[nodiscard]] virtual MatrixData computeInverseOfRows(const std::vector<int> &labels) const = 0;

protected:

    /**
     * @brief Checks the parameters of a coding matrix, before a subclass computes its values.
     * @param row Number of rows, i.e. of fragments generated (n).
     * @param column Number of columns, i.e. of fragments needed for reconstruction (m).
     * @param field Field used for the arithmetic.
     * @throws std::invalid_argument If @p field is null, or @p row or @p column is less than 1.
     */
    static void checkParameters(int row, int column, const std::shared_ptr<const Field> &field);

    /**
     * @brief Creates a coding matrix with the values computed by a subclass; only subclasses create coding matrices.
     * @param values Values of the matrix, n rows of m values.
     * @param field Field used for the arithmetic.
     */
    CodingMatrix(MatrixData values, std::shared_ptr<const Field> field);

    //Copy and move are protected: a CodingMatrix cannot be copied through a base
    //reference (slicing), but every subclass can still be copied and moved.

    /**
     * @brief Copies a coding matrix.
     * @param other Coding matrix to copy.
     */
    CodingMatrix(const CodingMatrix &other) = default;

    /**
     * @brief Moves a coding matrix.
     * @param other Coding matrix to move.
     */
    CodingMatrix(CodingMatrix &&other) = default;

    /**
     * @brief Copies a coding matrix into this one.
     * @param other Coding matrix to copy.
     * @return This coding matrix.
     */
    CodingMatrix &operator=(const CodingMatrix &other) = default;

    /**
     * @brief Moves a coding matrix into this one.
     * @param other Coding matrix to move.
     * @return This coding matrix.
     */
    CodingMatrix &operator=(CodingMatrix &&other) = default;

public:

    /**
     * @brief Returns the field used for the arithmetic.
     * @return Reference to the shared pointer to the field.
     */
    [[nodiscard]] const std::shared_ptr<const Field> &getField() const;

    /**
     * @brief Computes the inverse of the square submatrix made of the rows chosen by the labels.
     * @param labels Indices of the m rows (fragments) chosen for reconstruction.
     * @return The values of the inverse of the submatrix whose i-th row is the row labels[i].
     * @throws std::invalid_argument If @p labels does not hold m indices, or a label is repeated (the submatrix
     *         is not invertible).
     * @throws std::out_of_range If a label is not the index of a row.
     */
    [[nodiscard]] MatrixData invertRows(const std::vector<int> &labels) const;

    /**
     * @brief Virtual destructor.
     */
    ~CodingMatrix() override = default;
};

} //namespace ida
