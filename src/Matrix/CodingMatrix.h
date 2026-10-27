/**
 * @file CodingMatrix.h
 * @brief Coding matrix (Vandermonde) used by the IDA.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once


#include "Matrix/Matrix.h"
#include "Field/Field.h"

/**
 * @brief n x m coding matrix.
 *
 * It is a Vandermonde matrix: row i is (1, i+1, (i+1)^2, ..., (i+1)^(m-1)),
 * with the powers computed in the field. Any m of its rows are linearly
 * independent, so any m fragments are enough to rebuild the data.
 */
class CodingMatrix : public Matrix {

private:

    std::shared_ptr<Field> field;   ///< Field used for the arithmetic, shared with the other objects that use it.

    MatrixData codingMatrix;        ///< Matrix values (n x m).

    /**
     * @brief Creates the matrix and fills it with the Vandermonde values.
     * @return Always 0.
     */
    int initializeCodingMatrix();

public:

    /**
     * @brief Creates the coding matrix.
     *
     * If @p field is null, or @p row or @p column is less than 1, prints an
     * error and terminates the program.
     *
     * @param row Number of rows, i.e. of fragments generated (n).
     * @param column Number of columns, i.e. of fragments needed for reconstruction (m).
     * @param field Field used for the arithmetic.
     */
    CodingMatrix(int row, int column, std::shared_ptr<Field> field);

    /**
     * @brief Returns the field used for the arithmetic.
     * @return Reference to the shared pointer to the field.
     */
    const std::shared_ptr<Field> &getField() const;

    /**
     * @brief Prints the coding matrix to standard output.
     * @param label Header printed before the values.
     */
    void printMatrix(const std::string &label = "\nCoding matrix:") const override;

    /**
     * @brief Returns the matrix values.
     * @return Reference to the internal values, not a copy.
     */
    const MatrixData &getCodingMatrix() const;

};
