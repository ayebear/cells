// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef CFG_FILE_H
#define CFG_FILE_H

#include <map>
#include <vector>
#include <string>
#include "configoption.h"

using std::string;

namespace cfg
{

/*
A class for reading/writing configuration files.
See README.md for more information.

TODO:
    Handle escape codes inside of strings.
    Handle multiple array elements on the same line.
    Writing should optionally preserve formatting and comments.
*/
class File
{
    public:
        // Types used to store the options
        using Section = std::map<string, Option>;
        using ConfigMap = std::map<string, Section>;

        // Constructors
        File();
        File(const string& filename);
        File(const ConfigMap& defaultOptions, bool warnings = false);
        File(const string& filename, const ConfigMap& defaultOptions, bool warnings = false);
        ~File();

        // Loading/saving
        bool loadFromFile(const string& filename); // Loads options from a file
        void loadFromString(const string& str); // Loads options from a string
        bool writeToFile(string filename = "") const; // Saves current options to a file (default is last loaded)
        void writeToString(string& str) const; // Saves current options to a string (same format as writeToFile)
        string buildString() const; // Returns a string of the current options (same format as writeToFile)

        // Settings
        void setShowWarnings(bool setting = false); // Enable/disable showing warnings
        void setAutosave(bool setting = false); // Enable/disable autosave
        void resetSettings(); // Resets all settings

        // Accessing/modifying options
        Option& operator()(const string& name, const string& section); // Returns a reference to an option with the specified name (and section). If it does not exist, it will be automatically created
        Option& operator()(const string& name); // Same as above but uses the current section
        bool optionExists(const string& name, const string& section) const; // Returns true if an option exists
        bool optionExists(const string& name) const; // Returns true if an option exists
        void setDefaultOptions(const ConfigMap& defaultOptions); // Sets initial values in the map from another map in memory
        ConfigMap::iterator begin(); // Returns an iterator to the beginning of the map
        ConfigMap::iterator end(); // Returns an iterator to the end of the map

        // Accessing/modifying sections
        void useSection(const string& section = ""); // Sets the default current section to be used
        Section& getSection(const string& section); // Returns a reference to a section
        Section& getSection(); // Returns a reference to the default section
        bool sectionExists(const string& section) const; // Returns true if a section exists
        bool sectionExists() const; // Returns true if a section exists

        // Erasing options/sections
        bool eraseOption(const string& name, const string& section); // Erases an option, returns true if the option was successfully erased
        bool eraseOption(const string& name); // Erases an option from the default section
        bool eraseSection(const string& section); // Erases a section, returns true if the section was successfully erased
        bool eraseSection(); // Erases the default section
        void clear(); // Clears all of the sections and options in memory, but keeps the filename

    private:
        enum class Comment
        {
            None,   // No comment
            Single, // Single line comment
            Start,  // Start of multiple line comment
            End     // End of multiple line comment
        };

        // File parsing
        void parseLines(std::vector<string>& lines); // Processes the lines in memory and adds them to the options map
        bool isSection(const string& section) const; // Returns true if the line is a section header
        void parseSectionLine(const string& line, string& section); // Processes a section header line and adds a section to the map
        void parseOptionLine(const string& line, const string& section); // Processes an option line and adds an option to the map
        //void setOption(const string& section, const string& name, const string& value); // Adds or sets an option
        bool setOption(Option& option, const string& value); // Sets an existing option
        Option& getArrayOption(const string& section, const string& name); // Returns an option at the current array level
        void startArray(Option& option); // Starts another array when "{" is found

        // Comment handling
        bool isEndComment(const string& str) const; // Returns true if it contains a multiple-line end comment symbol
        Comment getCommentType(const string& str, bool checkEnd = false) const; // Returns an enum value of the comment type
        Comment stripComments(string& str, bool checkEnd = false); // Removes all comments from a string

        // Objects/variables
        ConfigMap options; // The data structure for storing all of the options in memory
        string configFilename; // The filename of the config file to read/write to
        string currentSection; // The default current section
        bool showWarnings; // Show warnings when loading options that are out of range
        bool autosave; // Automatically write to the last file loaded on destruction

        // Array related objects
        std::vector<unsigned> currentArrayStack; // Stack of array indices for current option
        string arrayOptionName; // Name of option whose array is currently being handled
};

}

#endif
