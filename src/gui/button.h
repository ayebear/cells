#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include <functional>
#include <SFML/Graphics.hpp>

/*
TODO:
    Work with multiple modes
        Make a method to link buttons together
    Support using an image to the left of the text (or as the whole button)
    Support having focus of the keyboard
    Support using an image for the button backgrounds
    Integrate with guilib
*/

// Widget that is a clickable button with settings to change its behavior
class Button: public sf::Drawable
{
    using CallbackType = std::function<void(Button&)>;

    public:
        enum class Mode
        {
            Normal, // Temporary push-button
            Toggle, // Toggle switch or a checkbox
            Sticky  // Tabs or radio buttons (Once clicked, can't be undone)
        };

        Button();

        // These should be called in a loop
        void handleMouseEvent(const sf::Event& event, const sf::Vector2f& pos);
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
        void setMode(Mode m);
        void linkButtons(std::vector<Button>& buttons);
            // This should use some sort of messaging system instead

        // Widget state
        void setText(const std::string& str = "");
        std::string getText() const;
        void setPressed(bool state);
        bool isPressed() const;
        void setPressedCallback(CallbackType callback);

    private:
        void updateColors();
        void updateTextPosition();
        void unpressLinkedButtons();

        bool hovered;
        bool pressed; // Button is actually in a different state
        bool beingPressed; // Button is just being held down by the user
        Mode mode;

        sf::RectangleShape box;
        sf::Text text;
        sf::FloatRect collisionBox;
        sf::Color fillColors[2];
        sf::Color outlineColors[2];

        CallbackType pressedCallback;

        std::vector<Button>* linkedButtons;
};

#endif
