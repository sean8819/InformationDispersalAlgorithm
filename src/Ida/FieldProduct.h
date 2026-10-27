/**
 * @file FieldProduct.h
 * @brief One product of two elements computed in a field.
 * @author Sean8819
 * @date 27/10/2026
 */

#pragma once

namespace ida {

/**
 * @brief One product of a row-by-column product of matrices: the two factors and their product in the field.
 */
struct FieldProduct {
    long leftFactor{0};  ///< Factor taken from the left matrix, e.g. the coding matrix A.
    long rightFactor{0}; ///< Factor taken from the right matrix, e.g. the text matrix B.
    long value{0};       ///< Product of the two factors in the field.
};

} //namespace ida
