/**
 * @file IDA.cpp
 * @brief Implementation of the IDA class.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Ida/IDA.h"

IDA::IDA(int n, int m, std::shared_ptr<Field> field, bool verbose) {

    if (field == nullptr) {
        std::cerr << "The field cannot be null" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (m < 1) {
        std::cerr << "The parameter m must be at least 1" << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (m >= n) {
        std::cerr << "The parameter m must be strictly less than n" << std::endl;
        exit(EXIT_FAILURE);
    }
    //Every byte value 0 ... 255 must be an element of the field, otherwise some characters would be lost.
    else if (field->getSize() < 256) {
        std::cerr << "The field must have at least 256 elements, one for each byte value" << std::endl;
        exit(EXIT_FAILURE);
    }
    //The coding matrix uses the n distinct non-zero elements 1 ... n, so n must be less than the field size.
    else if (n >= field->getSize()) {
        std::cerr << "The parameter n must be less than the number of elements of the field" << std::endl;
        exit(EXIT_FAILURE);
    }
    else {
        this->n = n;
        this->m = m;
        this->field = std::move(field);
        this->verbose = verbose;
    }

}

std::unique_ptr<EncodedMatrix> IDA::encode(const CodingMatrix &A, const TextMatrix &B) const {

    if (A.getField() != this->field) {
        std::cerr << "The coding matrix must use the same field object as the IDA object!" << std::endl;
        return (nullptr);
    }
    else if (A.getRow() != this->n || A.getColumn() != this->m) {
        std::cerr << "The coding matrix must have n rows and m columns!" << std::endl;
        return (nullptr);
    }
    else if (A.getColumn() != B.getRow()) {
        std::cerr << "The number of columns of A must equal the number of rows of B!" << std::endl;
        return (nullptr);
    }
    else {

        if (this->verbose) {
            B.printCharacterCodes();
            std::cout << "\n--- ENCODING ---" << std::endl;
        }
        //Encode the data.

        int row = A.getRow();
        int column = B.getColumn();

        const MatrixData &matrixValueA = A.getCodingMatrix();
        const MatrixData &matrixValueB = B.getTextMatrix();

        long product = 0;

        EncodedMatrix C(row, column);

        //To encode, compute the row-by-column product of the coding matrix A and the text matrix B in the field.
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < column; j++) {
                for (int k = 0; k < A.getColumn(); k++) {

                    product = this->field->mul(matrixValueA[i][k], matrixValueB[k][j]);

                    C.getDataEncoded()[i][j] = this->field->add(C.getDataEncoded()[i][j], product);

                    if (this->verbose)
                        std::cout << matrixValueA[i][k] << " * " << matrixValueB[k][j] << " = " << product << std::endl;

                }

                if (this->verbose)
                    std::cout << "Result: " << C.getDataEncoded()[i][j] << std::endl << std::endl;
            }
        }

        C.setPadding(B.getPadding());

        if (this->verbose) {
            A.printMatrix();
            B.printMatrix();
        }

        C.printMatrix();

        //Move the encoded matrix into a new object owned by the caller.
        return (std::make_unique<EncodedMatrix>(std::move(C)));
    }

}

std::string IDA::decode(const InverseCodingMatrix &A, const EncodedMatrix &C, const std::vector<int> &labels) const {

    if (A.getField() != this->field) {
        std::cerr << "The inverse must use the same field object as the IDA object!" << std::endl;
        return ("");
    }
    else if (A.getRow() != this->m || A.getColumn() != this->m) {
        std::cerr << "The inverse must have m rows and m columns!" << std::endl;
        return ("");
    }
    else if (static_cast<int>(labels.size()) != this->m) {
        std::cerr << "There must be exactly m labels!" << std::endl;
        return ("");
    }
    //The inverse rebuilds the text only from the fragments it was computed for.
    else if (A.getLabels() != labels) {
        std::cerr << "The labels must be the same ones used to compute the inverse!" << std::endl;
        return ("");
    }

    if (this->verbose)
        std::cout << "\n--- Decoding ---" << std::endl;

    std::unique_ptr<EncodedMatrix> fragments = C.getFragments(labels);

    if (fragments == nullptr)
        return ("");

    int padding = C.getPadding();

    if (this->verbose) {
        A.printMatrix();
        fragments->printMatrix("\nFragment matrix used:");
    }

    //Multiply the inverse of the coding submatrix by the matrix of the selected fragments to rebuild the original string.
    MatrixData decoded(this->m, std::vector<long>(C.getColumn(), 0));
    std::string rebuiltText;

    for (int k = 0; k < C.getColumn(); k++)
        for (int i = 0; i < A.getRow(); i++) {
            for (int j = 0; j < A.getColumn(); j++)
                decoded[i][k] = this->field->add(decoded[i][k], this->field->mul(A.getInverseCodingMatrixData()[i][j],
                                                                                 fragments->getDataEncoded()[j][k]));

            rebuiltText += static_cast<char>(decoded[i][k]);
        }

    std::cout << "\nDecoded matrix:" << std::endl;

    for (int i = 0; i < this->m; i++) {
        for (int k = 0; k < C.getColumn(); k++)
            std::cout << decoded[i][k] << " ";

        std::cout << std::endl;
    }

    return (rebuiltText.substr(0, rebuiltText.length() - padding));
}
