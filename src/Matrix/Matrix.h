/**
 * @file Matrix.h
 * @brief Abstract base class for the project's matrices.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include<iostream>
#include<string>
#include<utility>
#include<cmath>
#include<vector>
#include<memory>

/**
 * @brief Values of a matrix: a vector of rows, each one a vector of values.
 *
 * The vectors free their memory by themselves when the matrix is destroyed.
 */
using MatrixData = std::vector<std::vector<long>>;

/**
 * @brief Generic matrix; the base class only knows its dimensions.
 *
 * Each subclass manages its own data and implements printMatrix().
 */
class Matrix {

private:

    int row;    ///< Number of rows.
    int column; ///< Number of columns.

public:

    /**
     * @brief Creates a matrix with the given dimensions.
     * @param row Number of rows.
     * @param column Number of columns.
     */
    Matrix(int row, int column);

    /**
     * @brief Creates a 0 x 0 matrix.
     */
    Matrix();

    //The class declares a virtual destructor, so it declares copy and move explicitly too (rule of five).

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

    /**
     * @brief Sets the number of rows.
     * @param row New number of rows.
     */
    void setRow(int row);

    /**
     * @brief Sets the number of columns.
     * @param column New number of columns.
     */
    void setColumn(int column);

    /**
     * @brief Returns the number of rows.
     * @return Number of rows.
     */
    int getRow() const;

    /**
     * @brief Returns the number of columns.
     * @return Number of columns.
     */
    int getColumn() const;

    /**
     * @brief Prints the matrix to standard output.
     * @param label Header printed before the values.
     */
    virtual void printMatrix(const std::string &label) const = 0;

    /**
     * @brief Virtual destructor.
     */
    virtual ~Matrix() = default;

};
