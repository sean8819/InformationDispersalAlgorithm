/**
 * @file TextMatrix.h
 * @brief Matrix holding the text to encode.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include "Matrix/Matrix.h"

/**
 * @brief Matrix of the character codes of the text to encode.
 *
 * Characters are laid out by column: the character at index k goes to
 * position [k % rows][k / rows]. It is the transpose of the matrix in the
 * original paper, to simplify the computations.
 */
class TextMatrix : public Matrix {

private:

    int padding{};          ///< Number of null characters appended to the text.

    MatrixData textMatrix;  ///< Matrix values.

    std::string data;       ///< Text to encode, padding included.

    /**
     * @brief Adds padding to the text.
     *
     * If the text is shorter than the number of cells of the matrix, appends
     * null characters until it fills the matrix and stores their count in #padding.
     */
    void checkMessage();

    /**
     * @brief Applies the padding, creates the matrix and fills it with the character codes.
     * @return Always 0.
     */
    int initializeDataMatrix();

public:

    /**
     * @brief Creates the matrix from the text.
     *
     * The number of columns is computed from the text: ceil(text length / m).
     * If @p row is less than 1, prints an error and terminates the program.
     *
     * @param row Number of rows (m).
     * @param data Text to encode.
     */
    TextMatrix(int row, std::string data);

    /**
     * @brief Returns the number of padding characters.
     * @return Number of null characters appended to the text.
     */
    int getPadding() const;

    /**
     * @brief Returns the matrix values.
     * @return Reference to the internal values, not a copy.
     */
    const MatrixData &getTextMatrix() const;

    /**
     * @brief Prints the codes of the characters of the text, padding included, to standard output.
     */
    void printCharacterCodes() const;

    /**
     * @brief Prints the text matrix to standard output.
     * @param label Header printed before the values.
     */
    void printMatrix(const std::string &label = "\nText matrix:") const override;

};
