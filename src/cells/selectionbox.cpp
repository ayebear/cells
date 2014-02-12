// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "selectionbox.h"
#include <iostream>

SelectionBox::SelectionBox():
    minSize(0, 0)
{
    setPosition(sf::Vector2i(0, 0));
    setCorner(sf::Vector2i(1, 1));
    rectShape.setFillColor(sf::Color::Transparent);
}

void SelectionBox::setPosition(const sf::Vector2i& pos)
{
    rectShape.setPosition(pos.x, pos.y);
    rect.left = pos.x;
    rect.top = pos.y;
}

void SelectionBox::setCorner(const sf::Vector2i& pos, bool square)
{
    if (square)
    {
        int squareWidth = std::max(pos.x - rect.left, pos.y - rect.top);
        setSize(squareWidth, squareWidth);
    }
    else
        setSize(pos.x - rect.left, pos.y - rect.top);
}

void SelectionBox::setSize(const sf::Vector2i& size)
{
    setSize(size.x, size.y);
}

void SelectionBox::setSize(int width, int height)
{
    rect.width = std::max(width, static_cast<int>(minSize.x));
    rect.height = std::max(height, static_cast<int>(minSize.y));
    rectShape.setSize(sf::Vector2f(rect.width, rect.height));
    // May need to swap the size and position if the size is negative
}

void SelectionBox::setColor(const sf::Color& col)
{
    rectShape.setOutlineColor(col);
}

void SelectionBox::setThickness(float thickness)
{
    rectShape.setOutlineThickness(thickness);
}

void SelectionBox::setMinimumSize(const sf::Vector2u& newMinSize)
{
    minSize = newMinSize;
}

bool SelectionBox::isMinimumSize() const
{
    return (rect.width == static_cast<int>(minSize.x) && rect.height == static_cast<int>(minSize.y));
}

sf::Vector2u SelectionBox::getSize() const
{
    return sf::Vector2u(rect.width, rect.height);
}

const sf::IntRect& SelectionBox::getRect() const
{
    return rect;
}

void SelectionBox::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    window.draw(rectShape);
}
