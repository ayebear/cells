#ifndef TOOL_H
#define TOOL_H

#include <SFML/Graphics.hpp>
#include "selectionbox.h"

class Tool: public sf::Drawable
{
    public:
        enum
        {
            Paint = 0,
            Copy,
            NormalSimulator,
            ToroidalSimulator,
            TotalTools
        };

        Tool();
        void setTool(int tool);
        int getTool() const;
        void updateCursor(const sf::Vector2i& pos);
        void handleMouseButtonPressed(const sf::Event::MouseButtonEvent& mouseButton, const sf::Vector2i& mousePos);
        bool handleMouseButtonReleased(const sf::Event::MouseButtonEvent& mouseButton, const sf::Vector2i& mousePos);
        bool handleMouseWheelMoved(const sf::Event::MouseWheelEvent& mouseWheel);
        void draw(sf::RenderTarget& window, sf::RenderStates states) const;

        // Move these to private!
        SelectionBox cursor;
        bool changingSelection; // If the cursor size is being changed by the user
        bool squareSelection; // Square selection if true, otherwise rectangular selection

    private:
        bool controlKeyPressed() const;

        int currentTool; // The current tool being used
        sf::Color toolColors[TotalTools];
};

#endif
