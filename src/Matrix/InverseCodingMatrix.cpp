/**
 * @file InverseCodingMatrix.cpp
 * @brief Implementation of the InverseCodingMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/InverseCodingMatrix.h"

#include <stdexcept>
#include <utility>

#include "Field/Field.h"
#include "Matrix/CodingMatrix.h"

namespace ida {

namespace {

/**
 * @brief Copies a square matrix without one of its rows and one of its columns.
 * @param matrix Square matrix.
 * @param removedRow Index of the row to leave out.
 * @param removedColumn Index of the column to leave out.
 * @return The (size - 1) x (size - 1) submatrix.
 */
[[nodiscard]] MatrixData removeRowAndColumn(const MatrixData &matrix, int removedRow, int removedColumn) {

    const int size{static_cast<int>(matrix.size())};
    MatrixData subMatrix{};

    for (int i = 0; i < size; i++) {

        if (i == removedRow)
            continue;

        std::vector<long> subRow{};

        for (int j = 0; j < size; j++)
            if (j != removedColumn)
                subRow.push_back(matrix[i][j]);

        subMatrix.push_back(subRow);
    }

    return (subMatrix);
}

/**
 * @brief Computes the determinant by Laplace expansion along the first column.
 * @param matrix Square matrix.
 * @param field Field used for the arithmetic.
 * @return The determinant.
 */
[[nodiscard]] long computeDeterminant(const MatrixData &matrix, const Field &field) {

    const int size{static_cast<int>(matrix.size())};

    //The determinant of the empty (0 x 0) matrix is 1: it is the minor of a 1 x 1 matrix.
    if (size == 0)
        return (1);

    if (size == 1)
        return (matrix[0][0]);

    if (size == 2)
        return (field.subtract(field.multiply(matrix[1][1], matrix[0][0]), field.multiply(matrix[0][1], matrix[1][0])));

    long determinant{0};

    //The terms of the expansion have sign (-1)^row: the terms of the odd rows are subtracted.
    for (int row = 0; row < size; row++) {
        const long term{field.multiply(matrix[row][0], computeDeterminant(removeRowAndColumn(matrix, row, 0), field))};

        if (row % 2 == 0)
            determinant = field.add(determinant, term);
        else
            determinant = field.subtract(determinant, term);
    }

    return (determinant);
}

/**
 * @brief Computes the adjugate matrix: the transpose of the matrix of the cofactors.
 * @param matrix Square matrix.
 * @param field Field used for the arithmetic.
 * @return The adjugate of @p matrix.
 */
[[nodiscard]] MatrixData computeAdjugate(const MatrixData &matrix, const Field &field) {

    const int size{static_cast<int>(matrix.size())};
    MatrixData adjugate(size, std::vector<long>(size, 0));

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            const long minorOfElement{computeDeterminant(removeRowAndColumn(matrix, i, j), field)};

            //The cofactor is the minor with sign (-1)^(i+j): when i + j is odd, it is subtracted from zero.
            //It goes to [j][i], because the adjugate is the transpose of the cofactor matrix.
            if ((i + j) % 2 == 0)
                adjugate[j][i] = minorOfElement;
            else
                adjugate[j][i] = field.subtract(0, minorOfElement);
        }

    return (adjugate);
}

/**
 * @brief Inverts a square matrix with the adjugate method: A^-1 = det(A)^-1 · adj(A).
 * @param matrix Square matrix.
 * @param field Field used for the arithmetic.
 * @return The inverse of @p matrix.
 * @throws std::invalid_argument If the determinant of @p matrix is zero.
 */
[[nodiscard]] MatrixData invert(const MatrixData &matrix, const Field &field) {

    const long determinant{computeDeterminant(matrix, field)};

    if (determinant == 0)
        throw std::invalid_argument("The selected matrix is not invertible!");

    const long determinantInverse{field.computeInverse(determinant)};
    MatrixData inverse{computeAdjugate(matrix, field)};

    for (std::vector<long> &row : inverse)
        for (long &value : row)
            value = field.multiply(value, determinantInverse);

    return (inverse);
}

} //namespace

InverseCodingMatrix::InverseCodingMatrix(MatrixData values, std::shared_ptr<const Field> field, std::vector<int> labels)
    : Matrix(std::move(values)), field(std::move(field)), labels(std::move(labels)) {}

std::unique_ptr<InverseCodingMatrix> InverseCodingMatrix::getInverse(const CodingMatrix &A,
                                                                     const std::vector<int> &labels) {

    if (static_cast<int>(labels.size()) != A.getColumn())
        throw std::invalid_argument("There must be exactly m labels, one for each column of the coding matrix!");

    //The rows of A chosen by the labels form the square submatrix to invert.
    const MatrixData subMatrix{A.selectRows(labels)};

    //The constructor is private: a static member function can call it.
    InverseCodingMatrix inverse{invert(subMatrix, *A.getField()), A.getField(), labels};

    //Move the inverse into a new object owned by the caller. The move constructor is public, so std::make_unique can
    //use it.
    return (std::make_unique<InverseCodingMatrix>(std::move(inverse)));
}

const std::shared_ptr<const Field> &InverseCodingMatrix::getField() const {
    return (this->field);
}

const std::vector<int> &InverseCodingMatrix::getLabels() const {
    return (this->labels);
}

} //namespace ida
