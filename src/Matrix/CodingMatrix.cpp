/**
 * @file CodingMatrix.cpp
 * @brief Implementation of the CodingMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/CodingMatrix.h"

CodingMatrix::CodingMatrix(int row, int column, std::shared_ptr<Field> field) : Matrix(row, column) {

    if (field == nullptr) {
        std::cerr << "The field cannot be null" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (row < 1 || column < 1) {
        std::cerr << "The coding matrix must have at least one row and one column" << std::endl;
        exit(EXIT_FAILURE);
    }

    this->field = std::move(field);
    this->initializeCodingMatrix();
}

//Generate the matrix of linearly independent vectors. This matrix is the Vandermonde matrix.

int CodingMatrix::initializeCodingMatrix() {

    this->codingMatrix = MatrixData(this->getRow(), std::vector<long>(this->getColumn(), 0));

    for (int i = 0; i < this->getRow(); i++) {
        this->codingMatrix[i][0] = 1;

        for (int j = 1; j < this->getColumn(); j++)
            this->codingMatrix[i][j] = this->field->pow(i + 1, j);
    }

    return 0;
}

void CodingMatrix::printMatrix(const std::string &label) const {

    std::cout << label << std::endl;

    for (int i = 0; i < this->getRow(); i++) {
        for (int j = 0; j < this->getColumn(); j++)
            std::cout << this->codingMatrix[i][j] << " ";

        std::cout << std::endl;

    }
}


const MatrixData &CodingMatrix::getCodingMatrix() const {
    return (this->codingMatrix);
}

const std::shared_ptr<Field> &CodingMatrix::getField() const {
    return this->field;
}





