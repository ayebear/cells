#include "tool.h"

Tool::Tool()
{
    currentTool = Paint;
    changingSelection = false;
    squareSelection = false;

    // Setup tool colors
    toolColors[Paint] = sf::Color(96, 96, 255, 128);
    toolColors[Copy] = sf::Color(255, 96, 96, 128);
    toolColors[NormalSimulator] = sf::Color(255, 255, 96, 128);
    toolColors[ToroidalSimulator] = sf::Color(96, 255, 96, 128);

    // Setup cursor
    setTool(Paint);
    cursor.setSize(sf::Vector2i(1, 1));
    cursor.setThickness(0.5);
    cursor.setMinimumSize(sf::Vector2u(1, 1));
}

void Tool::setTool(int tool)
{
    currentTool = tool;
    cursor.setColor(toolColors[currentTool]);
}

int Tool::getTool() const
{
    return currentTool;
}

void Tool::updateCursor(const sf::Vector2i& pos)
{
    // Only update the cursor position if a selection isn't being made
    if (changingSelection)
        cursor.setCorner(pos, squareSelection);
    else
        cursor.setPosition(sf::Vector2i(pos.x - (cursor.getSize().x / 2), pos.y - (cursor.getSize().y / 2)));
}

void Tool::handleMouseButtonPressed(const sf::Event::MouseButtonEvent& mouseButton, const sf::Vector2i& mousePos)
{
    // Start resizing the cursor
    if ((mouseButton.button == sf::Mouse::Left || mouseButton.button == sf::Mouse::Right) && controlKeyPressed())
    {
        changingSelection = true;
        squareSelection = (mouseButton.button == sf::Mouse::Right);
        cursor.setPosition(mousePos);
        cursor.setCorner(mousePos, squareSelection);
        //std::cout << "mousePos = (" << mousePos.x << ", " << mousePos.y << ")\n";
        //auto size = cursor.getSize();
        //std::cout << "cursor.getSize() = (" << size.x << ", " << size.y << ")\n";
    }
}

bool Tool::handleMouseButtonReleased(const sf::Event::MouseButtonEvent& mouseButton, const sf::Vector2i& mousePos)
{
    bool status = false;
    // Finish resizing the cursor
    if (changingSelection &&
        ((!squareSelection && mouseButton.button == sf::Mouse::Left) ||
        (squareSelection && mouseButton.button == sf::Mouse::Right)))
    {
        cursor.setCorner(mousePos, squareSelection);
        //std::cout << "mousePos = (" << mousePos.x << ", " << mousePos.y << ")\n";
        //auto size = cursor.getSize();
        //std::cout << "cursor.getSize() = (" << size.x << ", " << size.y << ")\n";
        changingSelection = false;
        status = true;
    }
    return status;
}

bool Tool::handleMouseWheelMoved(const sf::Event::MouseWheelEvent& mouseWheel)
{
    bool status = false;
    if (controlKeyPressed())
    {
        // Handle switching tools
        if (mouseWheel.delta > 0)
            setTool(std::min(currentTool + 1, TotalTools - 1));
        else
            setTool(std::max(currentTool - 1, 0));
        status = true;
    }
    return status;
}

void Tool::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    window.draw(cursor);
}

bool Tool::controlKeyPressed() const
{
    return (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::RControl));
}
