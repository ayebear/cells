#include "rulegrid.h"
#include "strlib.h"
#include <iostream>

const sf::Color RuleGrid::colors[] = {
    sf::Color::Black,
    sf::Color::White
};

RuleGrid::RuleGrid():
    grid(sf::Lines, gridArraySize)
{
    mouseState = NotPressed;
    image.create(width, height);
    texture.loadFromImage(image);
    sprite.setTexture(texture);
    setSize();
    headerBg.setFillColor(sf::Color(64, 64, 64, 192));
    description.setString("B: Birth, S: Survival");
    headers.resize(width);
    for (unsigned i = 0; i < width; ++i)
        headers[i].setString(strlib::toString(i, 0));
    for (unsigned i = 0; i < gridArraySize; ++i)
        grid[i].color = sf::Color(192, 192, 192);
}

void RuleGrid::setFont(const sf::Font& f, unsigned fontSize)
{
    for (auto& text: headers)
    {
        text.setFont(f);
        text.setCharacterSize(fontSize);
    }
    description.setFont(f);
    description.setCharacterSize(fontSize);
}

void RuleGrid::setPosition(const sf::Vector2f& pos)
{
    position = pos;
    sprite.setPosition(position.x, position.y + 20);
    description.setPosition(position.x, position.y + 56);
    headerBg.setPosition(position);
    collisionBox.left = position.x;
    collisionBox.top = position.y + 20;
    updateHeaderPositions();
    generateGrid();
}

void RuleGrid::setSize(int s)
{
    size = s;
    sprite.setScale(size, size);
    headerBg.setSize(sf::Vector2f(size * width, size + 4));
    collisionBox.width = size * width;
    collisionBox.height = size * 2;
}

void RuleGrid::setRules(const RuleSet& rules)
{
    for (unsigned ruleType = 0; ruleType < height; ++ruleType) // Birth/survival
    {
        for (unsigned count = 0; count < width; ++count)
            image.setPixel(count, ruleType, colors[rules.getRule(ruleType, count)]);
    }
    texture.loadFromImage(image);
    sprite.setTexture(texture);
}

bool RuleGrid::handleMouseEvent(const sf::Event& event, const sf::Vector2f& pos, RuleSet& rules)
{
    bool status = false;
    if (event.type == sf::Event::MouseButtonPressed)
    {
        updateMouseState(event.mouseButton.button);
        status = modifyRule(rules, pos);
    }
    else if (event.type == sf::Event::MouseButtonReleased)
        mouseState = NotPressed;
    else if (event.type == sf::Event::MouseMoved)
        status = modifyRule(rules, pos);
    return status;
}

void RuleGrid::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    window.draw(headerBg);
    window.draw(sprite);
    window.draw(grid);
    for (auto& text: headers)
        window.draw(text);
    window.draw(description);
}

void RuleGrid::updateHeaderPositions()
{
    for (unsigned i = 0; i < width; ++i)
        headers[i].setPosition(position.x + (i * size) + 4, position.y);
}

void RuleGrid::generateGrid()
{
    // Generate columns
    for (unsigned col = 0; col <= width; ++col)
    {
        unsigned x = position.x + (col * size);
        grid[col * 2].position = sf::Vector2f(x, position.y);
        grid[col * 2 + 1].position = sf::Vector2f(x, position.y + (size * 3) + 4);
    }
    // Generate rows
    grid[20].position = sf::Vector2f(position.x, position.y);
    grid[21].position = sf::Vector2f(position.x + (width * size), position.y);
    for (unsigned row = 1; row < 4; ++row)
    {
        unsigned y = position.y + (row * size) + 4;
        grid[20 + (row * 2)].position = sf::Vector2f(position.x, y);
        grid[21 + (row * 2)].position = sf::Vector2f(position.x + (width * size), y);
    }
}

void RuleGrid::updateMouseState(sf::Mouse::Button button)
{
    if (button == sf::Mouse::Left)
        mouseState = LeftPressed;
    else if (button == sf::Mouse::Right)
        mouseState = RightPressed;
    else
        mouseState = NotPressed;
}

bool RuleGrid::modifyRule(RuleSet& rules, const sf::Vector2f& pos)
{
    bool status = false;
    if (mouseState != NotPressed && collisionBox.contains(pos))
    {
        sf::Vector2u rulePos((pos.x - collisionBox.left) / size, (pos.y - collisionBox.top) / size);
        if (rulePos.x < width && rulePos.y < height)
        {
            bool state = (mouseState != 0);
            rules.setRule(rulePos.y, rulePos.x, state);
            image.setPixel(rulePos.x, rulePos.y, colors[state]);
            texture.loadFromImage(image);
            sprite.setTexture(texture);
            status = true;
        }
    }
    return status;
}
