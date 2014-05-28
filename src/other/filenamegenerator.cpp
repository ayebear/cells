#include "filenamegenerator.h"
#include <fstream>
#include <algorithm>
#include "strlib.h"

FilenameGenerator::FilenameGenerator():
    currentNum(0)
{
}

void FilenameGenerator::setFormat(const std::string& str)
{
    format = str;
}

std::string FilenameGenerator::getNextFilename() const
{
    findNext();
    return getFilename();
}

std::string FilenameGenerator::getFilename() const
{
    std::string filename(format);
    if (filename.find("%n") == std::string::npos)
        filename += " %n.png";
    strlib::replaceAll(filename, "%n", std::to_string(currentNum));
    return filename;
}

void FilenameGenerator::findNext() const
{
    bool status = true;
    while (status)
    {
        ++currentNum;
        std::ifstream file(getFilename().c_str());
        status = file;
    }
}
