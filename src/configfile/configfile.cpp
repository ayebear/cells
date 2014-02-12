// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "configfile.h"
#include <fstream>
#include <iostream>
#include "stringutils.h"

ConfigFile::ConfigFile(bool warnings)
{
    showWarnings = warnings;
}

ConfigFile::ConfigFile(const string& filename, bool warnings)
{
    showWarnings = warnings;
    loadFromFile(filename);
}

ConfigFile::ConfigFile(const ConfigMap& defaultOptions, bool warnings)
{
    showWarnings = warnings;
    setDefaultOptions(defaultOptions);
}

ConfigFile::ConfigFile(const Section& defaultOptions, const string& section, bool warnings)
{
    showWarnings = warnings;
    setDefaultOptions(defaultOptions, section);
}

ConfigFile::ConfigFile(const string& filename, const ConfigMap& defaultOptions, bool warnings)
{
    showWarnings = warnings;
    setDefaultOptions(defaultOptions);
    loadFromFile(filename);
}

ConfigFile::ConfigFile(const string& filename, const Section& defaultOptions, const string& section, bool warnings)
{
    showWarnings = warnings;
    setDefaultOptions(defaultOptions, section);
    loadFromFile(filename);
}

bool ConfigFile::loadFromFile(const string& filename)
{
    bool status = false;
    configFilename = filename;
    std::vector<string> lines;
    if (StringUtils::readLinesFromFile(configFilename, lines, false))
    {
        parseLines(lines);
        status = true;
    }
    else
        std::cout << "Error loading \"" << configFilename << "\"\n";
    return status;
}

void ConfigFile::loadFromString(const string& str)
{
    std::vector<string> lines;
    StringUtils::getLinesFromString(str, lines, false);
    parseLines(lines);
}

bool ConfigFile::writeToFile(string outputFilename) const
{
    if (outputFilename.empty())
        outputFilename = configFilename;
    // Write the string to the output file
    return StringUtils::writeStringToFile(outputFilename, buildString());
}

void ConfigFile::writeToString(string& str) const
{
    for (const auto& section: options) // Go through all of the sections
    {
        if (!section.first.empty())
            str += '[' + section.first + "]\n"; // Add the section line if it is not blank
        for (const auto& o: section.second) // Go through all of the options in this section
            str += o.first + " = " + o.second.asStringWithQuotes() + '\n'; // Include the original quotes if any
        str += '\n';
    }
    if (!str.empty() && str.back() == '\n')
        str.pop_back(); // Strip the extra new line at the end
}

string ConfigFile::buildString() const
{
    string configStr;
    writeToString(configStr);
    return configStr;
}

Option& ConfigFile::getOption(const string& name, const string& section)
{
    return options[getCurrentSection(section)][name];
}

Option& ConfigFile::operator[](const string& name)
{
    return options[currentSection][name];
}

bool ConfigFile::optionExists(const string& name, const string& section) const
{
    auto sectionFound = options.find(getCurrentSection(section));
    return (sectionFound != options.end() && sectionFound->second.find(name) != sectionFound->second.end());
}

void ConfigFile::setDefaultOptions(const ConfigMap& defaultOptions)
{
    options.insert(defaultOptions.begin(), defaultOptions.end());
}

void ConfigFile::setDefaultOptions(const Section& defaultOptions, const string& section)
{
    options[getCurrentSection(section)].insert(defaultOptions.begin(), defaultOptions.end());
}

void ConfigFile::setSection(const string& section)
{
    currentSection = section;
}

ConfigFile::ConfigMap::iterator ConfigFile::begin()
{
    return options.begin();
}

ConfigFile::ConfigMap::iterator ConfigFile::end()
{
    return options.end();
}

ConfigFile::Section& ConfigFile::getSection(const string& section)
{
    return options[getCurrentSection(section)];
}

bool ConfigFile::eraseOption(const string& name, const string& section)
{
    auto sectionFound = options.find(getCurrentSection(section));
    if (sectionFound != options.end()) // If the section exists
        return (sectionFound->second.erase(name) > 0); // Erase the option
    return false;
}

bool ConfigFile::eraseSection(const string& section)
{
    return (options.erase(getCurrentSection(section)) > 0); // Erase the section
}

void ConfigFile::clear()
{
    options.clear(); // Clear all of the sections and options
}

void ConfigFile::parseLines(std::vector<string>& lines)
{
    string section = "";
    bool multiLineComment = false;
    int commentType = StringUtils::NoComment;
    for (string& line: lines) // Iterate through the std::vector of strings
    {
        StringUtils::trimWhiteSpace(line);
        commentType = StringUtils::stripComments(line, multiLineComment);

        if (commentType == StringUtils::MultiStart)
            multiLineComment = true;

        if (!multiLineComment && !line.empty()) // Don't process a comment or an empty line
        {
            if (isSection(line))
                parseSectionLine(line, section); // Example: "[Section]"
            else
                parseOptionLine(line, section); // Example: "Option = Value"
        }

        if (commentType == StringUtils::MultiEnd)
            multiLineComment = false;
    }
}

bool ConfigFile::isSection(const string& str) const
{
    return (str.size() >= 2 && str.front() == '[' && str.back() == ']');
}

void ConfigFile::parseSectionLine(const string& line, string& section)
{
    section = line.substr(1, line.size() - 2); // Set the current section
    options[section]; // Add that section to the map
}

void ConfigFile::parseOptionLine(const string& line, const string& section)
{
    size_t equalPos = line.find("="); // Find the position of the "=" symbol
    if (equalPos != string::npos && equalPos >= 1) // Ignore the line if there is no "=" symbol
    {
        string name, value;
        // Extract the name and value
        name = line.substr(0, equalPos);
        value = line.substr(equalPos + 1);
        // Trim any whitespace around the name and value
        StringUtils::trimWhiteSpace(name);
        StringUtils::trimWhiteSpace(value);
        // Remove outer quotes if any
        bool trimmedQuotes = StringUtils::trimQuotes(value);
        // Add/update the option in memory
        Option& option = options[section][name];
        bool optionSet = (option = value);
        if (trimmedQuotes) // If quotes were removed
            option.setQuotes(true); // Add quotes to the option
        if (showWarnings && !optionSet)
            std::cout << "Warning: Option \"" << name << "\" was out of range. Using default value: " << option.asStringWithQuotes() << std::endl;
    }
}

const string& ConfigFile::getCurrentSection(const string& section) const
{
    return (section.empty() ? currentSection : section);
}
