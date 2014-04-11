// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef SELECTIONBOX_H
#define SELECTIONBOX_H

#include <SFML/Graphics.hpp>

/*
This class contains a rectangle shape that can be used for selecting things.
*/
class SelectionBox: public sf::Drawable
{
    public:
        SelectionBox();
        void setPosition(const sf::Vector2i& pos); // First corner selected
        void setCorner(const sf::Vector2i& pos, bool square = false); // Second corner selected (can be less than above)
        void setSize(const sf::Vector2i& size); // Sets the size, similar to above
        void setSize(int width, int height); // Overload of above
        void setColor(const sf::Color& col); // Sets the outline color
        void setThickness(float thickness); // Sets the outline thickness
        void setMinimumSize(const sf::Vector2u& newMinSize); // Sets the minimum size the cursor should be
        bool isMinimumSize() const; // Returns true if the current size is the minimum size
        sf::Vector2u getSize() const; // Gets the size of the rectangle
        const sf::IntRect& getRect() const; // Returns the logical rectangle
        sf::Vector2i getPosition() const; // Returns the top-left corner position
        void draw(sf::RenderTarget& window, sf::RenderStates states) const;

    private:
        sf::IntRect rect;
        sf::RectangleShape rectShape;
        sf::Vector2u minSize;
};

#endif
