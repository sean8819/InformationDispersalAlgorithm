/**
 * @file Latin1Converter.cpp
 * @brief Implementation of the Latin1Converter class.
 * @author Sean8819
 * @date 29/10/2026
 */

#include "Console/Latin1Converter.h"

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>

namespace ida {

namespace {

constexpr int highestBit{0x80};                 ///< Highest bit of a byte.
constexpr std::size_t maximumSequenceLength{4}; ///< UTF-8 writes a character in at most 4 bytes.
constexpr int markerMask{0xC0};                 ///< Selects the two highest bits of a byte.
constexpr int continuationMarker{0x80};         ///< A continuation byte is 10xxxxxx: its two highest bits are 10.
constexpr int twoByteMarker{0xC0};              ///< The first byte of a two-byte character is 110xxxxx.
constexpr int payloadBits{6};                   ///< Number of bits of the code carried by a continuation byte.
constexpr int payloadMask{0x3F};                ///< Selects the bits of the code carried by a continuation byte.
constexpr int sevenBitsMask{0x7F};              ///< Selects the 7 lowest bits of a byte.
constexpr int asciiLimit{0x80};                 ///< Codes below it are written in UTF-8 as one byte: the code itself.

/**
 * @brief Smallest code written with 1, 2, 3 and 4 bytes.
 *
 * A smaller code written with more bytes is an overlong encoding, which
 * UTF-8 does not allow: every character has exactly one way to be written.
 */
constexpr std::array<long, maximumSequenceLength> minimumCodes{0x0, 0x80, 0x800, 0x10000};

/**
 * @brief Builds the exception for a text that is not valid UTF-8.
 * @param position Index of the first wrong byte.
 * @return The exception, whose message counts the bytes from 1.
 */
[[nodiscard]] std::invalid_argument makeInvalidUtf8Error(std::size_t position) {
    return (std::invalid_argument("The text is not valid UTF-8 at byte " + std::to_string(position + 1)));
}

/**
 * @brief Computes how many bytes the character starting at a byte takes.
 *
 * The first byte of a character starts with a 0 bit for a one-byte
 * character, otherwise with as many 1 bits as the bytes of the character
 * (110xxxxx: two bytes). A single 1 bit marks a continuation byte.
 *
 * @param utf8Text Text in UTF-8.
 * @param position Index of the first byte of the character.
 * @return The number of bytes of the character, from 1 to 4.
 * @throws std::invalid_argument If the byte cannot start a character.
 */
[[nodiscard]] std::size_t computeSequenceLength(const std::string &utf8Text, std::size_t position) {

    const int firstByte{static_cast<unsigned char>(utf8Text[position])};
    std::size_t leadingOnes{0};

    for (int bit = highestBit; (firstByte & bit) != 0; bit >>= 1)
        leadingOnes++;

    if (leadingOnes == 1 || leadingOnes > maximumSequenceLength)
        throw makeInvalidUtf8Error(position);

    if (leadingOnes == 0)
        return (1);

    return (leadingOnes);
}

/**
 * @brief Decodes the code of a character from its bytes.
 * @param utf8Text Text in UTF-8.
 * @param position Index of the first byte of the character.
 * @param length Number of bytes of the character, from 1 to 4.
 * @return The Unicode code of the character.
 * @throws std::invalid_argument If a byte is missing or is not a continuation byte, or the encoding is overlong.
 */
[[nodiscard]] long decodeCode(const std::string &utf8Text, std::size_t position, std::size_t length) {

    if (position + length > utf8Text.size())
        throw makeInvalidUtf8Error(position);

    //The first byte carries the bits after its leading 1s; the 0 bit that ends them adds nothing to the code.
    long code{static_cast<unsigned char>(utf8Text[position]) & (sevenBitsMask >> (length - 1))};

    for (std::size_t i = 1; i < length; i++) {
        const int nextByte{static_cast<unsigned char>(utf8Text[position + i])};

        if ((nextByte & markerMask) != continuationMarker)
            throw makeInvalidUtf8Error(position + i);

        code = (code << payloadBits) | (nextByte & payloadMask);
    }

    if (code < minimumCodes[length - 1])
        throw makeInvalidUtf8Error(position);

    return (code);
}

/**
 * @brief Writes a code from 128 to 255 in UTF-8.
 * @param code Code of the character.
 * @return The two bytes 110000xx 10xxxxxx: the two highest bits of @p code, then the other six.
 */
[[nodiscard]] std::string encodeAsTwoBytes(int code) {
    return (std::string{static_cast<char>(twoByteMarker | (code >> payloadBits)),
                        static_cast<char>(continuationMarker | (code & payloadMask))});
}

} //namespace

std::string Latin1Converter::fromUtf8(const std::string &utf8Text) {

    std::string latin1Text{};
    std::size_t position{0};

    while (position < utf8Text.size()) {
        const std::size_t length{computeSequenceLength(utf8Text, position)};
        const long code{decodeCode(utf8Text, position, length)};

        if (code > Latin1Converter::maximumCode)
            throw std::invalid_argument("The character \"" + utf8Text.substr(position, length) + "\" at position " +
                                        std::to_string(latin1Text.size() + 1) + " has the code " +
                                        std::to_string(code) + ", outside the range 0-" +
                                        std::to_string(Latin1Converter::maximumCode));

        latin1Text += static_cast<char>(code);
        position += length;
    }

    return (latin1Text);
}

std::string Latin1Converter::toUtf8(const std::string &latin1Text) {

    std::string utf8Text{};

    for (const char character : latin1Text) {
        const int code{static_cast<unsigned char>(character)};

        if (code < asciiLimit)
            utf8Text += character;
        else
            utf8Text += encodeAsTwoBytes(code);
    }

    return (utf8Text);
}

} //namespace ida
