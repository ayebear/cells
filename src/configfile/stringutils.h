// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <string>
#include <sstream>
#include <vector>

namespace StringUtils
{
    void trimWhiteSpace(std::string&); // Trims all whitespace on both sides of the string
    bool trimQuotes(std::string&); // Trims quotes on ends of string, returns true if the string was modified
    void stripNewLines(std::string&); // Removes all new lines/carriage returns from a string
    bool isMultiEndComment(const std::string&); // Returns true if it contains a multiple-line end comment symbol
    int isComment(const std::string&, bool = false); // Returns an enum value of the comment type
    int stripComments(std::string&, bool = false); // Removes all comments from a string
    bool isWhitespace(char); // Returns true if the character is a form of whitespace
    bool areQuotes(char, char); // Returns true if both characters are either single or double quotes
    bool strToBool(const std::string&); // Parses a string to determine its boolean value
    bool mustEndWith(std::string&, const std::string&); // Appends the second string if the first doesn't end with it
    unsigned replaceAll(std::string&, const std::string&, const std::string&); // Replaces all instances of a sub-string with another string, and returns the number of replaces
    void split(const std::string&, const std::string&, std::vector<std::string>&, bool = true); // Splits a string into a vector of strings with a delimeter
    void getLinesFromString(std::string, std::vector<std::string>&, bool = true); // Splits a string into separate lines with CR and/or LF characters
    bool readLinesFromFile(const std::string&, std::vector<std::string>&, bool = true); // Simply reads a file into memory in separate lines
    bool writeStringToFile(const std::string&, const std::string&); // Writes a string to a file, will overwrite an existing file
    std::string toLower(std::string); // Creates an all-lowercase version of the passed in string
    std::vector<std::string> splitArrayString(std::string); // Converts an array string to a vector
    std::string joinArrayString(const std::vector<std::string>&); // Converts a vector to an array string
    template <class T> std::string toString(T data); // Converts most types to strings using a string stream
    template <class T> std::string toString(bool data); // Converts a bool to a string

    const char cr = '\r';
    const char lf = '\n';

    enum CommentType
    {
        NoComment = 0,
        Single,
        MultiStart,
        MultiEnd
    };
}

template <class T>
std::string StringUtils::toString(T data)
{
    std::ostringstream tmp;
    tmp.precision(16);
    tmp << data;
    return tmp.str();
    //return std::to_string(data); // Precision is fixed to 6...
}

template <class T>
std::string StringUtils::toString(bool data)
{
    return (data ? "true" : "false");
}

#endif
