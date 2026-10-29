/**
 * @file Latin1Converter.h
 * @brief Conversion between UTF-8 text and one code from 0 to 255 for each character.
 * @author Sean8819
 * @date 29/10/2026
 */

#pragma once

#include <string>

namespace ida {

/**
 * @brief Converts the text typed in the console, in UTF-8, into one byte for each character, and back.
 *
 * The byte of a character is its Unicode code, so only the characters with
 * a code from 0 to 255 (ASCII and the Latin-1 letters, such as "è") can be
 * converted: the IDA then encodes one value for each character.
 */
class Latin1Converter final {

private:

    static constexpr long maximumCode{255}; ///< Largest code that fits in one byte.

public:

    /**
     * @brief Converts a UTF-8 text into one byte for each character.
     * @param utf8Text Text in UTF-8.
     * @return The text with one byte for each character: the code of the character.
     * @throws std::invalid_argument If @p utf8Text is not valid UTF-8, or a character has a code above 255.
     */
    [[nodiscard]] static std::string fromUtf8(const std::string &utf8Text);

    /**
     * @brief Converts a text with one byte for each character into UTF-8.
     * @param latin1Text Text whose bytes are the codes of the characters.
     * @return The same text in UTF-8.
     */
    [[nodiscard]] static std::string toUtf8(const std::string &latin1Text);
};

} //namespace ida
