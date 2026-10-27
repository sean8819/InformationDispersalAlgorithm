/**
 * @file Field.h
 * @brief Abstract base class for the finite fields used by the IDA.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

namespace ida {

/**
 * @brief Finite field whose elements are the integers 0 ... size - 1.
 *
 * Each subclass implements addition, subtraction and multiplication.
 * Powers and inverses are computed here from multiplication.
 *
 * The contract every subclass keeps: when the arguments of an operation are
 * elements of the field, its result is an element of the field too, and the
 * operations satisfy the field axioms.
 */
class Field {

protected:

    //Copy and move are protected: a Field cannot be copied through a base
    //reference (slicing), but every subclass can still be copied and moved.

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

public:

    /**
     * @brief Creates a field.
     */
    Field() = default;

    /**
     * @brief Adds two elements.
     * @param a First element.
     * @param b Second element.
     * @return The sum a + b.
     */
    [[nodiscard]] virtual long add(long a, long b) const = 0;

    /**
     * @brief Subtracts two elements.
     * @param a First element.
     * @param b Element to subtract.
     * @return The difference a - b.
     */
    [[nodiscard]] virtual long subtract(long a, long b) const = 0;

    /**
     * @brief Multiplies two elements.
     * @param a First element.
     * @param b Second element.
     * @return The product a · b.
     */
    [[nodiscard]] virtual long multiply(long a, long b) const = 0;

    /**
     * @brief Returns the number of elements of the field.
     * @return Number of elements.
     */
    [[nodiscard]] virtual int getSize() const = 0;

    /**
     * @brief Raises an element to a power by repeated multiplication.
     * @param base Element to raise.
     * @param exponent Exponent.
     * @return The power base^exponent; base^0 is 1.
     * @pre @p exponent is not negative.
     * @throws std::invalid_argument If @p exponent is negative.
     */
    [[nodiscard]] long computePower(long base, int exponent) const;

    /**
     * @brief Computes the multiplicative inverse of an element.
     *
     * In a field with q elements every non-zero a satisfies a^(q-1) = 1,
     * so its inverse is a^(q-2).
     *
     * @param a Element to invert.
     * @return The inverse of @p a.
     * @pre @p a is not zero.
     * @post a · result = 1.
     * @throws std::invalid_argument If @p a is zero.
     */
    [[nodiscard]] long computeInverse(long a) const;

    /**
     * @brief Virtual destructor.
     */
    virtual ~Field() = default;
};

} //namespace ida
