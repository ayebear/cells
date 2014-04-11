#ifndef RULEGRID_H
#define RULEGRID_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "ruleset.h"

// Draws an interactive table of a set of rules for CA
class RuleGrid: public sf::Drawable
{
    public:
        RuleGrid();
        void setFont(const sf::Font& f, unsigned fontSize = 16);
        void setPosition(const sf::Vector2f& pos);
        void setSize(int s = 16);
        void setRules(const RuleSet& rules);
        bool handleMouseEvent(const sf::Event& event, const sf::Vector2f& pos, RuleSet& rules);
        void draw(sf::RenderTarget& window, sf::RenderStates states) const;

    private:
        void updateHeaderPositions();
        void generateGrid();
        void updateMouseState(sf::Mouse::Button button);
        bool modifyRule(RuleSet& rules, const sf::Vector2f& pos);

        sf::Vector2f position;
        sf::Image image;
        sf::Texture texture;
        sf::Sprite sprite;
        sf::VertexArray grid;
        sf::RectangleShape headerBg;
        sf::Text description;
        std::vector<sf::Text> headers;
        int size;
        int mouseState;
        sf::FloatRect collisionBox;

        enum MouseState
        {
            NotPressed = -1,
            RightPressed = 0,
            LeftPressed = 1
        };

        static const sf::Color colors[2];
        static const unsigned width = 9;
        static const unsigned height = 2;
        static const unsigned gridArraySize = 28;
};

#endif
