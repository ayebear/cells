// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "colorcode.h"
#include <sstream>
#include <iomanip>
#include <iostream>

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

ColorCode& ColorCode::operator=(const std::string& str)
{
    return setString(str);
}

ColorCode& ColorCode::operator=(const sf::Color& col)
{
    return setColor(col);
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
            validHex += toupper(c);
    }
    // Supports #000 and #000000 formats
    unsigned length = validHex.size();
    if (length == 3)
    {
        // Convert #ABC to #AABBCC
        std::string tmp = validHex;
        validHex.resize(6);
        validHex[0] = tmp[0];
        validHex[1] = tmp[0];
        validHex[2] = tmp[1];
        validHex[3] = tmp[1];
        validHex[4] = tmp[2];
        validHex[5] = tmp[2];
        length = validHex.size();
    }
    if (length == 6)
    {
        unsigned hexWidth = length / 3;
        col.r = hexStrToInt(validHex.substr(0, hexWidth));
        col.g = hexStrToInt(validHex.substr(1 * hexWidth, hexWidth));
        col.b = hexStrToInt(validHex.substr(2 * hexWidth, hexWidth));
        outStr = "#" + validHex;
        status = true;
    }
    return status;
}

unsigned ColorCode::hexStrToInt(const std::string& str) const
{
    unsigned out;
    std::stringstream ss;
    ss << std::hex << str;
    ss >> out;
    return out;
}
