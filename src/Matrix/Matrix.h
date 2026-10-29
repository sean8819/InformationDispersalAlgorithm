/**
 * @file Matrix.h
 * @brief Base class for the project's matrices.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include <vector>

namespace ida {

/**
 * @brief Values of a matrix: a vector of rows, each one a vector of values.
 *
 * The vectors free their memory by themselves when the matrix is destroyed.
 */
using MatrixData = std::vector<std::vector<long>>;

/**
 * @brief Generic matrix: its values and the operations common to every kind of matrix.
 *
 * Each subclass computes its values in its constructor; the values never
 * change afterwards.
 *
 * @invariant Every row has the same number of values.
 */
class Matrix {

private:

    MatrixData values; ///< Values, one vector for each row.

protected:

    /**
     * @brief Creates a matrix with the given values; only subclasses create matrices.
     * @param values Values, one vector for each row, every row with the same length.
     */
    explicit Matrix(MatrixData values);

    /**
     * @brief Checks that a label is the index of a row.
     * @param label Index of a row, i.e. of a fragment.
     * @throws std::out_of_range If @p label is not the index of a row.
     */
    void checkRowLabel(int label) const;

    //Copy and move are protected: a Matrix cannot be copied through a base
    //reference (slicing), but every subclass can still be copied and moved.

    /**
     * @brief Copies a matrix.
     * @param other Matrix to copy.
     */
    Matrix(const Matrix &other) = default;

    /**
     * @brief Moves a matrix.
     * @param other Matrix to move.
     */
    Matrix(Matrix &&other) = default;

    /**
     * @brief Copies a matrix into this one.
     * @param other Matrix to copy.
     * @return This matrix.
     */
    Matrix &operator=(const Matrix &other) = default;

    /**
     * @brief Moves a matrix into this one.
     * @param other Matrix to move.
     * @return This matrix.
     */
    Matrix &operator=(Matrix &&other) = default;

public:

    /**
     * @brief Returns the number of rows.
     * @return Number of rows.
     */
    [[nodiscard]] int getRow() const;

    /**
     * @brief Returns the number of columns.
     * @return Number of columns; 0 for a matrix without rows.
     */
    [[nodiscard]] int getColumn() const;

    /**
     * @brief Returns the matrix values.
     * @return Reference to the internal values, not a copy.
     */
    [[nodiscard]] const MatrixData &getValues() const;

    /**
     * @brief Copies the selected rows, i.e. fragments.
     * @param labels Indices of the rows to copy, in the order they are wanted.
     * @return The values of the selected rows, one row for each label.
     * @throws std::out_of_range If a label is not the index of a row.
     */
    [[nodiscard]] MatrixData selectRows(const std::vector<int> &labels) const;

    /**
     * @brief Virtual destructor.
     */
    virtual ~Matrix() = default;
};

} //namespace ida
