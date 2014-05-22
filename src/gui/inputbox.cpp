// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "inputbox.h"
#include <iostream>
#include <sstream>
#include "colorcode.h"
#include "strlib.h"

InputBox::InputBox()
{
    focus = false;
    setFillMode(false);
    setFontSize();
    setOutlineThickness();
    text.setPosition(1, 0); // Because the cursor is 1 px to the left
    labelText.setPosition(1, 0);
    setColor("outline", "#FFF9");
    setColor("fill", "#3338");
    setColor("text", "#FFF");
    setColor("label", "#CCC");
    setColor("cursor", "#FFFC");
    //setPosition(sf::Vector2f(0, 0));
    //setSize(sf::Vector2u(150, 20));
}

void InputBox::handleEvent(const sf::Event& event)
{
    if (focus)
    {
        if (event.type == sf::Event::TextEntered)
        {
            if (event.text.unicode >= 32 && event.text.unicode <= 126)
                insertChar(static_cast<char>(event.text.unicode));
        }
        else if (event.type == sf::Event::KeyPressed)
            handleKeyPressed(event.key.code);
    }
}

void InputBox::handleMouseEvent(const sf::Event& event, const sf::Vector2f& pos)
{
    // Handle mouse click inside of input box
    if (event.type == sf::Event::MouseButtonPressed)
        setFocus(event.mouseButton.button == sf::Mouse::Left && collisionBox.contains(pos));
}

void InputBox::update()
{
    if (focus)
        cursor.update();

    // Draw the text and cursor to the render texture
    texture.clear(sf::Color::Transparent);
    if (textStr.empty())
        texture.draw(labelText);
    else
        texture.draw(text);
    if (focus)
        texture.draw(cursor);
    texture.display();
}

void InputBox::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    window.draw(box);
    window.draw(sprite);
}

void InputBox::setColor(const std::string& setting, const std::string& color)
{
    setColor(setting, ColorCode(color).toColor());
}

void InputBox::setColor(const std::string& setting, const sf::Color& color)
{
    if (setting == "outline")
        box.setOutlineColor(color);
    else if (setting == "fill")
        box.setFillColor(color);
    else if (setting == "text")
        text.setColor(color);
    else if (setting == "label")
        labelText.setColor(color);
    else if (setting == "cursor")
        cursor.rect.setFillColor(color);
}

void InputBox::setFont(const sf::Font& f)
{
    text.setFont(f);
    labelText.setFont(f);
}

void InputBox::setFontSize(unsigned size)
{
    text.setCharacterSize(size);
    labelText.setCharacterSize(size);
}

void InputBox::setOutlineThickness(float thickness)
{
    box.setOutlineThickness(thickness);
}

void InputBox::setPosition(const sf::Vector2f& pos)
{
    box.setPosition(pos);
    collisionBox.left = pos.x;
    collisionBox.top = pos.y;
    sprite.setPosition(pos);
}

void InputBox::setSize(const sf::Vector2u& size)
{
    box.setSize(sf::Vector2f(size.x, size.y));
    collisionBox.width = size.x;
    collisionBox.height = size.y;
    texture.create(size.x, size.y);
    sprite.setTexture(texture.getTexture(), true);
}

void InputBox::setFillMode(bool mode, char c)
{
    fillMode = mode;
    fillModeChar = c;
}

void InputBox::setLabel(const std::string& str)
{
    labelText.setString(str);
    updateText(false);
}

void InputBox::setText(const std::string& str)
{
    textStr = str;
    updateText(false);
    home();
    end();
}

const std::string& InputBox::getText() const
{
    return textStr;
}

void InputBox::setNumber(double num)
{
    setText(strlib::toString(num));
}

double InputBox::getNumber() const
{
    std::istringstream tmpStream(textStr);
    double number = 0;
    tmpStream >> number;
    return number;
}

void InputBox::setFocus(bool mode)
{
    focus = mode;
    if (focus)
        cursor.restartTimer();
}

bool InputBox::hasFocus() const
{
    return focus;
}

void InputBox::setTextEnteredCallback(CallbackType callback)
{
    textEnteredCallback = callback;
}

void InputBox::insertChar(char c)
{
    cursor.restartTimer();
    if (textStr.empty() || cursor.pos == -1)
    {
        textStr += c;
        cursor.pos = -1;
    }
    else
    {
        textStr.insert(textStr.begin() + cursor.pos, c);
        cursor.pos++;
    }
    updateText();
    updateCursorPos();
}

void InputBox::backspace()
{
    cursor.restartTimer();
    if (!textStr.empty() && cursor.pos != 0)
    {
        if (cursor.pos == -1)
            textStr.pop_back();
        else
        {
            cursor.pos--;
            textStr.erase(textStr.begin() + cursor.pos);
        }
        updateText();
        updateCursorPos();
    }
}

void InputBox::del()
{
    cursor.restartTimer();
    if (!textStr.empty() && cursor.pos != -1 && cursor.pos < (int)textStr.size())
    {
        textStr.erase(textStr.begin() + cursor.pos);
        updateText();
        updateCursorPos();
    }
}

void InputBox::left()
{
    cursor.restartTimer();
    if (cursor.pos == -1)
    {
        cursor.pos = textStr.size() - 1;
        updateCursorPos();
    }
    else if (cursor.pos != 0)
    {
        cursor.pos--;
        updateCursorPos();
    }
}

void InputBox::right()
{
    cursor.restartTimer();
    if (cursor.pos != -1)
    {
        cursor.pos++;
        if (cursor.pos >= (int)textStr.size())
            cursor.pos = -1;
        updateCursorPos();
    }
}

void InputBox::home()
{
    cursor.restartTimer();
    cursor.pos = 0;
    updateCursorPos();
}

void InputBox::end()
{
    cursor.restartTimer();
    cursor.pos = -1;
    updateCursorPos();
}

void InputBox::handleKeyPressed(const sf::Keyboard::Key& keyCode)
{
    switch (keyCode)
    {
        case sf::Keyboard::BackSpace:
            backspace();
            break;
        case sf::Keyboard::Delete:
            del();
            break;
        case sf::Keyboard::Left:
            left();
            break;
        case sf::Keyboard::Right:
            right();
            break;
        case sf::Keyboard::Home:
            home();
            break;
        case sf::Keyboard::End:
            end();
            break;
        default:
            break;
    }
}

void InputBox::updateText(bool invoke)
{
    if (fillMode)
        text.setString(std::string(textStr.size(), fillModeChar));
    else
        text.setString(textStr);
    if (invoke && textEnteredCallback)
        textEnteredCallback(*this);
}

void InputBox::updateCursorPos()
{
    auto cursorPos = text.findCharacterPos(cursor.pos);
    cursorPos.x -= 1;
    cursorPos.y += 2;
    if (cursorPos.x > collisionBox.width - 2)
    {
        text.setPosition(text.getPosition().x + collisionBox.width - cursorPos.x - 2, text.getPosition().y);
        cursorPos.x = collisionBox.width - 2;
    }
    else if (cursorPos.x < 0)
    {
        text.setPosition(text.getPosition().x - cursorPos.x, text.getPosition().y);
        cursorPos.x = 0;
    }
    cursor.rect.setPosition(cursorPos.x, cursorPos.y);
}
