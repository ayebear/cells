#include "button.h"
#include "colorcode.h"

Button::Button()
{
    hovered = false;
    pressed = false;
    mode = Mode::Normal;
    setFontSize();
    setOutlineThickness();
    setColor("outline", "#FFFC");
    setColor("fill", "#444C");
    setColor("text", "#FFF");
    setColor("outlinePressed", "#000C");
    setColor("fillHovered", "#888C");
    updateColors();
}

void Button::handleEvent(const sf::Event& event)
{

}

void Button::handleMouseEvent(const sf::Event& event, const sf::Vector2f& pos)
{
    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (collisionBox.contains(pos))
            setPressed(true);
    }
    else if (event.type == sf::Event::MouseButtonReleased)
    {
        setPressed(false);
        if (pressedCallback && collisionBox.contains(pos))
            pressedCallback(*this);
    }
    else if (event.type == sf::Event::MouseMoved)
    {
        hovered = collisionBox.contains(pos);
        updateColors();
    }
}

void Button::update()
{

}

void Button::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    window.draw(box);
    window.draw(text);
}

void Button::setColor(const std::string& setting, const std::string& color)
{
    setColor(setting, ColorCode(color).toColor());
}

void Button::setColor(const std::string& setting, const sf::Color& color)
{
    if (setting == "outline")
        outlineColors[0] = color;
    else if (setting == "fill")
        fillColors[0] = color;
    else if (setting == "text")
        text.setColor(color);
    else if (setting == "outlinePressed")
        outlineColors[1] = color;
    else if (setting == "fillHovered")
        fillColors[1] = color;
}

void Button::setFont(const sf::Font& f)
{
    text.setFont(f);
}

void Button::setFontSize(unsigned size)
{
    text.setCharacterSize(size);
}

void Button::setOutlineThickness(float thickness)
{
    box.setOutlineThickness(thickness);
}

void Button::setPosition(const sf::Vector2f& pos)
{
    box.setPosition(pos);
    collisionBox.left = pos.x;
    collisionBox.top = pos.y;
    updateTextPosition();
}

void Button::setSize(const sf::Vector2u& size)
{
    box.setSize(sf::Vector2f(size.x, size.y));
    collisionBox.width = size.x;
    collisionBox.height = size.y;
    updateTextPosition();
}

void Button::setMode(Mode m)
{
    mode = m;
}

void Button::setText(const std::string& str)
{
    text.setString(str);
    updateTextPosition();
}

std::string Button::getText() const
{
    return text.getString().toAnsiString();
}

void Button::setPressed(bool state)
{
    pressed = state;
    updateColors();
}

bool Button::isPressed() const
{
    return pressed;
}

void Button::setPressedCallback(CallbackType callback)
{
    pressedCallback = callback;
}

void Button::updateColors()
{
    box.setFillColor(fillColors[hovered]);
    box.setOutlineColor(outlineColors[pressed]);
}

void Button::updateTextPosition()
{
    sf::Vector2f pos = box.getPosition();
    text.setPosition(pos);
    float offset = ((pos.x + box.getSize().x) - text.findCharacterPos(-1).x) / 2;
    text.setPosition(pos.x + offset, pos.y);
}
