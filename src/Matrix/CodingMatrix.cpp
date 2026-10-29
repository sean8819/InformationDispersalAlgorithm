/**
 * @file CodingMatrix.cpp
 * @brief Implementation of the CodingMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/CodingMatrix.h"

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Field/Field.h"

namespace ida {

namespace {

/**
 * @brief Tells whether a label appears more than once.
 * @param labels Indices of rows.
 * @return True if two labels are equal.
 */
[[nodiscard]] bool hasRepeatedLabel(const std::vector<int> &labels) {

    std::vector<int> sortedLabels{labels};

    std::sort(sortedLabels.begin(), sortedLabels.end());

    return (std::adjacent_find(sortedLabels.begin(), sortedLabels.end()) != sortedLabels.end());
}

} //namespace

CodingMatrix::CodingMatrix(MatrixData values, std::shared_ptr<const Field> field)
    : Matrix(std::move(values)), field(std::move(field)) {}

void CodingMatrix::checkParameters(int row, int column, const std::shared_ptr<const Field> &field) {

    if (field == nullptr)
        throw std::invalid_argument("The field cannot be null");

    if (row < 1 || column < 1)
        throw std::invalid_argument("The coding matrix must have at least one row and one column");
}

const std::shared_ptr<const Field> &CodingMatrix::getField() const {
    return (this->field);
}

MatrixData CodingMatrix::invertRows(const std::vector<int> &labels) const {

    if (static_cast<int>(labels.size()) != this->getColumn())
        throw std::invalid_argument("There must be exactly m labels, one for each column of the coding matrix!");

    for (const int label : labels)
        this->checkRowLabel(label);

    //A row chosen twice gives a submatrix with two equal rows, whose determinant is zero.
    if (hasRepeatedLabel(labels))
        throw std::invalid_argument("The selected matrix is not invertible!");

    return (this->computeInverseOfRows(labels));
}

} //namespace ida
