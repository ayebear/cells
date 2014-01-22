// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef RULESET_H
#define RULESET_H

#include <string>

/*
This class contains a changeable set of rules to be used with cellular automation.
The rules can be set from a string in the S/B (like 3/23) or B{list}/S{list] formats.
The rules can also be set from binary bits with the setRule function.
You can use the getRule function to determine the new state of a cell by passing in the
    the current cell state (type), and live neighbor count (count).
*/
class RuleSet
{
    public:
        RuleSet();
        RuleSet(const std::string& str);
        void setFromString(const std::string& str); // Sets the rules from a rule string
        const std::string& toString() const; // Returns the rules in the same string format as above
        bool getRule(unsigned type, unsigned count) const; // Returns a rule
        void setRule(unsigned type, unsigned count, bool state); // Sets a rule
        void clear(); // Sets all of the rules to false
        
        enum {Birth = 0, Survival = 1}; // The rule types

    private:
        void generateRuleString() const; // Generates a new string only if needed

        bool rules[2][9]; // Holds all of the rules, you can plug in the cell state and live neighbor count to get the new state
        mutable bool rulesChanged; // Used so that the string doesn't have to be re-generated each time the rules change
        mutable std::string ruleString; // Stores the string version of the rules
};

#endif
