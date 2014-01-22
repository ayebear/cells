// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "stringutils.h"
#include <fstream>

void StringUtils::trimWhiteSpace(std::string& str)
{
    while (!str.empty() && isWhitespace(str.back())) // Loop until right side contains no whitespace
        str.pop_back(); // Remove last character
    while (!str.empty() && isWhitespace(str.front())) // Loop until left side contains no whitespace
        str.erase(str.begin()); // Remove first character
}

bool StringUtils::trimQuotes(std::string& str)
{
    if (str.size() >= 2) // If there are at least 2 characters in the string
    {
        if (areQuotes(str.front(), str.back())) // If the outsides of the string have quotes
        {
            // Remove the quotes
            str.pop_back();
            str.erase(str.begin());
            return true;
        }
    }
    return false;
}

void StringUtils::stripNewLines(std::string& str)
{
    if (!str.empty())
    {
        for (auto& newLineChar: {cr, lf})
        {
            auto pos = str.find(newLineChar); // Find the character
            while (pos != std::string::npos) // Continue removing the characters until there are none left
            {
                str.erase(pos, 1); // Remove the character
                pos = str.find(newLineChar); // Find another character to remove
            }
        }
    }
}

bool StringUtils::isMultiEndComment(const std::string& str)
{
    return (str.find("*/") != std::string::npos);
}

int StringUtils::isComment(const std::string& str, bool checkMultiEnd)
{
    // Comment symbols and types
    const std::vector<std::string> commentSymbols = {"/*", "//", "#", "::", ";"};
    const std::vector<int> commentTypes = {MultiStart, Single, Single, Single, Single};

    int commentType = NoComment;

    // This is optional so the function returns the correct result
    if (checkMultiEnd && isMultiEndComment(str))
        commentType = MultiEnd;

    // Find out which comment type the string is, if any
    for (unsigned int i = 0; (commentType == NoComment && i < commentSymbols.size()); i++)
    {
        if (str.compare(0, commentSymbols[i].size(), commentSymbols[i]) == 0)
            commentType = commentTypes[i];
    }

    // This check is required for comments using the multi-line symbols on a single line
    if (!checkMultiEnd && commentType == MultiStart && isMultiEndComment(str))
        commentType = Single;

    return commentType;
}

int StringUtils::stripComments(std::string& str, bool checkMultiEnd)
{
    int commentType = isComment(str, checkMultiEnd);
    if (commentType == Single)
        str.clear();
    return commentType;
}

bool StringUtils::isWhitespace(char c)
{
    // Just test if the char is a space or a tab
    return (c == ' ' || c == '\t');
}

bool StringUtils::areQuotes(char c1, char c2)
{
    // Both characters must be the same
    // Both of them must be either single quotes or double quotes
    // Only need to compare one char instead of both, because they must be equal due to the first check
    return ((c1 == c2) && (c1 == '"' || c1 == '\''));
}

bool StringUtils::strToBool(const std::string& str)
{
    // The string should only be true if it is "true"
    return (toLower(str) == "true");
}

bool StringUtils::mustEndWith(std::string& str, const std::string& endStr)
{
    bool endsWith = true;
    // If the original string is smaller than what it should end with
    // Or if the original string does not end with the ending string
    if (str.size() < endStr.size() || !str.compare(str.size() - endStr.size(), endStr.size(), endStr))
    {
        str += endStr; // Then we know it doesn't already end with it!
        endsWith = false;
    }
    return endsWith;
}

unsigned StringUtils::replaceAll(std::string& str, const std::string& findStr, const std::string& replaceStr)
{
    unsigned count = 0;
    size_t pos = 0;
    // Keep searching for the string to find
    while ((pos = str.find(findStr, pos)) != std::string::npos)
    {
        // Replace the found string with the replace string
        str.replace(pos, findStr.length(), replaceStr);
        pos += replaceStr.size();
        ++count;
    }
    return count; // Return the number of occurrences that were replaced
}

void StringUtils::split(const std::string& inStr, const std::string& delim, std::vector<std::string>& outVec, bool allowEmpty)
{
    size_t start = 0;
    size_t end = 0;
    // Keep searching for the delimiters to split
    while ((end = inStr.find(delim, start)) != std::string::npos)
    {
        if (allowEmpty || start != end) // Always add the string if empty strings are allowed, otherwise make sure the string being added is not empty
            outVec.push_back(inStr.substr(start, end - start)); // Add the sub string between the delimiters to the vector
        start = end + delim.size();
    }
    // Get the last part of the string
    if (start < inStr.size())
        outVec.push_back(inStr.substr(start, inStr.size()));
}

void StringUtils::getLinesFromString(std::string inStr, std::vector<std::string>& lines, bool allowEmpty)
{
    // First, search and replace all CRLF with LF, and then CR with LF
    while (replaceAll(inStr, "\r\n", "\n")); // This needs to be a while loop in case there is something like "\r\r\n"
    replaceAll(inStr, "\r", "\n");
    // Then, split the string on the LF characters into the vector
    split(inStr, "\n", lines, allowEmpty);
}

bool StringUtils::readLinesFromFile(const std::string& filename, std::vector<std::string>& lines, bool allowEmpty)
{
    bool status = false;
    std::ifstream file(filename, std::ifstream::in); // Open the file
    if (file.is_open())
    {
        std::string line;
        while (getline(file, line)) // Read a line
        {
            stripNewLines(line); // Make sure to strip any leftover new line characters
            if (allowEmpty || !line.empty()) // If the line is not empty
                lines.push_back(line); // Store the line
        }
        status = true;
    }
    return status;
}

bool StringUtils::writeStringToFile(const std::string& filename, const std::string& data)
{
    bool status = false;
    if (!filename.empty())
    {
        std::ofstream outFile(filename, std::ofstream::out | std::ofstream::trunc);
        if (outFile.is_open())
        {
            outFile << data; // Write the string to the file
            outFile.close();
            status = true;
        }
    }
    return status;
}

std::string StringUtils::toLower(std::string str)
{
    // Make all of the characters lowercase
    for (char& c: str)
        c = tolower(c);
    return str;
}

// TODO: Finish these functions so that arrays can be used in config files

std::vector<std::string> StringUtils::splitArrayString(std::string str)
{
    // There must be quotes in the array to determine what the elements are
    // Example format: {"1", "2", "test"}
    std::vector<std::string> vec;
    if (str.size() >= 2 && str.front() == '{' && str.back() == '}')
    {
        str.pop_back();
        str.erase(str.begin());
    }
    split(str, ",", vec);
    for (auto& elem: vec)
    {
        trimWhiteSpace(elem);
        trimQuotes(elem);
    }
    return vec;
}

std::string joinArrayString(const std::vector<std::string>& vec)
{
    // Join a vector back together into a properly formatted string
    std::string str = "{";
    for (auto& elem: vec)
        str += elem;
    str += "}";
    return str;
}
