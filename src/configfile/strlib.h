// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef STRLIB_H
#define STRLIB_H

#include <string>
#include <sstream>
#include <vector>

using std::string;

namespace strlib
{

// String manipulation
void trimWhitespace(string&); // Trims all whitespace on both sides of the string
bool trimQuotes(string&); // Trims quotes on ends of string, returns true if the string was modified
void stripNewLines(string&); // Removes all new lines/carriage returns from a string
unsigned replaceAll(string&, const string&, const string&); // Replaces all instances of a sub-string with another string, and returns the number of replaces
void split(const string&, const string&, std::vector<string>&, bool = true); // Splits a string into a vector of strings with a delimeter
string toLower(string); // Creates an all-lowercase version of the passed in string
bool areQuotes(char, char); // Returns true if both characters are either single or double quotes
bool mustEndWith(string&, const string&); // Appends the second string if the first doesn't end with it

// File operations
void getLinesFromString(string, std::vector<string>&, bool = true); // Splits a string into separate lines with CR and/or LF characters
bool readLinesFromFile(const string&, std::vector<string>&, bool = true); // Simply reads a file into memory in separate lines
bool writeStringToFile(const string&, const string&); // Writes a string to a file, will overwrite an existing file

// String converting
bool strToBool(const string&); // Parses a string to determine its boolean value
bool isBool(const string&); // Determines if a string is either "true" or "false"

// Converts most types to strings using a string stream
template <typename T>
string toString(T data, unsigned precision = 16)
{
    std::ostringstream tmp;
    tmp.precision(precision);
    tmp << data;
    return tmp.str();
}

// Converts a bool to a string
template <typename T>
string toString(bool data)
{
    return (data ? "true" : "false");
}

}

#endif
