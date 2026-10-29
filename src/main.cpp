/**
 * @file main.cpp
 * @brief Interactive console of the Information Dispersal Algorithm.
 * @author Sean8819
 * @date 26/10/2026
 */

#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <string>

#include "Console/Console.h"

namespace {

const std::string prompt{"$> "}; ///< Shown before each line the user types.

/**
 * @brief Shows the prompt and reads the next line typed by the user.
 * @return The line, or no value at the end of the input (Ctrl-D).
 */
[[nodiscard]] std::optional<std::string> readLine() {

    //A blank line keeps the prompt apart from the previous output; the prompt has no end of line, so it is
    //flushed to appear before the user types.
    std::cout << '\n' << prompt << std::flush;

    std::string line{};

    if (!std::getline(std::cin, line)) {
        //The end of the input leaves the cursor after the prompt: the console ends on a new line.
        std::cout << '\n';
        return (std::nullopt);
    }

    return (line);
}

/**
 * @brief Shows the help, then runs each line typed by the user until exit or the end of the input.
 */
void runConsole() {

    ida::Console console{};

    std::cout << console.formatHelp();

    for (std::optional<std::string> line{readLine()}; line.has_value() && !ida::Console::isExitCommand(*line);
         line = readLine()) {

        //A wrong command or sentence is shown and the console goes on, so the user can correct it.
        try {
            console.execute(*line, std::cout);
        }
        catch (const std::exception &error) {
            std::cerr << "Error: " << error.what() << '\n';
        }
    }
}

} //namespace

/**
 * @brief Runs the interactive console.
 * @return EXIT_SUCCESS, or EXIT_FAILURE if the console stops because of an unexpected error.
 */
int main() {

    try {
        runConsole();
    }
    catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return (EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
