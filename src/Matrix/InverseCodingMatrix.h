/**
 * @file InverseCodingMatrix.h
 * @brief Inverse of the coding submatrix, used for decoding.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include <memory>
#include <vector>

#include "Matrix/Matrix.h"

namespace ida {

class CodingMatrix;
class Field;

/**
 * @brief Inverse of the coding submatrix, computed in the field of the coding matrix.
 *
 * The m x m submatrix is made of the coding matrix rows that match the
 * selected fragments. The inverse is computed through the adjugate matrix:
 * A^-1 = det(A)^-1 · adj(A). Every operation is done in the field.
 *
 * @invariant The matrix is square, with one row for each label, and the field is not null.
 */
class InverseCodingMatrix final : public Matrix {

private:

    std::shared_ptr<const Field> field; ///< Field used for the arithmetic, taken from the coding matrix.
    std::vector<int> labels;            ///< Indices of the fragments the inverse was computed for.

public:

    /**
     * @brief Computes the inverse of the coding submatrix.
     * @param A Full coding matrix.
     * @param labels Indices of the m rows (fragments) chosen for reconstruction.
     * @throws std::invalid_argument If @p labels does not hold m indices, or the submatrix has a zero
     *         determinant (a label is repeated).
     * @throws std::out_of_range If a label is not the index of a row of @p A.
     */
    InverseCodingMatrix(const CodingMatrix &A, const std::vector<int> &labels);

    /**
     * @brief Returns the field used for the arithmetic.
     * @return Reference to the shared pointer to the field.
     */
    [[nodiscard]] const std::shared_ptr<const Field> &getField() const;

    /**
     * @brief Returns the indices of the fragments the inverse was computed for.
     * @return Reference to the internal labels, not a copy.
     */
    [[nodiscard]] const std::vector<int> &getLabels() const;
};

} //namespace ida
