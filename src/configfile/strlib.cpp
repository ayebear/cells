// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "strlib.h"
#include <fstream>
#include <algorithm>
#include <cctype>

namespace strlib
{

void trimWhitespace(string& str)
{
    while (!str.empty() && std::isspace(str.back())) // Loop until right side contains no whitespace
        str.pop_back(); // Remove last character
    while (!str.empty() && std::isspace(str.front())) // Loop until left side contains no whitespace
        str.erase(str.begin()); // Remove first character
}

bool trimQuotes(string& str)
{
    bool status = false;
    if (str.size() >= 2) // If there are at least 2 characters in the string
    {
        if (areQuotes(str.front(), str.back())) // If the outsides of the string have quotes
        {
            // Remove the quotes
            str.pop_back();
            str.erase(str.begin());
            status = true;
        }
    }
    return status;
}

void stripNewLines(string& str)
{
    if (!str.empty())
    {
        for (char newLineChar: {'\r', '\n'})
            str.erase(std::remove(str.begin(), str.end(), newLineChar), str.end());
    }
}

unsigned replaceAll(string& str, const string& findStr, const string& replaceStr)
{
    unsigned count = 0;
    size_t pos = 0;
    // Keep searching for the string to find
    while ((pos = str.find(findStr, pos)) != string::npos)
    {
        // Replace the found string with the replace string
        str.replace(pos, findStr.length(), replaceStr);
        pos += replaceStr.size();
        ++count;
    }
    return count; // Return the number of occurrences that were replaced
}

void split(const string& inStr, const string& delim, std::vector<string>& outVec, bool allowEmpty)
{
    size_t start = 0;
    size_t end = 0;
    // Keep searching for the delimiters to split
    while ((end = inStr.find(delim, start)) != string::npos)
    {
        if (allowEmpty || start != end) // Always add the string if empty strings are allowed, otherwise make sure the string being added is not empty
            outVec.push_back(inStr.substr(start, end - start)); // Add the sub string between the delimiters to the vector
        start = end + delim.size();
    }
    // Get the last part of the string
    if (start < inStr.size())
        outVec.push_back(inStr.substr(start, inStr.size()));
}

string toLower(string str)
{
    // Make all of the characters lowercase
    for (char& c: str)
        c = tolower(c);
    return str;
}

bool areQuotes(char c1, char c2)
{
    // Both characters must be the same
    // Both of them must be either single quotes or double quotes
    // Only need to compare one char instead of both, because they must be equal due to the first check
    return ((c1 == c2) && (c1 == '"' || c1 == '\''));
}

bool mustEndWith(string& str, const string& endStr)
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

void getLinesFromString(string inStr, std::vector<string>& lines, bool allowEmpty)
{
    // First, search and replace all CRLF with LF, and then CR with LF
    while (replaceAll(inStr, "\r\n", "\n")); // This needs to be a while loop in case there is something like "\r\r\n"
    replaceAll(inStr, "\r", "\n");
    // Then, split the string on the LF characters into the vector
    split(inStr, "\n", lines, allowEmpty);
}

bool readLinesFromFile(const string& filename, std::vector<string>& lines, bool allowEmpty)
{
    bool status = false;
    std::ifstream file(filename, std::ifstream::in); // Open the file
    if (file.is_open())
    {
        string line;
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

bool writeStringToFile(const string& filename, const string& data)
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

bool strToBool(const string& str)
{
    // The string should only be true if it is "true"
    return (toLower(str) == "true");
}

bool isBool(const string& str)
{
    // The string is only a boolean if it is either "true" or "false"
    string lowerStr = toLower(str);
    return (lowerStr == "true" || lowerStr == "false");
}

}
