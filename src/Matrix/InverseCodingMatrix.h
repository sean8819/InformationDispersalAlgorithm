/**
 * @file InverseCodingMatrix.h
 * @brief Inverse of the coding submatrix, used for decoding.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include "Matrix/CodingMatrix.h"

/**
 * @brief Inverse of the coding submatrix, computed in the field of the coding matrix.
 *
 * The m x m submatrix is made of the coding matrix rows that match the
 * selected fragments. The inverse is computed through the adjugate matrix:
 * A^-1 = det(A)^-1 · adj(A). Every operation is done in the field.
 */
class InverseCodingMatrix : public Matrix {

private:

    /**
     * @brief Builds the coding submatrix from the selected fragments.
     *
     * Use getInverse() to obtain the inverse.
     *
     * @param codingMatrix Full coding matrix (n x m).
     * @param labels Indices of the m rows (fragments) to use.
     */
    InverseCodingMatrix(const CodingMatrix &codingMatrix, const std::vector<int> &labels);

    std::shared_ptr<Field> field;           ///< Field used for the arithmetic, taken from the coding matrix.

    std::vector<int> labels;                ///< Indices of the fragments the inverse was computed for.

    MatrixData fragmentsCodingMatrix;       ///< m x m submatrix to invert.

    MatrixData inverseCodingMatrixData;     ///< Inverse values.

    /**
     * @brief Computes the determinant by Laplace expansion along the first column.
     * @param matrix Square matrix.
     * @return The determinant.
     */
    long calculateDeterminant(const MatrixData &matrix) const;

    /**
     * @brief Computes the minor of an element.
     *
     * It is the determinant of the matrix obtained by removing row
     * @p indexRow and column @p indexColumn from @p matrix.
     *
     * @param matrix Square matrix.
     * @param indexRow Index of the row to remove.
     * @param indexColumn Index of the column to remove.
     * @return The minor.
     */
    long calculateDeterminantAIJ(const MatrixData &matrix, int indexRow, int indexColumn) const;


public:

    /**
     * @brief Prints the inverse matrix to standard output.
     * @param label Header printed before the values.
     */
    void printMatrix(const std::string &label = "\nInverse matrix:") const override;

    /**
     * @brief Computes the inverse of the coding submatrix.
     * @param A Full coding matrix.
     * @param labels Indices of the m rows (fragments) chosen for reconstruction.
     * @return New inverse matrix, owned by the caller, or nullptr if @p labels
     *         does not hold m indices, a label is not the index of a row of
     *         @p A or the submatrix has a zero determinant.
     */
    static std::unique_ptr<InverseCodingMatrix> getInverse(const CodingMatrix &A, const std::vector<int> &labels);

    /**
     * @brief Returns the field used for the arithmetic.
     * @return Reference to the shared pointer to the field.
     */
    const std::shared_ptr<Field> &getField() const;

    /**
     * @brief Returns the indices of the fragments the inverse was computed for.
     * @return Reference to the internal labels, not a copy.
     */
    const std::vector<int> &getLabels() const;

    /**
     * @brief Returns the inverse values.
     * @return Reference to the internal values, not a copy.
     */
    const MatrixData &getInverseCodingMatrixData() const;

};
