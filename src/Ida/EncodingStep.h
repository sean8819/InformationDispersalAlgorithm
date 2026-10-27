/**
 * @file EncodingStep.h
 * @brief How one value of the encoded matrix is computed.
 * @author Sean8819
 * @date 27/10/2026
 */

#pragma once

#include <vector>

#include "Ida/FieldProduct.h"

namespace ida {

/**
 * @brief Computation of one value C[i][j] of the encoded matrix: the products A[i][k] · B[k][j] and their sum.
 */
struct EncodingStep {
    std::vector<FieldProduct> products; ///< One product for each column k of the coding matrix, in order.
    long result{0};                     ///< Sum of the products in the field: the value C[i][j].
};

} //namespace ida
