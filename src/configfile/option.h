// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <memory>
#include "stringutils.h"

// This class can store a value of different types based on a string
class Option
{
    public:
        Option(); // Default constructor
        Option(const std::string&); // Initialize with a string value
        template <class T> bool operator=(T); // Assignment operator
        template <class T> bool operator=(const std::string&); // Assignment operator with a string

        // Sets all values to 0 and removes the range
        void reset();

        // Setting will compute all possible types
        bool setString(const std::string&);
        template <class T> bool set(T);
        template <class T> bool set(const std::string&);

        // Getting will simply return the precomputed values
        const std::string& asString() const;
        std::string asStringWithQuotes() const;
        int asInt() const;
        long asLong() const;
        float asFloat() const;
        double asDouble() const;
        bool asBool() const;

        // For determining if the option was originally read in as a string with quotes
        void setQuotes(bool);
        bool hasQuotes();

        // For setting the valid range
        void setRange(double);
        void setRange(double, double);
        void removeRange();

    private:
        bool isInRange(double);

        enum RangeType
        {
            NoRange = 0,
            MinRange,
            MinMaxRange
        };

        // The "set" function will set all of these, no matter what the type is
        std::string str;
        long number;
        double decimal;
        bool logical;

        bool quotes;

        RangeType range;
        double rangeMin;
        double rangeMax;
};

template <class T>
bool Option::operator=(T data)
{
    return set<T>(data);
}

template <class T>
bool Option::operator=(const std::string& data)
{
    return setString(data);
}

template <class T>
bool Option::set(T data)
{
    // Only set the value if it is in range
    if (isInRange((double)data))
    {
        number = data;
        decimal = data;
        logical = (data != 0);
        str = StringUtils::toString<T>(data);
        return true;
    }
    return false;
}

template <class T>
bool Option::set(const std::string& data)
{
    return setString(data);
}

// Factory functions

template <class T>
Option makeOption(T data)
{
    Option tmp;
    tmp.set<T>(data);
    return tmp;
}

template <class T>
Option makeOption(T data, double num1)
{
    Option tmp;
    tmp.set<T>(data);
    tmp.setRange(num1);
    return tmp;
}

template <class T>
Option makeOption(T data, double num1, double num2)
{
    Option tmp;
    tmp.set<T>(data);
    tmp.setRange(num1, num2);
    return tmp;
}

#endif
