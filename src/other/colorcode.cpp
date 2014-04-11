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

ColorCode::ColorCode(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    setRGB(r, g, b, a);
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

ColorCode& ColorCode::setRGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
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
    ss << '#' << std::setw(2) << col.r << col.g << col.b << col.a;
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
    // Supports 3, 4, 6, and 8 length formats
    unsigned length = validHex.size();
    if (length == 3 || length == 4)
    {
        // Convert #RGB to #RRGGBB
        // Or #RGBA to #RRGGBBAA
        std::string tmp = validHex;
        validHex.resize(length * 2);
        for (unsigned i = 0; i < validHex.size(); ++i)
            validHex[i] = tmp[i / 2];
        length = validHex.size();
    }
    if (length == 6 || length == 8)
    {
        col.r = hexStrToInt(validHex.substr(0, 2));
        col.g = hexStrToInt(validHex.substr(2, 2));
        col.b = hexStrToInt(validHex.substr(4, 2));
        if (length == 8)
            col.a = hexStrToInt(validHex.substr(6, 2));
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
