/**
 * @file IDA.cpp
 * @brief Implementation of the IDA class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Ida/IDA.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include <utility>

#include "Field/Field.h"
#include "Matrix/CodingMatrix.h"
#include "Matrix/EncodedMatrix.h"
#include "Matrix/InverseCodingMatrix.h"
#include "Matrix/TextMatrix.h"

namespace ida {

IDA::IDA(int n, int m, std::shared_ptr<const Field> field) : n(n), m(m), field(std::move(field)) {

    if (this->field == nullptr)
        throw std::invalid_argument("The field cannot be null");

    if (m < 1)
        throw std::invalid_argument("The parameter m must be at least 1");

    if (m >= n)
        throw std::invalid_argument("The parameter m must be strictly less than n");

    //Every byte value must be an element of the field, otherwise some characters would be lost.
    if (this->field->getSize() < IDA::minimumFieldSize)
        throw std::invalid_argument("The field must have at least " + std::to_string(IDA::minimumFieldSize) +
                                    " elements, one for each byte value");

    //The coding matrix uses the n distinct non-zero elements 1 ... n, so n must be less than the field size.
    if (n >= this->field->getSize())
        throw std::invalid_argument("The parameter n must be less than the number of elements of the field");
}

std::unique_ptr<EncodedMatrix> IDA::encode(const CodingMatrix &A, const TextMatrix &B) const {

    this->checkEncodingArguments(A, B);

    EncodedMatrix C{this->multiplyMatrices(A, B), B.getPadding()};

    //Move the encoded matrix into a new object owned by the caller.
    return (std::make_unique<EncodedMatrix>(std::move(C)));
}

void IDA::checkEncodingArguments(const CodingMatrix &A, const TextMatrix &B) const {

    if (A.getField() != this->field)
        throw std::invalid_argument("The coding matrix must use the same field object as the IDA object!");

    if (A.getRow() != this->n || A.getColumn() != this->m)
        throw std::invalid_argument("The coding matrix must have n rows and m columns!");

    if (A.getColumn() != B.getRow())
        throw std::invalid_argument("The number of columns of A must equal the number of rows of B!");
}

MatrixData IDA::multiplyMatrices(const Matrix &left, const Matrix &right) const {

    //The callers check the dimensions of the matrices they receive.
    assert(left.getColumn() == right.getRow());

    MatrixData product(left.getRow(), std::vector<long>(right.getColumn(), 0));

    for (int i = 0; i < left.getRow(); i++)
        for (int j = 0; j < right.getColumn(); j++)
            product[i][j] = this->addProducts(this->computeProducts(left.getValues()[i], right, j));

    return (product);
}

std::vector<FieldProduct> IDA::computeProducts(const std::vector<long> &leftRow, const Matrix &right,
                                               int column) const {

    std::vector<FieldProduct> products{};

    for (int k = 0; k < static_cast<int>(leftRow.size()); k++) {
        const long rightFactor{right.getValues()[k][column]};

        products.push_back(FieldProduct{leftRow[k], rightFactor, this->field->multiply(leftRow[k], rightFactor)});
    }

    return (products);
}

long IDA::addProducts(const std::vector<FieldProduct> &products) const {

    long sum{0};

    for (const FieldProduct &product : products)
        sum = this->field->add(sum, product.value);

    return (sum);
}

std::vector<EncodingStep> IDA::computeEncodingSteps(const CodingMatrix &A, const TextMatrix &B) const {

    this->checkEncodingArguments(A, B);

    std::vector<EncodingStep> steps{};

    for (const std::vector<long> &codingRow : A.getValues())
        for (int j = 0; j < B.getColumn(); j++) {
            std::vector<FieldProduct> products{this->computeProducts(codingRow, B, j)};
            const long result{this->addProducts(products)};

            steps.push_back(EncodingStep{std::move(products), result});
        }

    return (steps);
}

std::string IDA::decode(const InverseCodingMatrix &inverse, const EncodedMatrix &C) const {
    return (TextMatrix::readText(this->computeDecodedValues(inverse, C), C.getPadding()));
}

MatrixData IDA::computeDecodedValues(const InverseCodingMatrix &inverse, const EncodedMatrix &C) const {

    this->checkInverse(inverse);

    //The inverse rebuilds the text only from the fragments it was computed for: B = A'^-1 · C'.
    const std::unique_ptr<EncodedMatrix> fragments{C.getFragments(inverse.getLabels())};

    return (this->multiplyMatrices(inverse, *fragments));
}

void IDA::checkInverse(const InverseCodingMatrix &inverse) const {

    if (inverse.getField() != this->field)
        throw std::invalid_argument("The inverse must use the same field object as the IDA object!");

    if (inverse.getRow() != this->m || inverse.getColumn() != this->m)
        throw std::invalid_argument("The inverse must have m rows and m columns!");
}

} //namespace ida
