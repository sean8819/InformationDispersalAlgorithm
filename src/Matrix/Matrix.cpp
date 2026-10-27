/**
 * @file Matrix.cpp
 * @brief Implementation of the Matrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/Matrix.h"

Matrix::Matrix(int row, int column) {
    this->row = row;
    this->column = column;
}

Matrix::Matrix() {
    this->row = 0;
    this->column = 0;
}

int Matrix::getRow() const {
    return (this->row);
}

int Matrix::getColumn() const {
    return (this->column);
}

void Matrix::setRow(int row) {
    this->row = row;
}

void Matrix::setColumn(int column) {
    this->column = column;
}





