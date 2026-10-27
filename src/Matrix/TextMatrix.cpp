/**
 * @file TextMatrix.cpp
 * @brief Implementation of the TextMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/TextMatrix.h"

TextMatrix::TextMatrix(int row, std::string data) : Matrix() {

    if (row < 1) {
        std::cerr << "The text matrix must have at least one row" << std::endl;
        exit(EXIT_FAILURE);
    }

    //The number of columns is the smallest one that holds every character: ceil(length / row).
    this->setRow(row);
    this->setColumn(static_cast<int>(ceil(static_cast<double>(data.length()) / row)));

    this->data = std::move(data);
    this->initializeDataMatrix();
}

void TextMatrix::checkMessage() {

    int missing = this->getRow() * this->getColumn() - static_cast<int>(this->data.length());

    //Append null characters until the text fills the whole matrix.
    for (int i = 0; i < missing; i++) {
        this->data += '\0';
        this->padding++;
    }
}

int TextMatrix::initializeDataMatrix() {

    this->checkMessage();

    int index = 0;

    //Compute the number of rows and columns of the matrix holding the characters.

    //Create and initialize the transpose of the matrix described in the original paper, in order to
    //simplify the computations.

    const int row = this->getRow();
    const int column = this->getColumn();

    this->textMatrix = MatrixData(row, std::vector<long>(column, 0));

    //Build the fragments matrix.
    for (int j = 0; j < column; j++)
        for (int i = 0; i < row; i++)
            if (index < static_cast<int>(this->data.length()))
                this->textMatrix[i][j] = static_cast<unsigned char>(this->data.at(index++));

    return 0;
}

void TextMatrix::printCharacterCodes() const {

    std::cout << "Character codes" << std::endl;

    for (int i = 0; i < static_cast<int>(this->data.length()); i++)
       std::cout << static_cast<int>(this->data.at(i)) << " ";

    std::cout << std::endl;
}

void TextMatrix::printMatrix(const std::string &label) const {

    const int row = this->getRow();
    const int column = this->getColumn();

    std::cout << label << std::endl;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++)
            std::cout << this->textMatrix[i][j] << " ";

        std::cout << "\n";
    }
}

const MatrixData &TextMatrix::getTextMatrix() const {
    return (this->textMatrix);
}

int TextMatrix::getPadding() const {
    return this->padding;
}
