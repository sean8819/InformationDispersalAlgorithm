/**
 * @file Latin1ConverterTest.cpp
 * @brief Checks the conversion between UTF-8 text and one code from 0 to 255 for each character.
 * @author Sean8819
 * @date 29/10/2026
 */

#include <optional>
#include <stdexcept>
#include <string>

#include "Console/Latin1Converter.h"
#include "TestUtil.h"

namespace {

/**
 * @brief Converts a UTF-8 text and returns the message of the exception it throws.
 * @param utf8Text Text to convert.
 * @return The message of the std::invalid_argument thrown, or no value if the text was accepted.
 */
[[nodiscard]] std::optional<std::string> findConversionError(const std::string &utf8Text) {

    try {
        static_cast<void>(ida::Latin1Converter::fromUtf8(utf8Text));
    }
    catch (const std::invalid_argument &error) {
        return (error.what());
    }

    return (std::nullopt);
}

/**
 * @brief Checks that ASCII text keeps its bytes in both directions.
 */
void testAsciiIsUnchanged() {

    const std::string text{"Hello, world! 0123456789"};

    check(ida::Latin1Converter::fromUtf8(text) == text, "fromUtf8 keeps ASCII text");
    check(ida::Latin1Converter::toUtf8(text) == text, "toUtf8 keeps ASCII text");
}

/**
 * @brief Checks that an accented letter, two bytes in UTF-8, becomes its single code.
 */
void testAccentedLettersBecomeOneCode() {

    //"è" is U+00E8, written in UTF-8 as the bytes 0xC3 0xA8; "ÿ" is U+00FF, the largest accepted code.
    check(ida::Latin1Converter::fromUtf8("\xC3\xA8") == "\xE8", "\"è\" becomes the code 232");
    check(ida::Latin1Converter::fromUtf8("\xC3\xBF") == "\xFF", "\"ÿ\" becomes the code 255");
    check(ida::Latin1Converter::toUtf8("\xE8") == "\xC3\xA8", "the code 232 becomes \"è\" in UTF-8");
    check(ida::Latin1Converter::fromUtf8("perch\xC3\xA8 s\xC3\xAC") == "perch\xE8 s\xEC",
          "a sentence with accents has one code per character");
}

/**
 * @brief Checks that every code from 1 to 255 survives the conversion to UTF-8 and back.
 */
void testEveryCodeRoundTrips() {

    std::string allCodes{};

    for (int code = 1; code < 256; code++)
        allCodes += static_cast<char>(code);

    check(ida::Latin1Converter::fromUtf8(ida::Latin1Converter::toUtf8(allCodes)) == allCodes,
          "every code from 1 to 255 comes back after toUtf8 and fromUtf8");
}

/**
 * @brief Checks that a character with a code above 255 is rejected with its position.
 */
void testCharacterAbove255IsRejected() {

    //"€" is U+20AC (8364), written in UTF-8 as 0xE2 0x82 0xAC; "ā" is U+0101 (257), two bytes.
    check(findConversionError("abcde\xE2\x82\xAC") ==
              "The character \"\xE2\x82\xAC\" at position 6 has the code 8364, outside the range 0-255",
          "\"€\" is rejected with its position and code");
    check(findConversionError("\xC4\x81") ==
              "The character \"\xC4\x81\" at position 1 has the code 257, outside the range 0-255",
          "\"ā\" is rejected: two bytes but a code above 255");
    check(findConversionError("\xF0\x9F\x98\x80").has_value(), "an emoji is rejected");
}

/**
 * @brief Checks that byte sequences that are not UTF-8 are rejected.
 */
void testInvalidUtf8IsRejected() {

    check(findConversionError("ab\xA8") == "The text is not valid UTF-8 at byte 3",
          "a continuation byte without its first byte is rejected");
    check(findConversionError("ab\xC3") == "The text is not valid UTF-8 at byte 3",
          "a truncated character is rejected");
    check(findConversionError("\xC3z").has_value(), "a first byte followed by a non-continuation byte is rejected");
    check(findConversionError("\xC0\xAF").has_value(), "an overlong encoding of \"/\" is rejected");
    check(findConversionError("\xE0\x80\x80").has_value(), "an overlong encoding of the code 0 is rejected");
    check(findConversionError("\xFF").has_value(), "the byte 0xFF, never used by UTF-8, is rejected");
}

} //namespace

/**
 * @brief Runs the Latin1Converter tests.
 * @return EXIT_SUCCESS if every check passes, EXIT_FAILURE otherwise.
 */
int main() {

    testAsciiIsUnchanged();
    testAccentedLettersBecomeOneCode();
    testEveryCodeRoundTrips();
    testCharacterAbove255IsRejected();
    testInvalidUtf8IsRejected();

    return (report("All Latin1Converter tests passed"));
}
