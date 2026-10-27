/**
 * @file TestUtil.h
 * @brief Helpers shared by the tests: checks and final report.
 * @author Sean8819
 * @date 26/10/2026
 */

#pragma once

#include <cstdlib>
#include <iostream>
#include <string>

/**
 * @brief Number of failed checks.
 *
 * It is an inline variable (C++17): every test program has a single counter
 * even though the variable is defined in a header.
 */
inline int failures{0};

/**
 * @brief Checks a condition and prints a message if it is false.
 * @param condition Condition to check.
 * @param description What must be true, printed on failure.
 */
inline void check(bool condition, const std::string &description) {

    if (!condition) {
        std::cerr << "FAIL: " << description << '\n';
        failures++;
    }
}

/**
 * @brief Prints the result of a test program.
 * @param summary Message printed if every check passed.
 * @return EXIT_SUCCESS if every check passed, EXIT_FAILURE otherwise.
 */
[[nodiscard]] inline int report(const std::string &summary) {

    if (failures == 0) {
        std::cout << summary << '\n';
        return (EXIT_SUCCESS);
    }

    std::cerr << failures << " checks failed" << '\n';
    return (EXIT_FAILURE);
}
