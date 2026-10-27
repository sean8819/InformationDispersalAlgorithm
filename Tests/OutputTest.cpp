/**
 * @file OutputTest.cpp
 * @brief Checks what the IDA object prints with and without the verbose flag.
 * @author Sean8819
 * @date 26/10/2026
 */

#include <sstream>
#include "Ida/IDA.h"
#include "Field/GaloisField256.h"
#include "TestUtil.h"

/**
 * @brief Encodes and decodes "hello!" and returns everything printed to standard output.
 * @param verbose Value of the verbose flag of the IDA object.
 * @return The text printed to standard output.
 */
std::string runAndCaptureOutput(bool verbose) {

    std::ostringstream output;

    //Redirect standard output to a string while the IDA object works.
    std::streambuf *standardOutput = std::cout.rdbuf(output.rdbuf());

    auto field = std::make_shared<GaloisField256>();
    CodingMatrix A(4, 2, field);
    TextMatrix B(2, "hello!");
    IDA ida(4, 2, field, verbose);

    std::unique_ptr<EncodedMatrix> C = ida.encode(A, B);

    std::vector<int> labels = {0, 3};
    std::unique_ptr<InverseCodingMatrix> inverse = InverseCodingMatrix::getInverse(A, labels);

    ida.decode(*inverse, *C, labels);

    std::cout.rdbuf(standardOutput);

    return output.str();
}

/**
 * @brief Checks whether a text contains a string.
 * @param text Text to search.
 * @param part String to look for.
 * @return true if @p part appears in @p text.
 */
bool contains(const std::string &text, const std::string &part) {
    return (text.find(part) != std::string::npos);
}

/**
 * @brief Runs the output tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    //Steps that are printed only with the verbose flag.
    std::string verboseSteps[] = {"Character codes", "--- ENCODING ---", " * ", "Result:", "Coding matrix:",
                                  "Text matrix:", "--- Decoding ---", "Inverse matrix:", "Fragment matrix used:"};

    //"hello!" in 2 rows, by column: h l o / e l !
    std::string decodedMatrix = "Decoded matrix:\n104 108 111 \n101 108 33 \n";

    std::string quiet = runAndCaptureOutput(false);

    check(contains(quiet, "Encoded matrix:"), "without verbose the encoded matrix is printed");
    check(contains(quiet, decodedMatrix), "without verbose the decoded matrix is printed");

    for (const std::string &step : verboseSteps)
        check(!contains(quiet, step), "without verbose \"" + step + "\" is not printed");

    std::string verbose = runAndCaptureOutput(true);

    check(contains(verbose, "Encoded matrix:"), "with verbose the encoded matrix is printed");
    check(contains(verbose, decodedMatrix), "with verbose the decoded matrix is printed");

    for (const std::string &step : verboseSteps)
        check(contains(verbose, step), "with verbose \"" + step + "\" is printed");

    return report("All output tests passed");
}
