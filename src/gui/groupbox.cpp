#include "groupbox.h"
#include "colorcode.h"

GroupBox::GroupBox()
{
    setFontSize();
    setOutlineThickness();
    setColor("outline", "#FFF9");
    setColor("fill", sf::Color::Transparent);
    setColor("text", sf::Color::White);
}

void GroupBox::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    window.draw(box);
    window.draw(underline);
    window.draw(text);
}

void GroupBox::setColor(const std::string& setting, const std::string& color)
{
    setColor(setting, ColorCode(color).toColor());
}

void GroupBox::setColor(const std::string& setting, const sf::Color& color)
{
    if (setting == "outline")
    {
        box.setOutlineColor(color);
        underline.setOutlineColor(color);
    }
    else if (setting == "fill")
    {
        box.setFillColor(color);
        underline.setFillColor(color);
    }
    else if (setting == "text")
        text.setColor(color);
}

void GroupBox::setFont(const sf::Font& f)
{
    text.setFont(f);
}

void GroupBox::setFontSize(unsigned size)
{
    text.setCharacterSize(size);
}

void GroupBox::setOutlineThickness(float thickness)
{
    box.setOutlineThickness(thickness);
    underline.setOutlineThickness(thickness / 2);
}

void GroupBox::setPosition(const sf::Vector2f& pos)
{
    box.setPosition(pos);
    updateTextPosition();
    updateLine();
}

void GroupBox::setSize(const sf::Vector2u& size)
{
    box.setSize(sf::Vector2f(size.x, size.y));
    updateTextPosition();
    updateLine();
}

void GroupBox::setText(const std::string& str)
{
    text.setString(str);
    updateTextPosition();
}

void GroupBox::updateTextPosition()
{
    sf::Vector2f pos = box.getPosition();
    text.setPosition(pos);
    float offset = ((pos.x + box.getSize().x) - text.findCharacterPos(-1).x) / 2;
    text.setPosition(pos.x + offset, pos.y);
}

void GroupBox::updateLine()
{
    sf::Vector2f pos = box.getPosition();
    sf::Vector2f size = box.getSize();
    unsigned fontSize = text.getCharacterSize();
    underline.setPosition(pos.x, pos.y + fontSize + 5);
    underline.setSize(sf::Vector2f(size.x, 0));
}
