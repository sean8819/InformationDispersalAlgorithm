/**
 * @file CodingMatrix.h
 * @brief Coding matrix (Vandermonde) used by the IDA.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include <memory>

#include "Matrix/Matrix.h"

namespace ida {

class Field;

/**
 * @brief n x m coding matrix.
 *
 * It is a Vandermonde matrix: row i is (1, i+1, (i+1)^2, ..., (i+1)^(m-1)),
 * with the powers computed in the field. Any m of its rows are linearly
 * independent, so any m fragments are enough to rebuild the data.
 *
 * @invariant The field is not null and the matrix has at least one row and one column.
 */
class CodingMatrix final : public Matrix {

private:

    std::shared_ptr<const Field> field; ///< Field used for the arithmetic, shared with the other objects that use it.

public:

    /**
     * @brief Creates the coding matrix and fills it with the Vandermonde values.
     * @param row Number of rows, i.e. of fragments generated (n).
     * @param column Number of columns, i.e. of fragments needed for reconstruction (m).
     * @param field Field used for the arithmetic.
     * @throws std::invalid_argument If @p field is null, or @p row or @p column is less than 1.
     */
    CodingMatrix(int row, int column, std::shared_ptr<const Field> field);

    /**
     * @brief Returns the field used for the arithmetic.
     * @return Reference to the shared pointer to the field.
     */
    [[nodiscard]] const std::shared_ptr<const Field> &getField() const;
};

} //namespace ida
