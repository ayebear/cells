// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef COLORCODE_H
#define COLORCODE_H

#include <string>
#include <SFML/Graphics/Color.hpp>

/*
A small class for storing a color code in a format like #00FF00 or #FFF.
Also converts between that and the equivalent SFML color objects.
*/
class ColorCode
{
    public:
        ColorCode();
        ColorCode(const std::string& str);
        ColorCode(const sf::Color& col);
        ColorCode(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
        ColorCode& operator=(const std::string& str);
        ColorCode& operator=(const sf::Color& col);
        ColorCode& setString(const std::string& str);
        ColorCode& setColor(const sf::Color& col);
        ColorCode& setRGB(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
        const std::string& toString() const;
        const sf::Color& toColor() const;

    private:
        void colorToStr(const sf::Color& col, std::string& str) const;
        bool strToColor(const std::string& str, sf::Color& col, std::string& outStr) const;
        unsigned hexStrToInt(const std::string& str) const;

        sf::Color color;
        std::string colorStr;
};

#endif
