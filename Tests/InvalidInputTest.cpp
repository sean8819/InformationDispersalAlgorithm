/**
 * @file InvalidInputTest.cpp
 * @brief Checks that invalid inputs are rejected instead of causing memory errors or wrong results.
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Ida/IDA.h"
#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"
#include "TestUtil.h"

/**
 * @brief Runs the invalid input tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    //The classes print every step to standard output: silence it while the tests run.
    std::cout.setstate(std::ios_base::failbit);

    auto field = std::make_shared<GaloisField256>();
    CodingMatrix A(4, 2, field);
    TextMatrix B(2, "abcdef");
    IDA ida(4, 2, field, false);

    std::unique_ptr<EncodedMatrix> C = ida.encode(A, B);

    std::vector<int> labels = {0, 3};
    std::vector<int> outOfRangeLabels = {0, 4};
    std::vector<int> negativeLabels = {-1, 0};

    //Labels outside 0 ... n-1 must be rejected instead of reading outside the matrix.
    check(InverseCodingMatrix::getInverse(A, outOfRangeLabels) == nullptr, "getInverse rejects a label equal to n");
    check(InverseCodingMatrix::getInverse(A, negativeLabels) == nullptr, "getInverse rejects a negative label");
    check(C->getFragments(outOfRangeLabels) == nullptr, "getFragments rejects a label equal to n");
    check(C->getFragments(negativeLabels) == nullptr, "getFragments rejects a negative label");

    //There must be exactly m labels.
    std::vector<int> oneLabel = {0};
    std::vector<int> threeLabels = {0, 1, 3};

    check(InverseCodingMatrix::getInverse(A, oneLabel) == nullptr, "getInverse rejects fewer than m labels");
    check(InverseCodingMatrix::getInverse(A, threeLabels) == nullptr, "getInverse rejects more than m labels");

    std::unique_ptr<InverseCodingMatrix> inverse = InverseCodingMatrix::getInverse(A, labels);

    check(ida.decode(*inverse, *C, oneLabel).empty(), "decode rejects fewer than m labels");
    check(ida.decode(*inverse, *C, threeLabels).empty(), "decode rejects more than m labels");

    //The matrices must use the same field object as the IDA object.
    auto otherField = std::make_shared<PrimeField>(257);
    CodingMatrix otherA(4, 2, otherField);
    std::unique_ptr<InverseCodingMatrix> otherInverse = InverseCodingMatrix::getInverse(otherA, labels);

    check(ida.encode(otherA, B) == nullptr, "encode rejects a coding matrix with a different field");
    check(ida.decode(*otherInverse, *C, labels).empty(), "decode rejects an inverse with a different field");

    //The coding matrix must be n x m.
    CodingMatrix tallA(5, 2, field);

    check(ida.encode(tallA, B) == nullptr, "encode rejects a coding matrix with more than n rows");

    //The labels must be the same ones used to compute the inverse.
    std::vector<int> otherLabels = {1, 2};

    check(ida.decode(*inverse, *C, otherLabels).empty(), "decode rejects labels different from those of the inverse");

    //The inverse must be m x m: a larger one would read outside the fragments matrix.
    CodingMatrix wideA(4, 3, field);
    std::unique_ptr<InverseCodingMatrix> wideInverse = InverseCodingMatrix::getInverse(wideA, threeLabels);

    check(ida.decode(*wideInverse, *C, threeLabels).empty(), "decode rejects an inverse larger than m x m");

    std::cout.clear();

    return report("All invalid input tests passed");
}
