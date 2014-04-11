#ifndef GROUPBOX_H
#define GROUPBOX_H

#include <SFML/Graphics.hpp>

// Widget that has a label on a rectangle shape
// Meant for other widgets to be organized inside of these
class GroupBox: public sf::Drawable
{
    public:
        GroupBox();
        void draw(sf::RenderTarget& window, sf::RenderStates states) const;

        // Style settings
        void setColor(const std::string& setting, const std::string& color);
        void setColor(const std::string& setting, const sf::Color& color);
        void setFont(const sf::Font& f);
        void setFontSize(unsigned size = 16);
        void setOutlineThickness(float thickness = 2);

        // Layout settings
        void setPosition(const sf::Vector2f& pos);
        void setSize(const sf::Vector2u& size);

        // Widget state
        void setText(const std::string& str = "");

    private:
        void updateTextPosition();
        void updateLine();

        sf::RectangleShape box;
        sf::RectangleShape underline;
        sf::Text text;
};

#endif
