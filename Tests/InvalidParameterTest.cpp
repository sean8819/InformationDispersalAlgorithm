/**
 * @file InvalidParameterTest.cpp
 * @brief Checks that invalid parameters stop the program with an error message.
 *
 * Each case is a separate CTest test: the program receives the name of the
 * case, creates the object with the invalid parameter and must stop. CTest
 * checks that the expected error message is printed.
 *
 * @author Sean8819
 * @date 26/10/2026
 */

#include "Ida/IDA.h"
#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"

/**
 * @brief Creates an object with the invalid parameter named on the command line.
 * @param argc Number of command line arguments.
 * @param argv Command line arguments: argv[1] is the name of the case.
 * @return EXIT_SUCCESS if the invalid parameter was accepted, so CTest reports a failure.
 */
int main(int argc, char **argv) {

    if (argc < 2) {
        std::cerr << "Usage: InvalidParameterTest <case>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string testCase = argv[1];
    auto field = std::make_shared<GaloisField256>();

    if (testCase == "null-field")
        IDA ida(4, 2, nullptr);
    else if (testCase == "m-zero")
        IDA ida(4, 0, field);
    else if (testCase == "m-not-less-than-n")
        IDA ida(4, 4, field);
    else if (testCase == "n-too-large")
        IDA ida(256, 2, field);
    else if (testCase == "small-field")
        IDA ida(4, 2, std::make_shared<PrimeField>(13));
    else if (testCase == "not-prime")
        PrimeField primeField(256);
    else if (testCase == "zero-modulus")
        PrimeField primeField(0);
    else if (testCase == "coding-matrix-no-columns")
        CodingMatrix A(4, 0, field);
    else if (testCase == "coding-matrix-null-field")
        CodingMatrix A(4, 2, nullptr);
    else if (testCase == "text-matrix-no-rows")
        TextMatrix B(0, "hello");
    else {
        std::cerr << "Unknown case: " << testCase << std::endl;
        return EXIT_FAILURE;
    }

    //If the program gets here, the invalid parameter was accepted.
    std::cout << "Invalid parameter accepted" << std::endl;
    return EXIT_SUCCESS;
}
