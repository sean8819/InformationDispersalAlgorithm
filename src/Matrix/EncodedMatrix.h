/**
 * @file EncodedMatrix.h
 * @brief Encoded matrix produced by the IDA.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include "Matrix/Matrix.h"

/**
 * @brief Encoded matrix C = A · B, computed in the field.
 *
 * Each row is a fragment. It also stores how many padding characters were
 * added to the text, so that decoding can remove them.
 */
class EncodedMatrix : public Matrix {

private:

    int padding;            ///< Number of padding characters added to the text.

    MatrixData dataEncoded; ///< Matrix values.

    /**
     * @brief Creates the matrix with every value set to zero.
     */
    void initializeEncodedMatrix();


public:

    /**
     * @brief Creates an encoded matrix with every value set to zero.
     * @param row Number of rows, i.e. of fragments.
     * @param column Number of columns.
     */
    EncodedMatrix(int row, int column);

    /**
     * @brief Returns the matrix values.
     * @return Reference to the internal values, not a copy.
     */
    MatrixData &getDataEncoded();

    /**
     * @brief Extracts the selected fragments, i.e. rows.
     * @param labels Indices of the rows to extract.
     * @return New matrix with one row for each label, owned by the caller, or
     *         nullptr if a label is not the index of a row.
     */
    std::unique_ptr<EncodedMatrix> getFragments(const std::vector<int> &labels) const;

    /**
     * @brief Sets the number of padding characters.
     * @param padding Number of padding characters; negative values are ignored.
     */
    void setPadding(int padding);

    /**
     * @brief Returns the number of padding characters.
     * @return Number of padding characters.
     */
    int getPadding() const;

    /**
     * @brief Prints the encoded matrix to standard output.
     * @param label Header printed before the values.
     */
    void printMatrix(const std::string &label = "\nEncoded matrix:") const override;

};
