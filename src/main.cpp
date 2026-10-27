/**
 * @file main.cpp
 * @brief Usage example of the Information Dispersal Algorithm.
 * @author Sean8819
 * @date 26/10/2026
 */

#include <iostream>
#include "Matrix/EncodedMatrix.h"
#include "Matrix/InverseCodingMatrix.h"

#include "Field/PrimeField.h"
#include "Field/GaloisField256.h"

#include "Ida/IDA.h"

/**
 * @brief Encodes a sample string and rebuilds it.
 *
 * Uses n = 4, m = 2 and the Galois field GF(2^8), or the prime field GF(257)
 * if galoisField is false, and rebuilds the text from fragments 0 and 3.
 *
 * @return EXIT_SUCCESS, or EXIT_FAILURE if encoding or inversion fails.
 */
int main() {

    int n = 4, m = 2, p = 257;

    bool verbose = false;

    //If true use the Galois field GF(2^8), otherwise the prime field GF(p).
    bool galoisField = true;

    std::string data = "Il barone Fanfulla da Lodi, condottier di gran rinomanza ...";

    std::cout << "\nData: " << data << std::endl;

    //Create the field used for the arithmetic. It is shared by the coding matrix, its inverse and the IDA object.
    std::shared_ptr<Field> field;

    if (galoisField)
        field = std::make_shared<GaloisField256>();
    else
        field = std::make_shared<PrimeField>(p);

    //Create the coding matrix.
    CodingMatrix A(n, m, field);

    //Create the matrix holding the values to encode. It computes its number of columns from the text.
    TextMatrix B(m, data);

    //Create the IDA object that performs encoding and decoding.
    IDA ida(n, m, field, verbose);

    //Create the encoded matrix.
    auto C = ida.encode(A, B);

    //Select the fragments, i.e. the matrix rows used to rebuild the information.
    std::vector<int> labels = {0, 3};

    //Compute the inverse.
    auto inverseCodingMatrix = InverseCodingMatrix::getInverse(A, labels);

    //encode() and getInverse() return nullptr when they fail: stop before decoding.
    if (C == nullptr || inverseCodingMatrix == nullptr) {
        std::cerr << "Encoding or inversion failed" << std::endl;
        return EXIT_FAILURE;
    }

    //Decode the information.
    std::string originalData = ida.decode(*inverseCodingMatrix, *C, labels);

    //Print the results.
    std::cout << "\nRebuild data:\n" << originalData << std::endl;

    return EXIT_SUCCESS;

}
