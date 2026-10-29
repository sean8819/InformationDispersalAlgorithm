/**
 * @file Console.h
 * @brief Interactive console: commands that change the parameters, and sentences to encode and decode.
 * @author Sean8819
 * @date 29/10/2026
 */

#pragma once

#include <iosfwd>
#include <string>

#include "Console/Settings.h"

namespace ida {

/**
 * @brief Runs the lines typed in the console of the IDA.
 *
 * A line is a command when it is help or exit, or when its first word is
 * set (set m=3); any other line is a sentence, which is encoded into n
 * fragments and rebuilt from the m fragments chosen. The characters of a
 * sentence must have a code from 0 to 255.
 *
 * @invariant The settings hold only well-formed values; whether n, m, the field and the fragments fit together
 *            is checked when a sentence is run.
 */
class Console final {

private:

    Settings settings{}; ///< Current parameters.

    /**
     * @brief Changes a parameter.
     * @param assignment The text after set, such as "m=3"; spaces are ignored.
     * @param output Stream that receives a note when the fragments are reset.
     * @throws std::invalid_argument If @p assignment is not parameter=value, the parameter is unknown or the value
     *         is not well-formed; the parameters are left as they were.
     */
    void setParameter(const std::string &assignment, std::ostream &output);

    /**
     * @brief Stores the value of a parameter.
     * @param name Name of the parameter.
     * @param value Value, without spaces.
     * @throws std::invalid_argument If the parameter is unknown or the value is not well-formed.
     */
    void assignParameter(const std::string &name, const std::string &value);

    /**
     * @brief Resets the fragments to the first m when they no longer fit n and m, and says so.
     * @param output Stream that receives the note.
     */
    void resetInvalidFragmentChoice(std::ostream &output);

    /**
     * @brief Tells whether the fragments fit n and m.
     * @return True if there are m fragments, each an index from 0 to n - 1.
     */
    [[nodiscard]] bool isFragmentChoiceValid() const;

    /**
     * @brief Encodes a sentence into n fragments and rebuilds it from the chosen ones.
     * @param sentence Sentence in UTF-8.
     * @return The text to show: the sentence, the matrices and the rebuilt sentence.
     * @throws std::invalid_argument If a character has a code above 255, or n, m, the field and the fragments do not
     *         fit together.
     * @throws std::out_of_range If a fragment is not an index from 0 to n - 1.
     */
    [[nodiscard]] std::string encodeAndDecode(const std::string &sentence) const;

public:

    /**
     * @brief Builds the help: the commands and every parameter with its current value.
     * @return The text of the help.
     */
    [[nodiscard]] std::string formatHelp() const;

    /**
     * @brief Runs a command or a sentence.
     * @param line Line typed by the user: spaces at both ends of a command are ignored, a sentence is encoded as it
     *        is, and a line of spaces does nothing.
     * @param output Stream that receives what the line prints.
     * @throws std::invalid_argument If the command or the sentence is not valid.
     * @throws std::out_of_range If a fragment is not an index from 0 to n - 1.
     */
    void execute(const std::string &line, std::ostream &output);

    /**
     * @brief Tells whether a line closes the console.
     * @param line Line typed by the user.
     * @return True if @p line is exit, with or without spaces around it.
     */
    [[nodiscard]] static bool isExitCommand(const std::string &line);
};

} //namespace ida
