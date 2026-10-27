/**
 * @file main.cpp
 * @brief Usage example of the Information Dispersal Algorithm.
 * @author Sean8819
 * @date 26/10/2026
 */

#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Field/Field.h"
#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"
#include "Ida/EncodingStep.h"
#include "Ida/FieldProduct.h"
#include "Ida/IDA.h"
#include "Matrix/CodingMatrix.h"
#include "Matrix/EncodedMatrix.h"
#include "Matrix/InverseCodingMatrix.h"
#include "Matrix/Matrix.h"
#include "Matrix/TextMatrix.h"

namespace {

/**
 * @brief How much the program prints.
 */
enum class OutputDetail {
    ResultsOnly, ///< The text, the encoded matrix, the decoded matrix and the rebuilt text.
    EveryStep    ///< Also the character codes, every product of the encoding and every matrix used.
};

/**
 * @brief Finite field used for the arithmetic.
 */
enum class FieldKind {
    Galois, ///< The Galois field GF(2^8).
    Prime   ///< The prime field GF(p).
};

/**
 * @brief Creates the field used for the arithmetic.
 * @param fieldKind Kind of field.
 * @param p Prime modulus, used only by FieldKind::Prime.
 * @return The new field.
 * @throws std::invalid_argument If the field is FieldKind::Prime and @p p is not a prime number.
 */
[[nodiscard]] std::shared_ptr<const ida::Field> createField(FieldKind fieldKind, int p) {

    if (fieldKind == FieldKind::Galois)
        return (std::make_shared<ida::GaloisField256>());

    return (std::make_shared<ida::PrimeField>(p));
}

/**
 * @brief Prints a matrix, one row per line, each value followed by a space.
 * @param label Header printed before the values.
 * @param values Values of the matrix.
 */
void printMatrix(const std::string &label, const ida::MatrixData &values) {

    std::cout << label << '\n';

    for (const std::vector<long> &row : values) {
        for (const long value : row)
            std::cout << value << " ";

        std::cout << '\n';
    }
}

/**
 * @brief Prints the codes of the characters of the text, padding included.
 * @param B Text matrix.
 */
void printCharacterCodes(const ida::TextMatrix &B) {

    std::cout << "Character codes" << '\n';

    //char may be signed: going through unsigned char prints the byte 0 ... 255 that the text matrix holds.
    for (const char character : B.getPaddedText())
        std::cout << static_cast<int>(static_cast<unsigned char>(character)) << " ";

    std::cout << '\n';
}

/**
 * @brief Prints every product of the encoding and the value of C it gives.
 * @param steps Steps of the encoding, one for each value of the encoded matrix.
 */
void printEncodingSteps(const std::vector<ida::EncodingStep> &steps) {

    for (const ida::EncodingStep &step : steps) {
        for (const ida::FieldProduct &product : step.products)
            std::cout << product.leftFactor << " * " << product.rightFactor << " = " << product.value << '\n';

        std::cout << "Result: " << step.result << '\n' << '\n';
    }
}

/**
 * @brief Prints the details of the encoding: the character codes, every product and the matrices A and B.
 * @param dispersal Object that performs the encoding.
 * @param A Coding matrix.
 * @param B Text matrix.
 */
void printEncodingDetails(const ida::IDA &dispersal, const ida::CodingMatrix &A, const ida::TextMatrix &B) {

    printCharacterCodes(B);
    std::cout << "\n--- ENCODING ---" << '\n';
    printEncodingSteps(dispersal.computeEncodingSteps(A, B));
    printMatrix("\nCoding matrix:", A.getValues());
    printMatrix("\nText matrix:", B.getValues());
}

/**
 * @brief Prints the details of the decoding: the inverse and the fragments it is multiplied by.
 * @param inverse Inverse of the coding submatrix.
 * @param C Encoded matrix.
 */
void printDecodingDetails(const ida::InverseCodingMatrix &inverse, const ida::EncodedMatrix &C) {

    const std::unique_ptr<ida::EncodedMatrix> fragments{C.getFragments(inverse.getLabels())};

    std::cout << "\n--- Decoding ---" << '\n';
    printMatrix("\nInverse matrix:", inverse.getValues());
    printMatrix("\nFragment matrix used:", fragments->getValues());
}

/**
 * @brief Rebuilds the text and prints the decoded matrix and the rebuilt text.
 * @param dispersal Object that performs the decoding.
 * @param inverse Inverse of the coding submatrix.
 * @param C Encoded matrix.
 */
void printDecoding(const ida::IDA &dispersal, const ida::InverseCodingMatrix &inverse, const ida::EncodedMatrix &C) {

    printMatrix("\nDecoded matrix:", dispersal.computeDecodedValues(inverse, C));
    std::cout << "\nRebuild data:\n" << dispersal.decode(inverse, C) << '\n';
}

} //namespace

/**
 * @brief Encodes a sample string and rebuilds it.
 *
 * Uses n = 4, m = 2 and the field chosen by fieldKind, and rebuilds the text
 * from fragments 0 and 3. outputDetail chooses whether every intermediate
 * step is printed too.
 *
 * @return EXIT_SUCCESS, or EXIT_FAILURE if a parameter is not valid.
 */
int main() {

    constexpr int n{4};
    constexpr int m{2};
    constexpr int p{257};

    //Settings: how much the program prints, and the field used for the arithmetic (GF(2^8) or GF(p)).
    constexpr OutputDetail outputDetail{OutputDetail::ResultsOnly};
    constexpr FieldKind fieldKind{FieldKind::Galois};

    const std::string data{"Il barone Fanfulla da Lodi, condottier di gran rinomanza fu condotto una sera in stanza da "
                           "una donna di facili amor!"};

    //Fragments, i.e. rows of the encoded matrix, used to rebuild the text: any m of the n are enough.
    const std::vector<int> labels{0, 3};

    try {
        std::cout << "\nData: " << data << '\n';

        //The field is shared by the coding matrix, its inverse and the IDA object.
        const std::shared_ptr<const ida::Field> field{createField(fieldKind, p)};
        const ida::CodingMatrix A{n, m, field};
        const ida::TextMatrix B{m, data};
        const ida::IDA dispersal{n, m, field};
        const std::unique_ptr<ida::EncodedMatrix> C{dispersal.encode(A, B)};

        if (outputDetail == OutputDetail::EveryStep)
            printEncodingDetails(dispersal, A, B);

        printMatrix("\nEncoded matrix:", C->getValues());

        const std::unique_ptr<ida::InverseCodingMatrix> inverse{ida::InverseCodingMatrix::getInverse(A, labels)};

        if (outputDetail == OutputDetail::EveryStep)
            printDecodingDetails(*inverse, *C);

        printDecoding(dispersal, *inverse, *C);
    }
    catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
