#include "buttonmap.h"

ButtonMap::ButtonMap()
{
}

Button& ButtonMap::operator[](const std::string& name)
{
    return buttons[name];
}

void ButtonMap::handleMouseEvent(const sf::Event& event, const sf::Vector2f& pos)
{
    for (auto& button: buttons)
        button.second.handleMouseEvent(event, pos);
}

void ButtonMap::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    for (auto& button: buttons)
        window.draw(button.second);
}
