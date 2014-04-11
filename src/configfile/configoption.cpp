// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "configoption.h"

namespace cfg
{

Option::Option():
    str(),
    number(0),
    decimal(0),
    logical(false),
    quotes(false),
    range(NoRange),
    rangeMin(0),
    rangeMax(0)
{
}

Option::Option(const std::string& data):
    Option()
{
    setString(data);
}

Option::Option(const Option& data):
    Option()
{
    operator=(data);
}

void Option::reset()
{
    quotes = false;
    range = NoRange;
    operator=(0);
}

bool Option::operator=(const char* data)
{
    return setString(data);
}

bool Option::operator=(const std::string& data)
{
    return setString(data);
}

Option& Option::operator=(const Option& data)
{
    str = data.str;
    number = data.number;
    decimal = data.decimal;
    logical = data.logical;
    quotes = data.quotes;
    range = data.range;
    rangeMin = data.rangeMin;
    rangeMax = data.rangeMax;
    if (data.options)
        options.reset(new OptionVector(*data.options));
    else
        options.reset();
    return *this;
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
        quotes = !(parsedNumber || strlib::isBool(data)); // No quotes around a number or a boolean
        logical = (parsedNumber ? (decimal != 0) : strlib::strToBool(data)); // Convert to a boolean
        str = data; // Set the string
        return true;
    }
    return false;
}

const std::string& Option::toString() const
{
    return str;
}

std::string Option::toStringWithQuotes() const
{
    // Automatically append quotes to the string if it originally had them
    return (quotes ? ('"' + str + '"') : str);
}

int Option::toInt() const
{
    return number;
}

long Option::toLong() const
{
    return static_cast<long>(number);
}

float Option::toFloat() const
{
    return static_cast<float>(decimal);
}

double Option::toDouble() const
{
    return decimal;
}

bool Option::toBool() const
{
    return logical;
}

char Option::toChar() const
{
    return static_cast<char>(number);
}

void Option::setQuotes(bool setting)
{
    quotes = setting;
}

bool Option::hasQuotes()
{
    return quotes;
}

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

void Option::removeRange()
{
    range = NoRange;
}

Option& Option::push(const Option& opt)
{
    if (!options)
        options.reset(new OptionVector());
    options->push_back(opt);
    return options->back();
}

void Option::pop()
{
    if (options && !options->empty())
        options->pop_back();
}

Option& Option::operator[](unsigned pos)
{
    return ((*options)[pos]);
}

Option& Option::back()
{
    return options->back();
}

unsigned Option::size() const
{
    return (options ? options->size() : 0);
}

void Option::clear()
{
    options.reset();
}

Option::OptionVector::iterator Option::begin()
{
    return options->begin();
}

Option::OptionVector::iterator Option::end()
{
    return options->end();
}

Option::OptionVector::const_iterator Option::begin() const
{
    return options->begin();
}

Option::OptionVector::const_iterator Option::end() const
{
    return options->end();
}

std::string Option::buildArrayString(const std::string& indentStr) const
{
    // Continue building array strings until the option is just a single element and not an array
    if (options)
    {
        std::string nextIndentStr(indentStr + '\t');
        // Build the array string
        std::string arrayStr("{\n");
        unsigned arraySize = options->size();
        for (unsigned i = 0; i < arraySize; ++i)
        {
            arrayStr += nextIndentStr;
            arrayStr += (*options)[i].buildArrayString(nextIndentStr);
            if (i < arraySize - 1)
                arrayStr += ",\n";
        }
        arrayStr += '\n' + indentStr + '}';
        return arrayStr;
    }
    else
        return toStringWithQuotes();
}

// Might be better to have the options themselves handle parsing the array strings.
// Then it could return the current size of the "array stack", so that ConfigFile can know
// when the array ends. And it would only be able to take 1 line at a time, because ConfigFile
// won't know anything about arrays except them starting with a "{".
/*bool Option::parseArrayString(const std::string& arrayStr)
{
    // This will have to handle escape codes and all in order to work with multiple elements
    // on the same line. This could split all of the elements into an array, and then pass
    // it to parseArrayLines();
    // Something like this:
    // parseArrayLines(split(arrayStr));
    return false;
}

bool Option::parseArrayLines(const std::vector<std::string>& lines)
{
    return false;
}*/

bool Option::isInRange(double num)
{
    return ((range == NoRange) || // If there is no range, then it will always be in range
            (range == MinRange && num >= rangeMin) || // If a minimum range is set, make sure the number is >= the minimum range
            (range == MinMaxRange && num >= rangeMin && num <= rangeMax)); // If a full range is set, make sure the number is between the minimum and maximum numbers
}

std::ostream& operator<<(std::ostream& stream, const Option& option)
{
    stream << option.toString();
    return stream;
}

}
