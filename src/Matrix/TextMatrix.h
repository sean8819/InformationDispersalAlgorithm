/**
 * @file TextMatrix.h
 * @brief Matrix holding the text to encode.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include <string>

#include "Matrix/Matrix.h"

namespace ida {

/**
 * @brief Matrix of the character codes of the text to encode.
 *
 * Characters are laid out by column: the character at index k goes to
 * position [k % rows][k / rows]. It is the transpose of the matrix in the
 * original paper, to simplify the computations.
 *
 * @invariant The matrix has at least one row, and the padded text fills it exactly.
 */
class TextMatrix final : public Matrix {

private:

    static constexpr long largestByteValue{255}; ///< The algorithm encodes bytes: every value is between 0 and 255.

    int padding;            ///< Number of null characters appended to the text so that it fills the matrix.
    std::string paddedText; ///< Text to encode, followed by the padding characters.

    /**
     * @brief Converts a decoded value back into the character it encodes.
     * @param value Decoded value.
     * @return The character whose byte is @p value.
     * @throws std::out_of_range If @p value is not between 0 and 255.
     */
    [[nodiscard]] static char convertToCharacter(long value);

public:

    /**
     * @brief Creates the matrix from the text.
     *
     * The number of columns is computed from the text: ceil(text length / row).
     * If the text is shorter than the number of cells of the matrix, null
     * characters are appended until it fills the matrix.
     *
     * @param row Number of rows (m).
     * @param text Text to encode.
     * @throws std::invalid_argument If @p row is less than 1.
     */
    TextMatrix(int row, std::string text);

    /**
     * @brief Reads a text laid out by column, such as the values of a decoded text matrix.
     * @param values Character codes laid out by column, as in a TextMatrix.
     * @param padding Number of padding characters to remove from the end.
     * @return The text, without the padding.
     * @throws std::out_of_range If a value is not between 0 and 255: only corrupted data can produce it.
     */
    [[nodiscard]] static std::string readText(const MatrixData &values, int padding);

    /**
     * @brief Returns the number of padding characters.
     * @return Number of null characters appended to the text.
     */
    [[nodiscard]] int getPadding() const;

    /**
     * @brief Returns the text to encode, padding included.
     * @return Reference to the internal text, not a copy.
     */
    [[nodiscard]] const std::string &getPaddedText() const;
};

} //namespace ida
