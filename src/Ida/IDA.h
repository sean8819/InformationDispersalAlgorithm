/**
 * @file IDA.h
 * @brief Encoding and decoding with the Information Dispersal Algorithm.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once


#include <string>
#include "Matrix/CodingMatrix.h"
#include "Matrix/TextMatrix.h"
#include "Matrix/EncodedMatrix.h"
#include "Matrix/InverseCodingMatrix.h"

/**
 * @brief Information Dispersal Algorithm (IDA).
 *
 * Splits data into n fragments so that any m of them (m < n) are enough to
 * rebuild it. All computations are done in a finite field: the prime field
 * GF(p) or the Galois field GF(2^8).
 */
class IDA {

private:

    int n;          ///< Number of fragments generated.
    int m;          ///< Number of fragments needed for reconstruction.
    std::shared_ptr<Field> field;   ///< Field used for the arithmetic, shared with the matrices.
    bool verbose;   ///< If true, encode() and decode() also print every intermediate step.

public:

    /**
     * @brief Sets the algorithm parameters.
     *
     * Prints an error and terminates the program if @p field is null, @p m is
     * less than 1 or not strictly less than @p n, @p field has fewer than 256
     * elements, or @p n is not less than the number of elements of @p field.
     *
     * @param n Number of fragments generated.
     * @param m Number of fragments needed for reconstruction.
     * @param field Field used for the arithmetic; the matrices must use the same object.
     * @param verbose If true, encode() and decode() also print every intermediate step.
     */
    IDA(int n, int m, std::shared_ptr<Field> field, bool verbose = false);

    /**
     * @brief Encodes the text by computing C = A · B in the field.
     *
     * Each row of C is a fragment. The padding of @p B is copied into C.
     * Prints the encoded matrix; with verbose, also the character codes, every
     * product and the matrices A and B.
     *
     * @param A Coding matrix (n x m), using the same field object as this object.
     * @param B Text matrix (m x columns).
     * @return New encoded matrix (n x columns), owned by the caller, or nullptr
     *         if @p A uses another field or the dimensions are not compatible.
     */
    std::unique_ptr<EncodedMatrix> encode(const CodingMatrix &A, const TextMatrix &B) const;

    /**
     * @brief Rebuilds the original text from m fragments.
     *
     * Multiplies @p A by the fragments of @p C selected by @p labels in the
     * field and removes the padding. Prints the decoded matrix; with verbose,
     * also the inverse and the fragments used.
     *
     * @param A Inverse of the coding submatrix (m x m), obtained from
     *          InverseCodingMatrix::getInverse() with the same @p labels.
     * @param C Encoded matrix.
     * @param labels Indices of the m fragments to use.
     * @return The original text, or an empty string if @p A uses another field,
     *         is not m x m or was computed for other labels, @p labels does not
     *         hold m indices, or a label is not the index of a fragment.
     */
    std::string decode(const InverseCodingMatrix &A, const EncodedMatrix &C, const std::vector<int> &labels) const;

};
