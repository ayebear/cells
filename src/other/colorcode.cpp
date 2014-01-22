// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "colorcode.h"
#include <sstream>
#include <iomanip>

ColorCode::ColorCode()
{
    setColor(sf::Color::Black);
}

ColorCode::ColorCode(const std::string& str)
{
    setString(str);
}

ColorCode::ColorCode(const sf::Color& col)
{
    setColor(col);
}

ColorCode::ColorCode(unsigned char r, unsigned char g, unsigned char b)
{
    setRGB(r, g, b);
}

ColorCode& ColorCode::setString(const std::string& str)
{
    strToColor(str, color, colorStr);
    return *this;
}

ColorCode& ColorCode::setColor(const sf::Color& col)
{
    color = col;
    colorToStr(color, colorStr);
    return *this;
}

ColorCode& ColorCode::setRGB(unsigned char r, unsigned char g, unsigned char b)
{
    color.r = r;
    color.g = g;
    color.b = b;
    colorToStr(color, colorStr);
    return *this;
}

const std::string& ColorCode::toString() const
{
    return colorStr;
}

const sf::Color& ColorCode::toColor() const
{
    return color;
}

void ColorCode::colorToStr(const sf::Color& col, std::string& str) const
{
    std::ostringstream ss;
    ss << std::hex << std::uppercase << std::setfill('0');
    ss << '#' << std::setw(2) << col.r << col.g << col.b;
    str = ss.str();
}

bool ColorCode::strToColor(const std::string& str, sf::Color& col, std::string& outStr) const
{
    bool status = false;
    std::string validHex;
    for (char c: str)
    {
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'F') ||
            (c >= 'a' && c <= 'f'))
            validHex += c;
    }
    // Supports #000 and #000000 formats
    unsigned length = validHex.size();
    if (length == 6 || length == 3)
    {
        unsigned hexWidth = length / 3;
        col.r = hexStrToChar(validHex.substr(0, hexWidth));
        col.g = hexStrToChar(validHex.substr(1 * hexWidth, hexWidth));
        col.b = hexStrToChar(validHex.substr(2 * hexWidth, hexWidth));
        outStr = "#" + validHex;
        status = true;
    }
    return status;
}

unsigned char ColorCode::hexStrToChar(const std::string& str) const
{
    unsigned char out;
    std::stringstream ss;
    ss << std::hex << str;
    ss >> out;
    return out;

    /*unsigned char out = 0;
    unsigned char factor = 16;
    for (unsigned char c: str)
    {
        out += (factor * c);
        factor *= 16;
    }
    return out;*/
}
