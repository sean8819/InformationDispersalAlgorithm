/**
 * @file Console.cpp
 * @brief Implementation of the Console class.
 * @author Sean8819
 * @date 29/10/2026
 */

#include "Console/Console.h"

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Console/Latin1Converter.h"
#include "Field/Field.h"
#include "Field/GaloisField256.h"
#include "Field/PrimeField.h"
#include "Ida/EncodingStep.h"
#include "Ida/FieldProduct.h"
#include "Ida/IDA.h"
#include "Matrix/CodingMatrix.h"
#include "Matrix/EncodedMatrix.h"
#include "Matrix/InverseCodingMatrix.h"
#include "Matrix/Matrix.h"
#include "Matrix/TextMatrix.h"

namespace ida {

namespace {

const std::string helpCommand{"help"}; ///< Command that shows the help.
const std::string exitCommand{"exit"}; ///< Command that closes the console.
const std::string setCommand{"set"};   ///< First word of the command that changes a parameter.

const std::string nParameter{"n"};                 ///< Name of the number of fragments generated.
const std::string mParameter{"m"};                 ///< Name of the number of fragments needed.
const std::string fieldParameter{"field"};         ///< Name of the field.
const std::string fragmentsParameter{"fragments"}; ///< Name of the fragments used for reconstruction.
const std::string detailParameter{"detail"};       ///< Name of the output detail.

const std::string galoisName{"galois"};       ///< Value of the field parameter for FieldKind::Galois.
const std::string primeName{"prime"};         ///< Value of the field parameter for FieldKind::Prime.
const std::string resultsOnlyName{"results"}; ///< Value of the detail parameter for OutputDetail::ResultsOnly.
const std::string everyStepName{"all"};       ///< Value of the detail parameter for OutputDetail::EveryStep.

constexpr int helpColumnWidth{20}; ///< Width of the column of the help that holds the commands and the parameters.
constexpr int lineWidth{
    80}; ///< Longest row of a matrix, in characters: a wider matrix is split into blocks of columns.

/**
 * @brief Removes the spaces and tabs at both ends of a text.
 * @param text Text to trim.
 * @return @p text without spaces and tabs at its ends.
 */
[[nodiscard]] std::string trim(const std::string &text) {

    const std::size_t first{text.find_first_not_of(" \t")};

    if (first == std::string::npos)
        return ("");

    return (text.substr(first, text.find_last_not_of(" \t") - first + 1));
}

/**
 * @brief Removes every space and tab of a text.
 * @param text Text to compact.
 * @return @p text without spaces and tabs.
 */
[[nodiscard]] std::string removeSpaces(const std::string &text) {

    std::string compactText{};

    for (const char character : text)
        if (character != ' ' && character != '\t')
            compactText += character;

    return (compactText);
}

/**
 * @brief Tells whether a line is a set command.
 * @param line Line typed by the user.
 * @return True if the first word of @p line is set.
 */
[[nodiscard]] bool isSetCommand(const std::string &line) {

    std::istringstream words{line};
    std::string firstWord{};

    words >> firstWord;

    return (firstWord == setCommand);
}

/**
 * @brief Reads an integer.
 * @param value Text of the integer.
 * @return The integer.
 * @throws std::invalid_argument If @p value is not an integer, also when it has other characters after it.
 */
[[nodiscard]] int parseInteger(const std::string &value) {

    std::istringstream stream{value};
    int number{0};

    stream >> number;

    if (stream.fail() || !stream.eof())
        throw std::invalid_argument("\"" + value + "\" is not an integer");

    return (number);
}

/**
 * @brief Reads a list of fragment indices separated by commas.
 * @param value Text of the list, such as "0,3".
 * @return The indices.
 * @throws std::invalid_argument If an index is not an integer.
 */
[[nodiscard]] std::vector<int> parseLabels(const std::string &value) {

    std::istringstream stream{value};
    std::string label{};
    std::vector<int> labels{};

    while (std::getline(stream, label, ','))
        labels.push_back(parseInteger(label));

    return (labels);
}

/**
 * @brief Reads the name of a field.
 * @param value galois or prime.
 * @return The kind of field.
 * @throws std::invalid_argument If @p value is not the name of a field.
 */
[[nodiscard]] FieldKind parseFieldKind(const std::string &value) {

    if (value == galoisName)
        return (FieldKind::Galois);

    if (value == primeName)
        return (FieldKind::Prime);

    throw std::invalid_argument("The field must be " + galoisName + " or " + primeName);
}

/**
 * @brief Reads the name of an output detail.
 * @param value results or all.
 * @return The output detail.
 * @throws std::invalid_argument If @p value is not the name of an output detail.
 */
[[nodiscard]] OutputDetail parseOutputDetail(const std::string &value) {

    if (value == resultsOnlyName)
        return (OutputDetail::ResultsOnly);

    if (value == everyStepName)
        return (OutputDetail::EveryStep);

    throw std::invalid_argument("The detail must be " + resultsOnlyName + " or " + everyStepName);
}

/**
 * @brief Gives the name of a field, as the set command reads it.
 * @param fieldKind Kind of field.
 * @return galois or prime.
 */
[[nodiscard]] std::string formatFieldKind(FieldKind fieldKind) {

    if (fieldKind == FieldKind::Galois)
        return (galoisName);

    return (primeName);
}

/**
 * @brief Gives the name of an output detail, as the set command reads it.
 * @param outputDetail Output detail.
 * @return results or all.
 */
[[nodiscard]] std::string formatOutputDetail(OutputDetail outputDetail) {

    if (outputDetail == OutputDetail::ResultsOnly)
        return (resultsOnlyName);

    return (everyStepName);
}

/**
 * @brief Writes fragment indices as the set command reads them.
 * @param labels Fragment indices.
 * @return The indices separated by commas, such as "0,3".
 */
[[nodiscard]] std::string formatLabels(const std::vector<int> &labels) {

    std::string text{};

    for (const int label : labels) {
        if (!text.empty())
            text += ",";

        text += std::to_string(label);
    }

    return (text);
}

/**
 * @brief Writes one line of the help: a command or a parameter, and what it does.
 * @param entry Command or parameter with its value.
 * @param description What the command or the parameter does.
 * @return The line, with the descriptions aligned in a column.
 */
[[nodiscard]] std::string formatHelpLine(const std::string &entry, const std::string &description) {

    std::ostringstream line{};

    line << "  " << std::left << std::setw(helpColumnWidth) << entry << description << '\n';

    return (line.str());
}

/**
 * @brief Creates the field used for the arithmetic.
 * @param fieldKind Kind of field.
 * @param p Prime modulus, used only by FieldKind::Prime.
 * @return The new field.
 * @throws std::invalid_argument If the field is FieldKind::Prime and @p p is not a prime number.
 */
[[nodiscard]] std::shared_ptr<const Field> createField(FieldKind fieldKind, int p) {

    if (fieldKind == FieldKind::Galois)
        return (std::make_shared<GaloisField256>());

    return (std::make_shared<PrimeField>(p));
}

/**
 * @brief Computes the number of characters of the widest value of a matrix.
 * @param values Values of the matrix.
 * @return The number of characters, at least 1.
 */
[[nodiscard]] int computeValueWidth(const MatrixData &values) {

    std::size_t width{1};

    for (const std::vector<long> &row : values)
        for (const long value : row)
            width = std::max(width, std::to_string(value).size());

    return (static_cast<int>(width));
}

/**
 * @brief Writes some columns of a matrix, one row per line, each value right-aligned to the widest value of the matrix.
 * @param values Values of the matrix.
 * @param firstColumn Index of the first column to write.
 * @param endColumn Index after the last column to write.
 * @return The rows, with the values separated by one space.
 */
[[nodiscard]] std::string formatColumns(const MatrixData &values, int firstColumn, int endColumn) {

    const int valueWidth{computeValueWidth(values)};
    std::ostringstream text{};

    for (const std::vector<long> &row : values) {
        for (int j = firstColumn; j < endColumn; j++) {
            if (j > firstColumn)
                text << ' ';

            text << std::right << std::setw(valueWidth) << row[j];
        }

        text << '\n';
    }

    return (text.str());
}

/**
 * @brief Writes a matrix with its values aligned in columns, in blocks of columns when it is wider than a line.
 * @param label Header written before the values.
 * @param values Values of the matrix.
 * @return The header and the values.
 */
[[nodiscard]] std::string formatMatrix(const std::string &label, const MatrixData &values) {

    const int columns{values.empty() ? 0 : static_cast<int>(values[0].size())};
    //A row of k values takes k widths and k - 1 spaces: adding one space to the line counts the missing one.
    const int columnsPerBlock{std::max(1, (lineWidth + 1) / (computeValueWidth(values) + 1))};
    std::string text{label + "\n"};

    for (int firstColumn = 0; firstColumn < columns; firstColumn += columnsPerBlock) {
        const int endColumn{std::min(firstColumn + columnsPerBlock, columns)};

        //A blank line keeps each block apart from the rows of the previous one.
        if (firstColumn > 0)
            text += "\n";

        if (columns > columnsPerBlock)
            text += "(columns " + std::to_string(firstColumn + 1) + "-" + std::to_string(endColumn) + " of " +
                    std::to_string(columns) + ")\n";

        text += formatColumns(values, firstColumn, endColumn);
    }

    return (text);
}

/**
 * @brief Writes the codes of the characters of the text, padding included, aligned like a matrix.
 * @param B Text matrix.
 * @return The header and the codes.
 */
[[nodiscard]] std::string formatCharacterCodes(const TextMatrix &B) {

    const std::string &paddedText{B.getPaddedText()};
    std::vector<long> codes{};

    codes.reserve(paddedText.size());

    //char may be signed: going through unsigned char gives the code 0 ... 255 that the text matrix holds.
    for (const char character : paddedText)
        codes.push_back(static_cast<unsigned char>(character));

    return (formatMatrix("Character codes", MatrixData{codes}));
}

/**
 * @brief Writes every product of the encoding and the value of C it gives.
 * @param steps Steps of the encoding, one for each value of the encoded matrix.
 * @return The products and the results.
 */
[[nodiscard]] std::string formatEncodingSteps(const std::vector<EncodingStep> &steps) {

    std::ostringstream text{};

    for (const EncodingStep &step : steps) {
        for (const FieldProduct &product : step.products)
            text << product.leftFactor << " * " << product.rightFactor << " = " << product.value << '\n';

        text << "Result: " << step.result << '\n' << '\n';
    }

    return (text.str());
}

/**
 * @brief Writes the details of the encoding: the character codes, every product and the matrices A and B.
 * @param dispersal Object that performs the encoding.
 * @param A Coding matrix.
 * @param B Text matrix.
 * @return The details of the encoding.
 */
[[nodiscard]] std::string formatEncodingDetails(const IDA &dispersal, const CodingMatrix &A, const TextMatrix &B) {
    return (formatCharacterCodes(B) + "\n--- ENCODING ---\n" +
            formatEncodingSteps(dispersal.computeEncodingSteps(A, B)) +
            formatMatrix("\nCoding matrix:", A.getValues()) + formatMatrix("\nText matrix:", B.getValues()));
}

/**
 * @brief Writes the details of the decoding: the inverse and the fragments it is multiplied by.
 * @param inverse Inverse of the coding submatrix.
 * @param C Encoded matrix.
 * @return The details of the decoding.
 */
[[nodiscard]] std::string formatDecodingDetails(const InverseCodingMatrix &inverse, const EncodedMatrix &C) {

    const std::unique_ptr<EncodedMatrix> fragments{C.selectFragments(inverse.getLabels())};

    return ("\n--- Decoding ---\n" + formatMatrix("\nInverse matrix:", inverse.getValues()) +
            formatMatrix("\nFragment matrix used:", fragments->getValues()));
}

/**
 * @brief Rebuilds the text and writes the decoded matrix and the rebuilt text.
 * @param dispersal Object that performs the decoding.
 * @param inverse Inverse of the coding submatrix.
 * @param C Encoded matrix.
 * @return The decoded matrix and the rebuilt text, in UTF-8.
 */
[[nodiscard]] std::string formatDecoding(const IDA &dispersal, const InverseCodingMatrix &inverse,
                                         const EncodedMatrix &C) {
    return (formatMatrix("\nDecoded matrix:", dispersal.computeDecodedValues(inverse, C)) + "\nRebuild data:\n" +
            Latin1Converter::toUtf8(dispersal.decode(inverse, C)) + "\n");
}

} //namespace

std::string Console::formatHelp() const {

    std::ostringstream help{};

    help << "Information Dispersal Algorithm: a sentence is split into n fragments, and any m of them rebuild it.\n"
         << "\nParameters (" << setCommand << " <parameter>=<value>):\n"
         << formatHelpLine(nParameter + "=" + std::to_string(this->settings.n), "number of fragments generated")
         << formatHelpLine(mParameter + "=" + std::to_string(this->settings.m),
                           "number of fragments needed to rebuild the sentence")
         << formatHelpLine(fieldParameter + "=" + formatFieldKind(this->settings.fieldKind),
                           galoisName + ": GF(2^8); " + primeName + ": integers modulo " +
                               std::to_string(this->settings.p))
         << formatHelpLine(fragmentsParameter + "=" + formatLabels(this->settings.labels),
                           "the m fragments used to rebuild the sentence, from 0 to n-1")
         << formatHelpLine(detailParameter + "=" + formatOutputDetail(this->settings.outputDetail),
                           resultsOnlyName + ": the main matrices; " + everyStepName + ": every intermediate step")
         << "\nCommands:\n"
         << formatHelpLine(helpCommand, "shows this help")
         << formatHelpLine(exitCommand, "closes the console (also Ctrl-D)")
         << "\nAny other line is a sentence to encode and decode: its characters must have a code from 0 to 255.\n";

    return (help.str());
}

void Console::execute(const std::string &line, std::ostream &output) {

    const std::string command{trim(line)};

    if (command.empty())
        return;

    if (command == helpCommand)
        output << this->formatHelp();
    else if (isSetCommand(command))
        this->setParameter(command.substr(setCommand.size()), output);
    else
        output << this->encodeAndDecode(line);
}

void Console::setParameter(const std::string &assignment, std::ostream &output) {

    const std::string compactAssignment{removeSpaces(assignment)};
    const std::size_t equalsSign{compactAssignment.find('=')};

    if (equalsSign == std::string::npos)
        throw std::invalid_argument("Usage: " + setCommand + " <parameter>=<value>");

    const std::string name{compactAssignment.substr(0, equalsSign)};

    this->assignParameter(name, compactAssignment.substr(equalsSign + 1));

    if (name == nParameter || name == mParameter)
        this->resetInvalidFragmentChoice(output);
}

void Console::assignParameter(const std::string &name, const std::string &value) {

    if (name == nParameter)
        this->settings.n = parseInteger(value);
    else if (name == mParameter)
        this->settings.m = parseInteger(value);
    else if (name == fieldParameter)
        this->settings.fieldKind = parseFieldKind(value);
    else if (name == fragmentsParameter)
        this->settings.labels = parseLabels(value);
    else if (name == detailParameter)
        this->settings.outputDetail = parseOutputDetail(value);
    else
        throw std::invalid_argument("Unknown parameter \"" + name + "\": type " + helpCommand +
                                    " to see the parameters");
}

void Console::resetInvalidFragmentChoice(std::ostream &output) {

    if (this->isFragmentChoiceValid())
        return;

    this->settings.labels.clear();

    for (int label = 0; label < this->settings.m; label++)
        this->settings.labels.push_back(label);

    output << "The fragments no longer fit n and m: " << fragmentsParameter << "="
           << formatLabels(this->settings.labels) << '\n';
}

bool Console::isFragmentChoiceValid() const {

    if (static_cast<int>(this->settings.labels.size()) != this->settings.m)
        return (false);

    for (const int label : this->settings.labels)
        if (label < 0 || label >= this->settings.n)
            return (false);

    return (true);
}

std::string Console::encodeAndDecode(const std::string &sentence) const {

    //The IDA encodes one value for each character: its code from 0 to 255, instead of its UTF-8 bytes.
    const std::string codes{Latin1Converter::fromUtf8(sentence)};

    //The field is shared by the coding matrix, its inverse and the IDA object.
    const std::shared_ptr<const Field> field{createField(this->settings.fieldKind, this->settings.p)};
    const CodingMatrix A{this->settings.n, this->settings.m, field};
    const TextMatrix B{this->settings.m, codes};
    const IDA dispersal{this->settings.n, this->settings.m, field};
    const std::unique_ptr<EncodedMatrix> C{dispersal.encode(A, B)};
    const InverseCodingMatrix inverse{A, this->settings.labels};
    const bool showEveryStep{this->settings.outputDetail == OutputDetail::EveryStep};

    std::string text{"\nData: " + sentence + "\n"};

    if (showEveryStep)
        text += formatEncodingDetails(dispersal, A, B);

    text += formatMatrix("\nEncoded matrix:", C->getValues());

    if (showEveryStep)
        text += formatDecodingDetails(inverse, *C);

    return (text + formatDecoding(dispersal, inverse, *C));
}

bool Console::isExitCommand(const std::string &line) {
    return (trim(line) == exitCommand);
}

} //namespace ida
