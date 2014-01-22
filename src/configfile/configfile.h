// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <map>
#include <vector>
#include <string>
#include "option.h"

using namespace std;

/*
TODO:
    Get rid of StringUtils dependency.
    Get rid of using namespace std.
    Config file writing should preserve formatting and comments.
        Maybe it could "merge" the contents into the file, new values in existing options would be updated,
            new options would be appended to the end of the file.
        In order for this to do this, it must either read the file before writing (which is slow), or it must
            store the entire original file in memory (which takes up a lot of memory).
        Either way, it should be an optional "feature", where you can choose to rebuild the file, or merge.
        Right now it is completely destructive, it doesn't preserve ordering, comments, invalid data, or whitespace.
*/

/*
A class for reading/writing configuration files.
See readme.txt for more information.
*/
class ConfigFile
{
    public:
        // Types used to store the options
        typedef map<string,Option> Section;
        typedef map<string,Section> ConfigMap;

        // Constructors
        ConfigFile(bool = false); // Would create an empty config file object
        ConfigFile(const string&, bool = false); // Loads options from a file
        ConfigFile(const ConfigMap&, bool = false); // Loads default options from memory
        ConfigFile(const Section&, const string& = "", bool = false); // Loads default options for a single section from memory
        ConfigFile(const string&, const ConfigMap&, bool = false); // Loads default options and then options from a file
        ConfigFile(const string&, const Section&, const string& = "", bool = false); // Loads default options for a single section and then options from a file

        // Loading/saving
        bool loadFromFile(const string&); // Loads options from a file
        void loadFromString(const string&); // Loads options from a string
        bool writeToFile(string = "") const; // Saves current options to a file (default is last loaded)
        void writeToString(string&) const; // Saves current options to a string (same format as writeToFile)
        string buildString() const; // Returns a string of the current options (same format as writeToFile)
        void setShowWarnings(bool); // Enable/disable showing warnings

        // Accessing/modifying options
        Option& getOption(const string&, const string& = ""); // Returns a reference to an option with the specified name (and section). If it does not exist, it will be automatically created
        Option& operator[](const string&); // Returns a reference to an option with the specified name. If it does not exist, it will be automatically created
        bool optionExists(const string&, const string& = "") const; // Returns if an option already exists
        void setDefaultOptions(const ConfigMap&); // Sets initial values in the map from another map in memory
        void setDefaultOptions(const Section&, const string& = ""); // Sets initial values for a single section
        void setSection(const string& = ""); // Sets the default current section to be used
        ConfigMap::iterator begin(); // Returns an iterator to the beginning of the map
        ConfigMap::iterator end(); // Returns an iterator to the end of the map
        Section& getSection(const string& = ""); // Returns a reference to a section

        // Erasing options
        bool eraseOption(const string&, const string& = ""); // Erases an option, returns true if the option was successfully erased
        bool eraseSection(const string& = ""); // Erases a section, returns true if the section was successfully erased
        void clear(); // Clears all of the sections and options in memory, but keeps the filename

    private:
        void parseLines(vector<string>&); // Processes the lines in memory and adds them to the options map
        bool isSection(const string&) const; // Returns true if the line is a section header
        void parseSectionLine(const string&, string&); // Processes a section header line and adds a section to the map
        void parseOptionLine(const string&, const string&); // Processes an option line and adds an option to the map
        const string& getCurrentSection(const string& = "") const; // Returns the current section if passed an empty string, otherwise returns back the string passed in

        ConfigMap options; // The data structure for storing all of the options in memory
        string configFilename; // The filename of the config file to read/write to
        string currentSection; // The default current section
        bool showWarnings; // Show warnings when loading options that are out of range
};

#endif
