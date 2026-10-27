/**
 * @file EncodedMatrix.cpp
 * @brief Implementation of the EncodedMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/EncodedMatrix.h"

#include <stdexcept>
#include <utility>

namespace ida {

EncodedMatrix::EncodedMatrix(MatrixData values, int padding) : Matrix(std::move(values)), padding(padding) {

    if (padding < 0)
        throw std::invalid_argument("The padding cannot be negative");
}

std::unique_ptr<EncodedMatrix> EncodedMatrix::getFragments(const std::vector<int> &labels) const {

    //The fragments come from the same text: decoding them must remove the same padding.
    EncodedMatrix fragments{this->selectRows(labels), this->padding};

    //Move the fragments into a new object owned by the caller.
    return (std::make_unique<EncodedMatrix>(std::move(fragments)));
}

int EncodedMatrix::getPadding() const {
    return (this->padding);
}

} //namespace ida
