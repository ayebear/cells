// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "ruleset.h"

RuleSet::RuleSet()
{
    clear();
}

void RuleSet::setFromString(const std::string& str)
{
    clear();
    unsigned type = Survival;
    for (char c: str)
    {
        if (c == 'B' || c == 'b')
            type = Birth;
        else if (c == 'S' || c == 's')
            type = Survival;
        else if (c == '/' || c == '\\')
            ++type %= 2;
        else if (c >= '0' && c <= '8')
            rules[type][c - '0'] = true;
    }
    rulesChanged = true;
}

const std::string& RuleSet::toString() const
{
    generateRuleString();
    return ruleString;
}

bool RuleSet::getRule(unsigned type, unsigned count) const
{
    return rules[type][count];
}

void RuleSet::setRule(unsigned type, unsigned count, bool state)
{
    rules[type][count] = state;
    rulesChanged = true;
}

void RuleSet::clear()
{
    for (auto& ruleSet: rules)
        for (bool& rule: ruleSet)
            rule = false;
    rulesChanged = true;
}

void RuleSet::generateRuleString() const
{
    if (rulesChanged)
    {
        ruleString = "B";
        for (unsigned type = 0; type <= 1; ++type)
        {
            unsigned num = 0;
            for (bool rule: rules[type])
            {
                if (rule)
                    ruleString += static_cast<char>(num + '0');
                ++num;
            }
            if (type == Birth)
                ruleString += "/S";
        }
        rulesChanged = false;
    }
}

