// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "option.h"

Option::Option():
    str(""),
    number(0),
    decimal(0),
    logical(false),
    quotes(false),
    range(NoRange)
{
}

Option::Option(const std::string& data)
{
    quotes = false;
    range = NoRange;
    set<int>(0);
    setString(data);
}

void Option::reset()
{
    range = NoRange;
    set<int>(0);
}

bool Option::setString(const std::string& data)
{
    std::istringstream tmpStream(data);
    double tmpDec = 0;
    bool parsedNumber = (tmpStream >> tmpDec); // Try reading a number from the string
    if (isInRange(tmpDec)) // Check if the number is in range
    {
        decimal = tmpDec; // Set the decimal from the temporary one
        number = decimal; // Truncate to an int
        quotes = !parsedNumber; // No quotes around a number
        logical = (parsedNumber ? (decimal != 0) : StringUtils::strToBool(data)); // Convert to a boolean
        str = data; // Set the string
        return true;
    }
    return false;
}

const std::string& Option::asString() const { return str; }

std::string Option::asStringWithQuotes() const
{
    // Automatically append quotes to the string if it originally had them
    return (quotes ? ('"' + str + '"') : str);
}

int Option::asInt() const { return number; }

long Option::asLong() const { return number; }

float Option::asFloat() const { return decimal; }

double Option::asDouble() const { return decimal; }

bool Option::asBool() const { return logical; }

void Option::setQuotes(bool setting) { quotes = setting; }

bool Option::hasQuotes() { return quotes; }

void Option::setRange(double num1)
{
    rangeMin = num1;
    range = MinRange;
}

void Option::setRange(double num1, double num2)
{
    rangeMin = num1;
    rangeMax = num2;
    range = MinMaxRange;
}

void Option::removeRange() { range = NoRange; }

bool Option::isInRange(double num)
{
    return ((range == NoRange) || // If there is no range, then it will always be in range
            (range == MinRange && num >= rangeMin) || // If a minimum range is set, make sure the number is >= the minimum range
            (range == MinMaxRange && num >= rangeMin && num <= rangeMax)); // If a full range is set, make sure the number is between the minimum and maximum numbers
}
