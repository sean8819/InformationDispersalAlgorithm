/**
 * @file ConsoleTest.cpp
 * @brief Checks the commands and the sentences run by the console.
 * @author Sean8819
 * @date 29/10/2026
 */

#include <exception>
#include <optional>
#include <sstream>
#include <string>

#include "Console/Console.h"
#include "TestUtil.h"

namespace {

/**
 * @brief Runs a line on a console and returns what it printed.
 * @param console Console that runs the line.
 * @param line Command or sentence.
 * @return The text printed by the console.
 */
[[nodiscard]] std::string execute(ida::Console &console, const std::string &line) {

    std::ostringstream output{};

    console.execute(line, output);

    return (output.str());
}

/**
 * @brief Runs a line on a console and returns the message of the exception it throws.
 * @param console Console that runs the line.
 * @param line Command or sentence.
 * @return The message of the exception thrown, or no value if the line was accepted.
 */
[[nodiscard]] std::optional<std::string> findError(ida::Console &console, const std::string &line) {

    try {
        static_cast<void>(execute(console, line));
    }
    catch (const std::exception &error) {
        return (error.what());
    }

    return (std::nullopt);
}

/**
 * @brief Tells whether a text contains another one.
 * @param text Text to search.
 * @param part Text to find.
 * @return True if @p part appears in @p text.
 */
[[nodiscard]] bool contains(const std::string &text, const std::string &part) {
    return (text.find(part) != std::string::npos);
}

/**
 * @brief Checks that the help shows every parameter with its default value, and that "help" prints it.
 */
void testHelpShowsTheParameters() {

    ida::Console console{};
    const std::string help{console.formatHelp()};

    check(contains(help, "n=4"), "the help shows n=4");
    check(contains(help, "m=2"), "the help shows m=2");
    check(contains(help, "field=galois"), "the help shows field=galois");
    check(contains(help, "fragments=0,3"), "the help shows fragments=0,3");
    check(contains(help, "detail=results"), "the help shows detail=results");
    check(execute(console, "help") == help, "the command help prints the help");
}

/**
 * @brief Checks that a sentence is encoded and rebuilt, also when it has accented letters.
 */
void testSentenceIsRebuilt() {

    ida::Console console{};
    const std::string output{execute(console, "hello!")};

    check(output.rfind("\nData: hello!\n", 0) == 0, "the output starts with the sentence");
    check(contains(output, "\nEncoded matrix:\n"), "the output shows the encoded matrix");
    check(contains(output, "\nDecoded matrix:\n104 108 111\n101 108  33\n"),
          "the output shows the decoded matrix, right-aligned in columns");
    check(contains(output, "\nRebuild data:\nhello!\n"), "the sentence is rebuilt");
    check(contains(execute(console, "perch\xC3\xA8 cos\xC3\xAC?"), "\nRebuild data:\nperch\xC3\xA8 cos\xC3\xAC?\n"),
          "a sentence with accented letters is rebuilt");
}

/**
 * @brief Checks that the detailed output gives one code from 0 to 255 to each character.
 */
void testEveryStepShowsOneCodePerCharacter() {

    ida::Console console{};

    static_cast<void>(execute(console, "set detail=all"));

    //"è" with m = 2 is the code 232 followed by one padding character.
    const std::string output{execute(console, "\xC3\xA8")};

    check(contains(output, "Character codes\n232   0\n"), "\"è\" has the single code 232");
    check(contains(output, "\nInverse matrix:\n"), "the detailed output shows the inverse");
    check(contains(output, "\nRebuild data:\n\xC3\xA8\n"), "\"è\" is rebuilt");
}

/**
 * @brief Checks that a matrix wider than a line is split into blocks of columns, none longer than 80 characters.
 */
void testWideMatricesAreSplitIntoBlocks() {

    ida::Console console{};

    //100 characters with m = 2 give 50 columns; the decoded values are all 97: 27 of them fill 80 characters.
    const std::string output{execute(console, std::string(100, 'a'))};

    check(contains(output, "\nDecoded matrix:\n(columns 1-27 of 50)\n97 97 "), "the first block holds 27 columns");
    check(contains(output, " 97\n\n(columns 28-50 of 50)\n97 97 "),
          "the second block holds the other 23 columns, after a blank line");

    std::istringstream lines{output};
    std::string line{};
    bool everyRowFits{true};

    //The sentence itself may be longer: only the rows of numbers must fit.
    while (std::getline(lines, line))
        if (line.find_first_not_of("0123456789 ") == std::string::npos && line.size() > 80)
            everyRowFits = false;

    check(everyRowFits, "no row of numbers is longer than 80 characters");
}

/**
 * @brief Checks that set changes the parameters, with or without spaces, and resets the fragments that no longer fit.
 */
void testSetChangesTheParameters() {

    ida::Console console{};

    check(execute(console, "set n=6").empty(), "set n=6 prints nothing: fragments 0 and 3 still fit");
    check(contains(execute(console, "set m = 3"), "fragments=0,1,2"), "set m=3 resets the fragments to the first 3");
    check(execute(console, "set fragments=1, 4, 5").empty(), "set fragments accepts spaces after the commas");
    check(execute(console, "set field=prime").empty(), "set field=prime prints nothing");

    const std::string help{console.formatHelp()};

    check(contains(help, "n=6") && contains(help, "m=3"), "the help shows the new n and m");
    check(contains(help, "fragments=1,4,5"), "the help shows the new fragments");
    check(contains(help, "field=prime"), "the help shows the new field");
    check(contains(execute(console, "hello, world"), "\nRebuild data:\nhello, world\n"),
          "the sentence is rebuilt with the new parameters");
}

/**
 * @brief Checks that a line is a set command only when its first word is set.
 */
void testOnlyTheWordSetStartsACommand() {

    ida::Console console{};

    check(contains(execute(console, "settle down"), "\nRebuild data:\nsettle down\n"),
          "\"settle down\" is a sentence, not a command");
    check(execute(console, "").empty(), "an empty line prints nothing");
    check(execute(console, "   ").empty(), "a line of spaces prints nothing");
    check(ida::Console::isExitCommand("exit"), "exit closes the console");
    check(ida::Console::isExitCommand("  exit "), "exit with spaces around closes the console");
    check(!ida::Console::isExitCommand("exit now"), "\"exit now\" is a sentence");
}

/**
 * @brief Checks that a wrong set command is rejected with a message and leaves the parameters as they were.
 */
void testWrongSetCommandsAreRejected() {

    ida::Console console{};

    check(findError(console, "set m") == "Usage: set <parameter>=<value>", "set without = is rejected");
    check(findError(console, "set x=1") == "Unknown parameter \"x\": type help to see the parameters",
          "an unknown parameter is rejected");
    check(findError(console, "set m=abc") == "\"abc\" is not an integer", "a value that is not a number is rejected");
    check(findError(console, "set n=4x") == "\"4x\" is not an integer", "a number followed by letters is rejected");
    check(findError(console, "set field=rational") == "The field must be galois or prime",
          "an unknown field is rejected");
    check(findError(console, "set detail=some") == "The detail must be results or all",
          "an unknown detail is rejected");
    check(findError(console, "set fragments=0,,3") == "\"\" is not an integer", "an empty fragment index is rejected");
    check(contains(console.formatHelp(), "m=2") && contains(console.formatHelp(), "fragments=0,3"),
          "the rejected commands leave the parameters as they were");
}

/**
 * @brief Checks that a sentence is rejected when a character or a parameter is not valid.
 */
void testWrongSentencesAreRejected() {

    ida::Console console{};

    check(findError(console, "costa 5\xE2\x82\xAC") ==
              "The character \"\xE2\x82\xAC\" at position 8 has the code 8364, outside the range 0-255",
          "a sentence with \"€\" is rejected");

    static_cast<void>(execute(console, "set fragments=0,9"));
    check(findError(console, "hello").has_value(), "a fragment index outside 0 ... n-1 is rejected");

    static_cast<void>(execute(console, "set m=4"));
    check(findError(console, "hello").has_value(), "m equal to n is rejected when a sentence is run");
}

} //namespace

/**
 * @brief Runs the Console tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    testHelpShowsTheParameters();
    testSentenceIsRebuilt();
    testEveryStepShowsOneCodePerCharacter();
    testWideMatricesAreSplitIntoBlocks();
    testSetChangesTheParameters();
    testOnlyTheWordSetStartsACommand();
    testWrongSetCommandsAreRejected();
    testWrongSentencesAreRejected();

    return (report("All Console tests passed"));
}
