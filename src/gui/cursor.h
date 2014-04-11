// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef CURSOR_H
#define CURSOR_H

#include <SFML/Graphics.hpp>

// A simple class that has a drawable cursor and timer
class Cursor: public sf::Drawable
{
    public:
        Cursor();
        void update();
        void restartTimer();
        void draw(sf::RenderTarget&, sf::RenderStates) const;

        // TODO: Move these to private and encapsulate the -1 cursor position stuff
        sf::RectangleShape rect;
        sf::Clock timer;
        int pos;
        bool show;

    private:
        static const float blinkRate;
        static const int endPos;
};

#endif
