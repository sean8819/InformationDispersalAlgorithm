/**
 * @file EncodedMatrix.h
 * @brief Encoded matrix produced by the IDA.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include <memory>
#include <vector>

#include "Matrix/Matrix.h"

namespace ida {

/**
 * @brief Encoded matrix C = A · B, computed in the field.
 *
 * Each row is a fragment. It also stores how many padding characters were
 * added to the text, so that decoding can remove them.
 *
 * @invariant The padding is not negative.
 */
class EncodedMatrix final : public Matrix {

private:

    int padding; ///< Number of padding characters added to the text.

public:

    /**
     * @brief Creates an encoded matrix.
     * @param values Encoded values, one row for each fragment.
     * @param padding Number of padding characters added to the text.
     * @throws std::invalid_argument If @p padding is negative.
     */
    EncodedMatrix(MatrixData values, int padding);

    /**
     * @brief Extracts the selected fragments, i.e. rows.
     * @param labels Indices of the rows to extract.
     * @return New matrix with one row for each label and the same padding, owned by the caller.
     * @throws std::out_of_range If a label is not the index of a row.
     */
    [[nodiscard]] std::unique_ptr<EncodedMatrix> selectFragments(const std::vector<int> &labels) const;

    /**
     * @brief Returns the number of padding characters.
     * @return Number of padding characters.
     */
    [[nodiscard]] int getPadding() const;
};

} //namespace ida
