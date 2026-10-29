/**
 * @file Settings.h
 * @brief Parameters of the console and their choices.
 * @author Sean8819
 * @date 29/10/2026
 */

#pragma once

#include <vector>

namespace ida {

/**
 * @brief How much the console prints for a sentence.
 */
enum class OutputDetail {
    ResultsOnly, ///< The text, the encoded matrix, the decoded matrix and the rebuilt text.
    EveryStep    ///< Also the character codes, every product of the encoding and every matrix used.
};

/**
 * @brief Coding matrix, which also decides how the inverse is computed.
 */
enum class MatrixKind {
    Vandermonde, ///< Powers of distinct points, inverted with Gauss-Jordan elimination in O(m^3) operations.
    Cauchy       ///< 1 / (x_i + y_j), inverted with Rabin's formula in O(m^2) operations.
};

/**
 * @brief Finite field used for the arithmetic.
 */
enum class FieldKind {
    Galois, ///< The Galois field GF(2^8).
    Prime   ///< The prime field GF(p).
};

constexpr int defaultN{4}; ///< Number of fragments generated when the console starts.
constexpr int defaultM{2}; ///< Number of fragments needed for reconstruction when the console starts.

/**
 * @brief Modulus of the prime field: the smallest prime above 255, so every code from 0 to 255 is an element.
 */
constexpr int primeModulus{257};

/**
 * @brief Parameters of the console, changed with the set command.
 */
struct Settings {
    int n{defaultN};                                      ///< Number of fragments generated.
    int m{defaultM};                                      ///< Number of fragments needed for reconstruction.
    int p{primeModulus};                                  ///< Prime modulus, used only by FieldKind::Prime.
    OutputDetail outputDetail{OutputDetail::ResultsOnly}; ///< How much the console prints.
    FieldKind fieldKind{FieldKind::Galois};               ///< Field used for the arithmetic: GF(2^8) or GF(p).
    MatrixKind matrixKind{MatrixKind::Vandermonde};       ///< Coding matrix: Vandermonde or Cauchy.
    std::vector<int> labels{0, defaultN - 1}; ///< Fragments used to rebuild the text: the first and the last.
};

} //namespace ida
