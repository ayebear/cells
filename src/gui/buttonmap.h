#ifndef BUTTONMAP_H
#define BUTTONMAP_H

#include <map>
#include <SFML/Graphics.hpp>
#include "button.h"

class ButtonMap: public sf::Drawable
{
    public:
        ButtonMap();
        Button& operator[](const std::string& name);
        void handleMouseEvent(const sf::Event& event, const sf::Vector2f& pos);
        void draw(sf::RenderTarget& window, sf::RenderStates states) const;

    private:
        std::map<std::string, Button> buttons;
};

#endif
