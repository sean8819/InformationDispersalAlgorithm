/**
 * @file IDA.h
 * @brief Encoding and decoding with the Information Dispersal Algorithm.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Ida/EncodingStep.h"
#include "Ida/FieldProduct.h"
#include "Matrix/Matrix.h"

namespace ida {

class CodingMatrix;
class EncodedMatrix;
class Field;
class InverseCodingMatrix;
class TextMatrix;

/**
 * @brief Information Dispersal Algorithm (IDA).
 *
 * Splits data into n fragments so that any m of them (m < n) are enough to
 * rebuild it. All computations are done in a finite field: the prime field
 * GF(p) or the Galois field GF(2^8).
 *
 * The object only computes: it prints nothing. Besides the results, it
 * exposes the intermediate ones (computeEncodingSteps(), computeDecodedValues())
 * for whoever wants to show them.
 *
 * @invariant The field is not null and has at least 256 elements, and 1 <= m < n < size of the field.
 */
class IDA final {

private:

    static constexpr int minimumFieldSize{256}; ///< Every byte value 0 ... 255 must be an element of the field.

    int n;                              ///< Number of fragments generated.
    int m;                              ///< Number of fragments needed for reconstruction.
    std::shared_ptr<const Field> field; ///< Field used for the arithmetic, shared with the matrices.

    /**
     * @brief Checks that a coding matrix and a text matrix can be encoded by this object.
     * @param A Coding matrix.
     * @param B Text matrix.
     * @throws std::invalid_argument If @p A uses another field object or is not n x m, or @p B does not have m
     *         rows.
     */
    void checkEncodingArguments(const CodingMatrix &A, const TextMatrix &B) const;

    /**
     * @brief Computes the row-by-column product of two matrices in the field.
     * @param left Left matrix.
     * @param right Right matrix, with as many rows as @p left has columns.
     * @return The product, with the rows of @p left and the columns of @p right.
     */
    [[nodiscard]] MatrixData multiplyMatrices(const Matrix &left, const Matrix &right) const;

    /**
     * @brief Computes the products of a row by a column, value by value, in the field.
     * @param leftRow Row of the left matrix.
     * @param right Right matrix, with one row for each value of @p leftRow.
     * @param column Index of the column of @p right.
     * @return One product for each value of @p leftRow, in order.
     */
    [[nodiscard]] std::vector<FieldProduct> computeProducts(const std::vector<long> &leftRow, const Matrix &right,
                                                            int column) const;

    /**
     * @brief Adds products in the field.
     * @param products Products to add.
     * @return The sum of the values of @p products; 0 if there are none.
     */
    [[nodiscard]] long addProducts(const std::vector<FieldProduct> &products) const;

    /**
     * @brief Checks that an inverse can be used by this object.
     * @param inverse Inverse of the coding submatrix.
     * @throws std::invalid_argument If @p inverse uses another field object or is not m x m.
     */
    void checkInverse(const InverseCodingMatrix &inverse) const;

public:

    /**
     * @brief Sets the algorithm parameters.
     * @param n Number of fragments generated.
     * @param m Number of fragments needed for reconstruction.
     * @param field Field used for the arithmetic; the matrices must use the same object.
     * @throws std::invalid_argument If @p field is null, @p m is less than 1 or not strictly less than @p n,
     *         @p field has fewer than 256 elements, or @p n is not less than the number of elements of @p field.
     */
    IDA(int n, int m, std::shared_ptr<const Field> field);

    /**
     * @brief Encodes the text by computing C = A · B in the field.
     *
     * Each row of C is a fragment. The padding of @p B is copied into C.
     *
     * @param A Coding matrix (n x m), using the same field object as this object.
     * @param B Text matrix (m x columns).
     * @return New encoded matrix (n x columns), owned by the caller.
     * @throws std::invalid_argument If @p A uses another field object or is not n x m, or @p B does not have m
     *         rows.
     */
    [[nodiscard]] std::unique_ptr<EncodedMatrix> encode(const CodingMatrix &A, const TextMatrix &B) const;

    /**
     * @brief Computes every step of the encoding C = A · B, for whoever wants to show it.
     * @param A Coding matrix (n x m), using the same field object as this object.
     * @param B Text matrix (m x columns).
     * @return One step for each value of C, row by row: its products A[i][k] · B[k][j] and their sum C[i][j].
     * @post The result of each step is the matching value of the matrix returned by encode().
     * @throws std::invalid_argument If @p A uses another field object or is not n x m, or @p B does not have m
     *         rows.
     */
    [[nodiscard]] std::vector<EncodingStep> computeEncodingSteps(const CodingMatrix &A, const TextMatrix &B) const;

    /**
     * @brief Rebuilds the original text from the m fragments the inverse was computed for.
     *
     * Multiplies @p inverse by the fragments of @p C selected by its labels in
     * the field, reads the text and removes the padding.
     *
     * @param inverse Inverse of the coding submatrix (m x m), built for the chosen fragments.
     * @param C Encoded matrix.
     * @return The original text.
     * @throws std::invalid_argument If @p inverse uses another field object or is not m x m.
     * @throws std::out_of_range If a label of @p inverse is not the index of a row of @p C.
     */
    [[nodiscard]] std::string decode(const InverseCodingMatrix &inverse, const EncodedMatrix &C) const;

    /**
     * @brief Computes the decoded matrix: the text matrix rebuilt from the fragments, padding included.
     * @param inverse Inverse of the coding submatrix (m x m), built for the chosen fragments.
     * @param C Encoded matrix.
     * @return The product of @p inverse by the fragments of @p C selected by its labels (m x columns).
     * @throws std::invalid_argument If @p inverse uses another field object or is not m x m.
     * @throws std::out_of_range If a label of @p inverse is not the index of a row of @p C.
     */
    [[nodiscard]] MatrixData computeDecodedValues(const InverseCodingMatrix &inverse, const EncodedMatrix &C) const;
};

} //namespace ida
