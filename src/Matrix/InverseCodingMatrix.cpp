/**
 * @file InverseCodingMatrix.cpp
 * @brief Implementation of the InverseCodingMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/InverseCodingMatrix.h"

#include <memory>
#include <vector>

#include "Matrix/CodingMatrix.h"

namespace ida {

InverseCodingMatrix::InverseCodingMatrix(const CodingMatrix &A, const std::vector<int> &labels)
    : Matrix(A.invertRows(labels)), field(A.getField()), labels(labels) {}

const std::shared_ptr<const Field> &InverseCodingMatrix::getField() const {
    return (this->field);
}

const std::vector<int> &InverseCodingMatrix::getLabels() const {
    return (this->labels);
}

} //namespace ida
