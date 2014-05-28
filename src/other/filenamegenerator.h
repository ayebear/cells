#ifndef FILENAMEGENERATOR_H
#define FILENAMEGENERATOR_H

#include <string>

class FilenameGenerator
{
    public:
        FilenameGenerator();
        void setFormat(const std::string& str); // %n is replaced with the number
        std::string getNextFilename() const; // Finds the next filename, and returns it
        std::string getFilename() const; // Returns generated filename
        void findNext() const; // Finds the next unused filename

    private:
        std::string format;
        mutable int currentNum;
};

#endif
