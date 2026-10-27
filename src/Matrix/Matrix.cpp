/**
 * @file Matrix.cpp
 * @brief Implementation of the Matrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/Matrix.h"

#include <stdexcept>
#include <utility>

namespace ida {

Matrix::Matrix(MatrixData values) : values(std::move(values)) {}

int Matrix::getRow() const {
    return (static_cast<int>(this->values.size()));
}

int Matrix::getColumn() const {

    if (this->values.empty())
        return (0);

    return (static_cast<int>(this->values.front().size()));
}

const MatrixData &Matrix::getValues() const {
    return (this->values);
}

MatrixData Matrix::selectRows(const std::vector<int> &labels) const {

    MatrixData selectedRows{};

    for (const int label : labels) {

        //Every label must be the index of a row of this matrix.
        if (label < 0 || label >= this->getRow())
            throw std::out_of_range("Fragment label out of range!");

        selectedRows.push_back(this->values[label]);
    }

    return (selectedRows);
}

} //namespace ida
