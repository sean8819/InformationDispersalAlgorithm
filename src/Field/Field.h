/**
 * @file Field.h
 * @brief Abstract base class for the finite fields used by the IDA.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include<iostream>

/**
 * @brief Finite field whose elements are the integers 0 ... size - 1.
 *
 * Each subclass implements addition, subtraction and multiplication.
 * Powers and inverses are computed here from multiplication.
 */
class Field {

public:

    /**
     * @brief Creates a field.
     */
    Field() = default;

    //The class declares a virtual destructor, so it declares copy and move explicitly too (rule of five).

    /**
     * @brief Copies a field.
     * @param other Field to copy.
     */
    Field(const Field &other) = default;

    /**
     * @brief Moves a field.
     * @param other Field to move.
     */
    Field(Field &&other) = default;

    /**
     * @brief Copies a field into this one.
     * @param other Field to copy.
     * @return This field.
     */
    Field &operator=(const Field &other) = default;

    /**
     * @brief Moves a field into this one.
     * @param other Field to move.
     * @return This field.
     */
    Field &operator=(Field &&other) = default;

    /**
     * @brief Adds two elements.
     * @param a First element.
     * @param b Second element.
     * @return The sum a + b.
     */
    virtual long add(long a, long b) = 0;

    /**
     * @brief Subtracts two elements.
     * @param a First element.
     * @param b Element to subtract.
     * @return The difference a - b.
     */
    virtual long sub(long a, long b) = 0;

    /**
     * @brief Multiplies two elements.
     * @param a First element.
     * @param b Second element.
     * @return The product a · b.
     */
    virtual long mul(long a, long b) = 0;

    /**
     * @brief Returns the number of elements of the field.
     * @return Number of elements.
     */
    virtual int getSize() const = 0;

    /**
     * @brief Raises an element to a power by repeated multiplication.
     * @param base Element to raise.
     * @param exponent Non-negative exponent.
     * @return The power base^exponent.
     */
    long pow(long base, int exponent);

    /**
     * @brief Returns the multiplicative inverse of an element.
     *
     * In a field with q elements every non-zero a satisfies a^(q-1) = 1,
     * so its inverse is a^(q-2).
     *
     * @param a Element to invert.
     * @return The inverse of @p a, or -1 if @p a is zero.
     */
    long inv(long a);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Field() = default;

};
