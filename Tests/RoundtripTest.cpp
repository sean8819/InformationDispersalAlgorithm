/**
 * @file RoundtripTest.cpp
 * @brief Encodes and decodes several texts with both fields and every choice of fragments.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Ida/IDA.h"
#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"
#include "TestUtil.h"

int decodes = 0;    ///< Number of decodings performed.

/**
 * @brief Decodes the encoded matrix with every choice of m fragments out of n.
 *
 * Fills @p labels recursively with increasing indices. When all m labels are
 * set, decodes and compares the result with the original text.
 *
 * @param ida Object that performs the decoding.
 * @param A Coding matrix.
 * @param C Encoded matrix.
 * @param data Original text.
 * @param labels Vector of m elements, filled here with the indices of the fragments.
 * @param n Number of fragments.
 * @param count Number of labels already set.
 * @param start First index that can be used for the next label.
 * @param description Description of the test case, printed on failure.
 */
void decodeAllCombinations(const IDA &ida, const CodingMatrix &A, const EncodedMatrix &C, const std::string &data,
                           std::vector<int> &labels, int n, int count, int start, const std::string &description) {

    if (count == static_cast<int>(labels.size())) {
        std::unique_ptr<InverseCodingMatrix> inverse = InverseCodingMatrix::getInverse(A, labels);

        check(inverse != nullptr, description + ": submatrix is invertible");

        if (inverse != nullptr) {
            check(ida.decode(*inverse, C, labels) == data, description + ": text is rebuilt");
            decodes++;
        }
    }
    else {
        for (int i = start; i < n; i++) {
            labels[count] = i;
            decodeAllCombinations(ida, A, C, data, labels, n, count + 1, i + 1, description);
        }
    }
}

/**
 * @brief Encodes a text, checks that every encoded value is a field element and decodes it with every choice of fragments.
 * @param field Field used for the arithmetic.
 * @param fieldName Name of the field, printed on failure.
 * @param n Number of fragments generated.
 * @param m Number of fragments needed for reconstruction.
 * @param data Text to encode.
 */
void roundtrip(const std::shared_ptr<Field> &field, const std::string &fieldName, int n, int m, const std::string &data) {

    std::string description = fieldName + " n=" + std::to_string(n) + " m=" + std::to_string(m) +
                              " length=" + std::to_string(data.length());

    CodingMatrix A(n, m, field);
    TextMatrix B(m, data);

    IDA ida(n, m, field, false);

    std::unique_ptr<EncodedMatrix> C = ida.encode(A, B);

    check(C != nullptr, description + ": text is encoded");

    if (C == nullptr)
        return;

    for (int i = 0; i < C->getRow(); i++)
        for (int j = 0; j < C->getColumn(); j++)
            check(C->getDataEncoded()[i][j] >= 0 && C->getDataEncoded()[i][j] < field->getSize(),
                  description + ": encoded value is a field element");

    std::vector<int> labels(m);

    decodeAllCombinations(ida, A, *C, data, labels, n, 0, 0, description);
}

/**
 * @brief Runs the round-trip tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    //The classes print every step to standard output: silence it while the tests run.
    std::cout.setstate(std::ios_base::failbit);

    std::shared_ptr<Field> fields[] = {std::make_shared<GaloisField256>(), std::make_shared<PrimeField>(257)};
    std::string fieldNames[] = {"GF(2^8)", "GF(257)"};

    //Text with every byte value from 1 to 255.
    std::string allBytes;

    for (int byteValue = 1; byteValue < 256; byteValue++)
        allBytes += static_cast<char>(byteValue);

    //"abcde" with m = 4 needs 3 padding characters to fill a 4 x 2 matrix.
    std::string texts[] = {"Il barone Fanfulla da Lodi, condottier di gran rinomanza ...",
                           "Hello, world!",
                           "abcdefghij",
                           "abcde",
                           allBytes};

    //Pairs (n, m).
    int parameters[][2] = {{3, 1}, {4, 2}, {6, 3}, {10, 4}, {12, 5}};

    for (int fieldIndex = 0; fieldIndex < 2; fieldIndex++)
        for (int textIndex = 0; textIndex < 5; textIndex++)
            for (int parameterIndex = 0; parameterIndex < 5; parameterIndex++)
                roundtrip(fields[fieldIndex], fieldNames[fieldIndex],
                          parameters[parameterIndex][0], parameters[parameterIndex][1], texts[textIndex]);

    std::cout.clear();

    return report("All round-trip tests passed (" + std::to_string(decodes) + " decodings)");
}
