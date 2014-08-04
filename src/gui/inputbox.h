// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef INPUTBOX_H
#define INPUTBOX_H

#include <string>
#include <functional>
#include <SFML/Graphics.hpp>
#include "cursor.h"

/*
TODO:
    When characters are deleted, the text should shift right as far as possible.
        Otherwise you can hold backspace, and it will appear that the input box is empty,
        when really there are more characters hidden to the left.
    Handle mouse click more precisely
        Actually move the cursor to the closest position
    Handle highlighting
        Using shift
        Using your mouse cursor and dragging
    Handle copying and pasting
        Through the system clipboard if possible
    Integrate with guilib
*/

// Widget that handles text input from the user in a single line box
class InputBox: public sf::Drawable
{
    using CallbackType = std::function<void(InputBox&)>;

    public:
        InputBox();

        // These should be called in a loop
        void handleEvent(const sf::Event& event);
        void handleMouseEvent(const sf::Event& event, const sf::Vector2f& pos);
        void update(); // This may need a delta time
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

        // Widget settings
        void setFillMode(bool mode, char c = '*'); // For passwords and stuff
        void setLabel(const std::string& str = ""); // Only shows up when nothing is entered

        // Widget state
        void setText(const std::string& str = "");
        const std::string& getText() const;
        void setNumber(double num = 0);
        double getNumber() const;
        void setFocus(bool);
        bool hasFocus() const;
        void setTextEnteredCallback(CallbackType callback);

    private:
        // Text manipulation
        void insertChar(char c); // Inserts a character at the current position
        void backspace();
        void del();

        // Cursor movement
        void left();
        void right();
        void home();
        void end();

        // Other methods
        void handleKeyPressed(const sf::Keyboard::Key& keyCode);
        void updateText(bool invoke = true); // Sets the visual text from the internal string
        void updateCursorPos(); // Sets the visual cursor position

        // Variables
        std::string textStr;
        bool focus;
        bool fillMode;
        char fillModeChar;
        unsigned fontSize;
        Cursor cursor;
        sf::RectangleShape box;
        sf::FloatRect collisionBox;

        // Text
        sf::Text text;
        sf::Text labelText;
        sf::RenderTexture texture;
        sf::Sprite sprite;
        CallbackType textEnteredCallback;
};

#endif
