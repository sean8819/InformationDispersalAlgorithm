/**
 * @file EncodedMatrix.cpp
 * @brief Implementation of the EncodedMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/EncodedMatrix.h"

EncodedMatrix::EncodedMatrix(int row, int column) : Matrix(row, column) {

    this->padding = 0;
    this->initializeEncodedMatrix();
}

void EncodedMatrix::initializeEncodedMatrix() {

    int row = this->getRow();
    int column = this->getColumn();

    this->dataEncoded = MatrixData(row, std::vector<long>(column, 0));

}

MatrixData &EncodedMatrix::getDataEncoded() {
    return this->dataEncoded;
}

void EncodedMatrix::printMatrix(const std::string &label) const {

    std::cout << label << std::endl;

    for (int i = 0; i < this->getRow(); i++) {
        for (int j = 0; j < this->getColumn(); j++)
            std::cout << this->dataEncoded[i][j] << " ";

        std::cout << std::endl;

    }

}

std::unique_ptr<EncodedMatrix> EncodedMatrix::getFragments(const std::vector<int> &labels) const {

    int m = static_cast<int>(labels.size());

    //Every label must be the index of a row of this matrix.
    for (int i = 0; i < m; i++)
        if (labels[i] < 0 || labels[i] >= this->getRow()) {
            std::cerr << "Fragment label out of range!" << std::endl;
            return (nullptr);
        }

    EncodedMatrix fragments(m, this->getColumn());

    for (int i = 0; i < m; i++)
        for (int j = 0; j < this->getColumn(); j++)
            fragments.dataEncoded[i][j] = this->dataEncoded[labels[i]][j];

    //Move the fragments into a new object owned by the caller.
    return (std::make_unique<EncodedMatrix>(std::move(fragments)));
}

void EncodedMatrix::setPadding(int padding) {

    if (padding >= 0)
        this->padding = padding;
}

int EncodedMatrix::getPadding() const {
    return this->padding;
}
