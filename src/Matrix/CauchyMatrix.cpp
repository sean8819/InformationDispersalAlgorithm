/**
 * @file CauchyMatrix.cpp
 * @brief Implementation of the CauchyMatrix class.
 * @author Sean8819
 * @date 29/10/2026
 */

#include "Matrix/CauchyMatrix.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Field/Field.h"

namespace ida {

namespace {

/**
 * @brief Computes the point of a column: y_j = -(n + j), so that x_i + y_j = i - (n + j) is never zero.
 * @param n Number of rows.
 * @param j Index of the column.
 * @param field Field used for the arithmetic.
 * @return The element -(n + j) of the field.
 */
[[nodiscard]] long computeColumnPoint(int n, int j, const Field &field) {
    return (field.subtract(0, n + j));
}

/**
 * @brief Computes the points of every column.
 * @param n Number of rows.
 * @param m Number of columns.
 * @param field Field used for the arithmetic.
 * @return The points y_0 ... y_(m-1).
 */
[[nodiscard]] std::vector<long> computeColumnPoints(int n, int m, const Field &field) {

    std::vector<long> columnPoints{};

    columnPoints.reserve(m);

    for (int j = 0; j < m; j++)
        columnPoints.push_back(computeColumnPoint(n, j, field));

    return (columnPoints);
}

/**
 * @brief Computes the points of the chosen rows: x_i = i.
 * @param labels Indices of the rows.
 * @return The points of the rows, in the order of @p labels.
 */
[[nodiscard]] std::vector<long> computeRowPoints(const std::vector<int> &labels) {

    std::vector<long> rowPoints{};

    rowPoints.reserve(labels.size());

    for (const int label : labels)
        rowPoints.push_back(label);

    return (rowPoints);
}

/**
 * @brief Multiplies the differences between a point and the other points of its list, in the order of the list.
 *
 * It is c_k for the points x and d_k for the points y in Rabin's paper:
 * the product of (p_i - p_k) for every i < k and of (p_k - p_j) for every
 * j > k.
 *
 * @param points Distinct points.
 * @param k Index of the point.
 * @param field Field used for the arithmetic.
 * @return The product, never zero because the points are distinct.
 */
[[nodiscard]] long multiplyDifferences(const std::vector<long> &points, int k, const Field &field) {

    const int size{static_cast<int>(points.size())};
    long product{1};

    for (int i = 0; i < k; i++)
        product = field.multiply(product, field.subtract(points[i], points[k]));

    for (int j = k + 1; j < size; j++)
        product = field.multiply(product, field.subtract(points[k], points[j]));

    return (product);
}

/**
 * @brief Multiplies the sums of a point with every point of the other list.
 *
 * It is e_k = prod_j (x_k + y_j) for a point x and f_k = prod_i (x_i + y_k)
 * for a point y in Rabin's paper.
 *
 * @param point Point of one list.
 * @param otherPoints Points of the other list.
 * @param field Field used for the arithmetic.
 * @return The product of the sums.
 */
[[nodiscard]] long multiplySums(long point, const std::vector<long> &otherPoints, const Field &field) {

    long product{1};

    for (const long otherPoint : otherPoints)
        product = field.multiply(product, field.add(point, otherPoint));

    return (product);
}

/**
 * @brief Computes the factor of each point: its product of sums divided by its product of differences.
 *
 * For the points x it gives e_k / c_k, for the points y it gives f_k / d_k:
 * m inverses in the field and O(m^2) other operations.
 *
 * @param points Distinct points of one list.
 * @param otherPoints Points of the other list.
 * @param field Field used for the arithmetic.
 * @return One factor for each point.
 */
[[nodiscard]] std::vector<long> computeFactors(const std::vector<long> &points, const std::vector<long> &otherPoints,
                                               const Field &field) {

    const int size{static_cast<int>(points.size())};
    std::vector<long> factors{};

    factors.reserve(points.size());

    for (int k = 0; k < size; k++)
        factors.push_back(field.multiply(multiplySums(points[k], otherPoints, field),
                                         field.computeInverse(multiplyDifferences(points, k, field))));

    return (factors);
}

} //namespace

//The field is a const reference and the base copies it: moving it here could empty it before computeValues reads
//it, since the order in which the two arguments are evaluated is unspecified.
CauchyMatrix::CauchyMatrix(int row, int column, const std::shared_ptr<const Field> &field)
    : CodingMatrix(CauchyMatrix::computeValues(row, column, field), field) {}

MatrixData CauchyMatrix::computeValues(int row, int column, const std::shared_ptr<const Field> &field) {

    CodingMatrix::checkParameters(row, column, field);

    //The points x_i = i and -y_j = n + j are the integers 0 ... n + m - 1: they must be distinct elements.
    if (row + column > field->getSize())
        throw std::invalid_argument(
            "The Cauchy matrix needs n + m distinct elements of the field: n + m cannot exceed " +
            std::to_string(field->getSize()));

    MatrixData values(row, std::vector<long>(column, 0));

    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            values[i][j] = field->computeInverse(field->add(i, computeColumnPoint(row, j, *field)));

    return (values);
}

//Rabin prints |A(j, i)| = |A| / (c_j · d_i · e_j · f_i) · (x_j + y_j), and (y_k - x_j) inside d_k: both are
//misprints. The Cauchy determinant (4) gives |A(j, i)| = |A| · e_j · f_i / (c_j · d_i · (x_j + y_i)), hence
//b_ij = (-1)^(i+j) · (e_j / c_j) · (f_i / d_i) · 1 / (x_j + y_i), where 1 / (x_j + y_i) is the value in row j and
//column i of the submatrix.
MatrixData CauchyMatrix::computeInverseOfRows(const std::vector<int> &labels) const {

    const Field &field{*this->getField()};
    const int size{static_cast<int>(labels.size())};
    const std::vector<long> rowPoints{computeRowPoints(labels)};
    const std::vector<long> columnPoints{computeColumnPoints(this->getRow(), size, field)};
    const std::vector<long> rowFactors{computeFactors(rowPoints, columnPoints, field)};
    const std::vector<long> columnFactors{computeFactors(columnPoints, rowPoints, field)};
    const MatrixData rows{this->selectRows(labels)};
    MatrixData inverse(size, std::vector<long>(size, 0));

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++) {
            const long value{field.multiply(field.multiply(rowFactors[j], columnFactors[i]), rows[j][i])};

            if ((i + j) % 2 == 0)
                inverse[i][j] = value;
            else
                inverse[i][j] = field.subtract(0, value);
        }

    return (inverse);
}

} //namespace ida
