/**
 * @file InverseCodingMatrix.cpp
 * @brief Implementation of the InverseCodingMatrix class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Matrix/InverseCodingMatrix.h"

InverseCodingMatrix::InverseCodingMatrix(const CodingMatrix &codingMatrix, const std::vector<int> &labels) : Matrix() {

    this->field = codingMatrix.getField();

    //Remember the labels, so that decoding can check it uses the same fragments.
    this->labels = labels;

    //Initialize rows and columns. The matrix is square, with size equal to the length of a coding matrix row.
    this->setRow(codingMatrix.getColumn());
    this->setColumn(codingMatrix.getColumn());

    int n = this->getRow();

    //Initialize the decoding data matrix.

    this->inverseCodingMatrixData = MatrixData(n, std::vector<long>(n, 0));

    //Initialize the array with the coding matrix rows that will be used to build the decoding matrix.

    this->fragmentsCodingMatrix = MatrixData(n, std::vector<long>(n, 0));

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            this->fragmentsCodingMatrix[i][j] = codingMatrix.getCodingMatrix()[labels[i]][j];

}

void InverseCodingMatrix::printMatrix(const std::string &label) const {

    std::cout << label << std::endl;

    for (int i = 0; i < this->getRow(); i++) {
        for (int j = 0; j < this->getColumn(); j++)
            std::cout << this->inverseCodingMatrixData[i][j] << " ";

        std::cout << std::endl;

    }
}

long InverseCodingMatrix::calculateDeterminant(const MatrixData &matrix) const {

    int n = static_cast<int>(matrix.size());

    long determinant = 0;
    //The determinant of the empty (0 x 0) matrix is 1: it is the minor of a 1 x 1 matrix.
    if (n == 0)
        return (1);
    else if (n == 1)
        return (matrix[0][0]);
    else if (n == 2)
        return (this->field->sub(this->field->mul(matrix[1][1], matrix[0][0]),
                                 this->field->mul(matrix[0][1], matrix[1][0])));
    else {
        for (int row = 0; row < n; row++) {
            MatrixData subMatrix(n - 1, std::vector<long>(n - 1, 0));

            for (int i = 0; i < n - 1; i++)
                for (int j = 0; j < n - 1; j++) {
                    int subRow = (i < row ? i : i + 1);
                    int subColumn = j + 1;

                    subMatrix[i][j] = matrix[subRow][subColumn];
                }

            if (row % 2 == 0)
                determinant = this->field->add(determinant,
                                               this->field->mul(matrix[row][0], this->calculateDeterminant(subMatrix)));
            else
                determinant = this->field->sub(determinant,
                                               this->field->mul(matrix[row][0], this->calculateDeterminant(subMatrix)));
        }

        return (determinant);
    }
}

long InverseCodingMatrix::calculateDeterminantAIJ(const MatrixData &matrix, int indexRow, int indexColumn) const {

    int m = static_cast<int>(matrix.size());

    long determinantAij = 0;
    int subMatrixRow = 0;
    int subMatrixColumn = 0;

    //Build the matrix leaving out row indexRow and column indexColumn.
    MatrixData matrixAij(m - 1, std::vector<long>(m - 1, 0));

    //Fill the matrix with the values outside row indexRow and column indexColumn.
    for (int i = 0; i < m; i++) {

        if (i != indexRow) {
            for (int j = 0; j < m; j++) {
                if (j != indexColumn)
                    matrixAij[subMatrixRow][subMatrixColumn++] = matrix[i][j];
            }

            subMatrixRow++;
            subMatrixColumn = 0;

        }
    }

    determinantAij = this->calculateDeterminant(matrixAij);

    return (determinantAij);

}

std::unique_ptr<InverseCodingMatrix> InverseCodingMatrix::getInverse(const CodingMatrix &A, const std::vector<int> &labels) {

    if (static_cast<int>(labels.size()) != A.getColumn()) {
        std::cerr << "There must be exactly m labels, one for each column of the coding matrix!" << std::endl;
        return (nullptr);
    }

    //Every label must be the index of a row of the coding matrix.
    for (int i = 0; i < A.getColumn(); i++)
        if (labels[i] < 0 || labels[i] >= A.getRow()) {
            std::cerr << "Fragment label out of range!" << std::endl;
            return (nullptr);
        }

    //The constructor is private: a static member function can call it.
    InverseCodingMatrix inverse(A, labels);

    long determinant = inverse.calculateDeterminant(inverse.fragmentsCodingMatrix);

    if (determinant == 0) {
        std::cerr << "The selected matrix is not invertible!" << std::endl;
        return (nullptr);
    }

    long determinantInverse = inverse.field->inv(determinant);

    //Create and initialize the cofactor matrix.

    MatrixData cofactorMatrix(inverse.getColumn(), std::vector<long>(inverse.getColumn(), 0));

    //The cofactor is the minor with sign (-1)^(i+j): when i + j is odd, it is subtracted from zero.
    for (int i = 0; i < inverse.getColumn(); i++)
        for (int j = 0; j < inverse.getColumn(); j++)
            if ((i + j) % 2 == 0)
                cofactorMatrix[i][j] = inverse.calculateDeterminantAIJ(inverse.fragmentsCodingMatrix, i, j);
            else
                cofactorMatrix[i][j] = inverse.field->sub(0, inverse.calculateDeterminantAIJ(inverse.fragmentsCodingMatrix, i, j));


    //Compute the transpose of the cofactor matrix.

    MatrixData transposedCofactorMatrix(inverse.getColumn(), std::vector<long>(inverse.getColumn(), 0));

    for (int i = 0; i < inverse.getColumn(); i++)
        for (int j = 0; j < inverse.getColumn(); j++)
            transposedCofactorMatrix[j][i] = cofactorMatrix[i][j];

    //Compute the inverse matrix.

    for (int i = 0; i < inverse.getColumn(); i++)
        for (int j = 0; j < inverse.getColumn(); j++)
            inverse.inverseCodingMatrixData[i][j] = inverse.field->mul(transposedCofactorMatrix[i][j], determinantInverse);

    //Move the inverse into a new object owned by the caller. The move constructor is public, so std::make_unique can use it.
    return (std::make_unique<InverseCodingMatrix>(std::move(inverse)));

}

const std::shared_ptr<Field> &InverseCodingMatrix::getField() const {
    return this->field;
}

const std::vector<int> &InverseCodingMatrix::getLabels() const {
    return (this->labels);
}

const MatrixData &InverseCodingMatrix::getInverseCodingMatrixData() const {
    return this->inverseCodingMatrixData;
}

