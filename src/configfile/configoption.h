// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef CFG_OPTION_H
#define CFG_OPTION_H

#include <string>
#include <memory>
#include <vector>
#include "strlib.h"

namespace cfg
{

// This class can store a value of different types based on a string
class Option
{
    using OptionVector = std::vector<Option>;

    public:
        Option(); // Default constructor
        Option(const std::string& data); // Initialize with a string value
        Option(const Option& data); // Copy constructor

        void reset(); // Sets all values to 0 and removes the range

        // Setting will compute all possible types
        bool operator=(const char* data);
        bool operator=(const std::string& data);
        Option& operator=(const Option& data);
        template <typename Type> bool operator=(Type data);
        bool setString(const std::string& data);

        // Getting will simply return the precomputed values
        const std::string& toString() const;
        std::string toStringWithQuotes() const;
        int toInt() const;
        long toLong() const;
        float toFloat() const;
        double toDouble() const;
        bool toBool() const;
        char toChar() const; // Based on int

        // This will try to cast the decimal to another type
        template <typename Type> Type to() const;

        // For determining if the option was originally read in as a string with quotes
        void setQuotes(bool setting);
        bool hasQuotes();

        // For setting the valid range
        void setRange(double num1);
        void setRange(double num1, double num2);
        void removeRange();

        // Array manipulation
        Option& push(const Option& opt = Option()); // push_back
        void pop(); // pop_back
        Option& operator[](unsigned pos);
        Option& back();
        unsigned size() const;
        void clear();
        OptionVector::iterator begin();
        OptionVector::iterator end();
        OptionVector::const_iterator begin() const;
        OptionVector::const_iterator end() const;

        // Arrays as strings
        std::string buildArrayString(const std::string& indentStr = "") const; // Returns the array in string format
        //bool parseArrayString(const std::string& arrayStr); // Sets the array elements from a string
        //bool parseArrayLines(const std::vector<std::string>& lines); // Sets the array elements from multiple lines
            // Note: This will be moved to private later.

    private:
        bool isInRange(double num);

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

        std::unique_ptr<OptionVector> options;
        // Wrapping the vector with a pointer to prevent recursive construction and incomplete type issues
        // Also, this is only created when push() is called for the first time
        // Also, this array is separate from the option itself, and nothing is kept in sync
            // This means that the first element can be different from the option.
};

template <typename Type>
bool Option::operator=(Type data)
{
    // Only set the value if it is in range
    if (isInRange((double)data))
    {
        number = data;
        decimal = data;
        logical = (data != 0);
        str = strlib::toString<Type>(data);
        quotes = false;
        return true;
    }
    return false;
}

template <typename Type>
Type Option::to() const
{
    return static_cast<Type>(decimal);
}

// Stream operator overload
std::ostream& operator<<(std::ostream& stream, const Option& option);

// Factory functions

template <typename Type>
Option makeOption(Type data)
{
    Option tmp;
    tmp = data;
    return tmp;
}

template <typename Type>
Option makeOption(Type data, double num1)
{
    Option tmp;
    tmp = data;
    tmp.setRange(num1);
    return tmp;
}

template <typename Type>
Option makeOption(Type data, double num1, double num2)
{
    Option tmp;
    tmp = data;
    tmp.setRange(num1, num2);
    return tmp;
}

}

#endif
