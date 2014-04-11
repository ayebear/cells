#ifndef WIDGET_H
#define WIDGET_H

#include <SFML/Graphics.hpp>

class Widget: public sf::Drawable
{
    public:
        Widget();
        //virtual ~Widget();
        void setFocus(bool);
        bool hasFocus() const;

    protected:
        bool focus;

    private:
};

#endif
