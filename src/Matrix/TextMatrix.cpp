/**
 * @file TextMatrix.cpp
 * @brief Implementation of the TextMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/TextMatrix.h"

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ida {

namespace {

/**
 * @brief Lays out the character codes of a text by column.
 *
 * It checks its parameters first because it runs in the member initializer
 * list of TextMatrix, before the body of the constructor.
 *
 * @param text Text to lay out.
 * @param row Number of rows.
 * @return Matrix with ceil(text length / row) columns; the cells after the
 *         end of the text hold 0, the code of the null padding character.
 * @throws std::invalid_argument If @p row is less than 1.
 */
[[nodiscard]] MatrixData layOutByColumn(const std::string &text, int row) {

    if (row < 1)
        throw std::invalid_argument("The text matrix must have at least one row");

    const int length{static_cast<int>(text.length())};

    //The number of columns is the smallest one that holds every character: ceil(length / row).
    const int column{(length + row - 1) / row};

    MatrixData values(row, std::vector<long>(column, 0));

    for (int index = 0; index < length; index++)
        values[index % row][index / row] = static_cast<unsigned char>(text[index]);

    return (values);
}

} //namespace

//The member padding is declared before paddedText, so it is already initialized when paddedText uses it.
TextMatrix::TextMatrix(int row, std::string text)
    : Matrix(layOutByColumn(text, row)), padding(this->getRow() * this->getColumn() - static_cast<int>(text.length())),
      paddedText(std::move(text) + std::string(static_cast<std::size_t>(this->padding), '\0')) {}

std::string TextMatrix::readText(const MatrixData &values, int padding) {

    std::string text{};

    if (values.empty())
        return (text);

    //Read the characters in the order they were laid out: column by column.
    for (int j = 0; j < static_cast<int>(values.front().size()); j++)
        for (const std::vector<long> &row : values)
            text += static_cast<char>(row[j]);

    return (text.substr(0, text.length() - static_cast<std::size_t>(padding)));
}

int TextMatrix::getPadding() const {
    return (this->padding);
}

const std::string &TextMatrix::getPaddedText() const {
    return (this->paddedText);
}

} //namespace ida
