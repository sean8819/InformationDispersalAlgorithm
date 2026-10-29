/**
 * @file RoundtripTest.cpp
 * @brief Encodes and decodes several texts with both fields and every choice of fragments.
 * @author Sean8819
 * @date 26/10/2026
 */

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "Field/Field.h"
#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"
#include "Ida/IDA.h"
#include "Matrix/CodingMatrix.h"
#include "Matrix/EncodedMatrix.h"
#include "Matrix/InverseCodingMatrix.h"
#include "Matrix/Matrix.h"
#include "Matrix/TextMatrix.h"
#include "TestUtil.h"

namespace {

//The matrices are moved into the objects returned by encode() and selectFragments().
static_assert(std::is_nothrow_move_constructible_v<ida::EncodedMatrix>,
              "EncodedMatrix must be movable without exceptions");
static_assert(std::is_nothrow_move_constructible_v<ida::InverseCodingMatrix>,
              "InverseCodingMatrix must be movable without exceptions");
static_assert(!std::is_copy_constructible_v<ida::Matrix>, "A Matrix cannot be copied through the base class");

/**
 * @brief A field to test, with the name printed on failure.
 */
struct FieldUnderTest {
    std::shared_ptr<const ida::Field> field; ///< Field used for the arithmetic.
    std::string name;                        ///< Name of the field, printed on failure.
};

/**
 * @brief Number of fragments generated and needed.
 */
struct FragmentCounts {
    int n{0}; ///< Number of fragments generated.
    int m{0}; ///< Number of fragments needed for reconstruction.
};

/**
 * @brief Lists every choice of m fragments out of n.
 * @param counts Number of fragments generated and needed.
 * @return Every set of m labels, each in increasing order.
 */
[[nodiscard]] std::vector<std::vector<int>> computeAllLabelChoices(FragmentCounts counts) {

    //The value selected[i] tells whether fragment i is chosen: every permutation of the selection is one choice.
    std::vector<bool> selected(static_cast<std::size_t>(counts.n), false);
    std::fill(selected.begin(), selected.begin() + counts.m, true);

    std::vector<std::vector<int>> choices{};

    //The selection starts as its largest permutation (the true values first), so prev_permutation visits them all.
    do {
        std::vector<int> labels{};

        for (int i = 0; i < counts.n; i++)
            if (selected[i])
                labels.push_back(i);

        choices.push_back(labels);
    } while (std::prev_permutation(selected.begin(), selected.end()));

    return (choices);
}

/**
 * @brief Checks that every value of an encoded matrix is an element of the field.
 * @param C Encoded matrix.
 * @param field Field used for the arithmetic.
 * @param description Description of the test case, printed on failure.
 */
void checkEncodedValues(const ida::EncodedMatrix &C, const ida::Field &field, const std::string &description) {

    for (const std::vector<long> &row : C.getValues())
        for (const long value : row)
            check(value >= 0 && value < field.getSize(), description + ": encoded value is a field element");
}

/**
 * @brief Encodes a text and decodes it with every choice of m fragments out of n.
 * @param fieldUnderTest Field used for the arithmetic, with its name.
 * @param counts Number of fragments generated and needed.
 * @param data Text to encode.
 * @return Number of decodings performed.
 */
[[nodiscard]] int checkRoundtrip(const FieldUnderTest &fieldUnderTest, FragmentCounts counts, const std::string &data) {

    const std::string description{fieldUnderTest.name + " n=" + std::to_string(counts.n) +
                                  " m=" + std::to_string(counts.m) + " length=" + std::to_string(data.length())};
    const ida::CodingMatrix A{counts.n, counts.m, fieldUnderTest.field};
    const ida::TextMatrix B{counts.m, data};
    const ida::IDA dispersal{counts.n, counts.m, fieldUnderTest.field};
    const std::unique_ptr<ida::EncodedMatrix> C{dispersal.encode(A, B)};

    checkEncodedValues(*C, *fieldUnderTest.field, description);

    const std::vector<std::vector<int>> labelChoices{computeAllLabelChoices(counts)};

    for (const std::vector<int> &labels : labelChoices) {
        const ida::InverseCodingMatrix inverse{A, labels};

        check(dispersal.decode(inverse, *C) == data, description + ": text is rebuilt");
        check(C->selectFragments(labels)->getPadding() == B.getPadding(), description + ": fragments keep the padding");
    }

    return (static_cast<int>(labelChoices.size()));
}

/**
 * @brief Builds a text with every byte value from 1 to 255.
 * @return The text, 255 characters long.
 */
[[nodiscard]] std::string makeAllBytesText() {

    std::string allBytes{};

    for (int byteValue = 1; byteValue < 256; byteValue++)
        allBytes += static_cast<char>(byteValue);

    return (allBytes);
}

} //namespace

/**
 * @brief Runs the round-trip tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    const std::vector<FieldUnderTest> fields{{std::make_shared<ida::GaloisField256>(), "GF(2^8)"},
                                             {std::make_shared<ida::PrimeField>(257), "GF(257)"}};

    //"abcde" with m = 4 needs 3 padding characters to fill a 4 x 2 matrix.
    const std::vector<std::string> texts{"Il barone Fanfulla da Lodi, condottier di gran rinomanza ...",
                                         "Hello, world!", "abcdefghij", "abcde", makeAllBytesText()};

    //m = 12 needs an inverse in O(m^3) operations: a method with (m-1)! terms would take minutes for each one.
    const std::vector<FragmentCounts> parameters{{3, 1}, {4, 2}, {6, 3}, {10, 4}, {12, 5}, {14, 12}};

    int decodings{0};

    for (const FieldUnderTest &fieldUnderTest : fields)
        for (const std::string &text : texts)
            for (const FragmentCounts counts : parameters)
                decodings += checkRoundtrip(fieldUnderTest, counts, text);

    return (report("All round-trip tests passed (" + std::to_string(decodings) + " decodings)"));
}
