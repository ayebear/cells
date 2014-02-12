// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "cells.h"
#include <iostream>

const ConfigFile::ConfigMap Cells::defaultOptions = {
    {"Window",{
        {"fullscreen", makeOption(false)},
        {"width", makeOption(640, 0)},
        {"height", makeOption(480, 0)},
        {"vsync", makeOption(true)}
        }
    },
    {"Board",{
        {"width", makeOption(640, 1)},
        {"height", makeOption(480, 1)},
        {"rules", makeOption<std::string>(Board::defaultRuleString)},
        {"colors", makeOption<std::string>("{#000,#FFF}")},
        {"saveOnExit", makeOption(true)}
        }
    }
};

Cells::Cells(const std::string& title):
    config("cells.cfg", defaultOptions)
{
    // Set board settings from config file
    config.setSection("Board");
    board.setColors(StringUtils::splitArrayString(config["colors"].asString()));
    board.setRules(config["rules"].asString());
    board.resize(config["width"].asInt(), config["height"].asInt(), false);

    // Set booleans
    running = false;
    hasFocus = true;
    simulating = false;
    panning = false;
    mouseMovedOrClicked = false;
    changingSelection = false;
    squareSelection = false;
    
    // Setup tool colors
    toolColors[Tool::Paint] = sf::Color(96, 96, 255, 128);
    toolColors[Tool::Copy] = sf::Color(255, 96, 96, 128);
    toolColors[Tool::NormalSimulator] = sf::Color(255, 255, 96, 128);
    toolColors[Tool::ToroidalSimulator] = sf::Color(96, 255, 96, 128);
    
    // Setup cursor
    setTool(Tool::Paint);
    cursor.setSize(sf::Vector2i(1, 1));
    cursor.setThickness(0.5);
    cursor.setMinimumSize(sf::Vector2u(1, 1));

    createWindow(title);
}

Cells::~Cells()
{
    config.setSection("Board");
    if (config["saveOnExit"].asBool())
    {
        config["rules"] = board.getRules();
        config["colors"] = StringUtils::joinArrayString(board.getColorStrings());
        config.writeToFile();
    }
    window.close();
}

void Cells::start()
{
    running = true;
    sf::Clock clock;
    while (running && window.isOpen())
    {
        elapsedTime = clock.restart().asSeconds();
        handleEvents();
        update();
        draw();
    }
}

void Cells::handleEvents()
{
    // This isn't very OOP, use an event handler class or something
    // That will allow for customizable controls as well

    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                running = false;
                break;

            case sf::Event::LostFocus:
                hasFocus = false;
                break;

            case sf::Event::GainedFocus:
                hasFocus = true;
                break;

            case sf::Event::KeyPressed:
                handleKeyPressed(event.key);
                break;

            case sf::Event::MouseButtonPressed:
                handleMouseButtonPressed(event.mouseButton);
                break;

            case sf::Event::MouseButtonReleased:
                handleMouseButtonReleased(event.mouseButton);
                break;

            case sf::Event::MouseMoved:
                handleMouseMoved(event.mouseMove);
                break;

            case sf::Event::MouseWheelMoved:
                handleMouseWheelMoved(event.mouseWheel);
                break;

            case sf::Event::Resized:
                handleWindowResized(event.size);
                break;

            default:
                break;
        }
    }
    handleInput();
}

void Cells::update()
{
    if (simulating)
        board.simulate();
    board.updateTexture();
}

void Cells::draw()
{
    window.clear();

    window.setView(boardView);
    window.draw(board);
    window.draw(cursor);

    // Will need this once there is GUI stuff
    //window.setView(uiView);

    window.display();
}

void Cells::createWindow(const std::string& title)
{
    // Read settings from config file
    config.setSection("Window");
    int width = config["width"].asInt();
    int height = config["height"].asInt();
    bool fullscreen = config["fullscreen"].asBool();
    bool vsync = config["vsync"].asBool();
    config.setSection();

    // Get the proper video mode and style
    sf::VideoMode vidMode;
    if (width > 0 && height > 0)
        vidMode = sf::VideoMode(width, height);
    else
        vidMode = sf::VideoMode::getDesktopMode();
    auto style = (fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
    windowSize.x = width;
    windowSize.y = height;

    // This is temporary
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    // Create the window
    window.create(vidMode, title, style, settings);

    // Use vsync
    window.setVerticalSyncEnabled(vsync);
    //window.setFramerateLimit(500);

    // Update the views
    boardView = window.getDefaultView();
    uiView = boardView;
}

void Cells::handleKeyPressed(const sf::Event::KeyEvent& key)
{
    switch (key.code)
    {
        case sf::Keyboard::Return:
            board.simulate(); // Run a simulation
            break;

        case sf::Keyboard::Num1:
            setTool(Tool::Paint);
            break;

        case sf::Keyboard::Num2:
            setTool(Tool::Copy);
            break;

        case sf::Keyboard::Num3:
            setTool(Tool::NormalSimulator);
            break;

        case sf::Keyboard::Num4:
            setTool(Tool::ToroidalSimulator);
            break;

        case sf::Keyboard::I:
            boardView.zoom(0.5); // Zoom in 2x
            break;

        case sf::Keyboard::O:
            boardView.zoom(2); // Zoom out 2x
            break;

        case sf::Keyboard::R:
            // Reset zoom to 1:1
            //sf::Vector2f windowSize = window.getSize();
            //boardView.setSize();
            break;

        case sf::Keyboard::M:
            if (!panning)
                boardView.setCenter(sf::Vector2f(board.width() / 2, board.height() / 2)); // Reset the view's position to the center
            break;

        case sf::Keyboard::C:
            board.clear(); // Clear the board
            break;

        case sf::Keyboard::S:
            board.saveToFile("board"); // Save the board to a file
            // TODO: Show an inputbox where the user can type the filename they want to save it as
            break;

        case sf::Keyboard::L:
            board.loadFromFile("board"); // Load the board from a file
            break;

        case sf::Keyboard::N:
            simulating = !simulating; // Toggle simulating
            break;

        case sf::Keyboard::Y:
            board.saveToImageFile("board.png"); // Save a screenshot
            break;

        case sf::Keyboard::Escape:
            running = false; // This should probably open a menu instead, and alt-f4 should close the program
            break;

        default:
            break;
    }
}

void Cells::handleMouseButtonPressed(const sf::Event::MouseButtonEvent& mouseButton)
{
    // Start resizing the cursor
    if ((mouseButton.button == sf::Mouse::Left || mouseButton.button == sf::Mouse::Right) && controlKeyPressed())
    {
        changingSelection = true;
        squareSelection = (mouseButton.button == sf::Mouse::Right);
        cursor.setPosition(mousePos);
        cursor.setCorner(mousePos, squareSelection);
        std::cout << "mousePos = (" << mousePos.x << ", " << mousePos.y << ")\n";
        auto size = cursor.getSize();
        std::cout << "cursor.getSize() = (" << size.x << ", " << size.y << ")\n";
    }
    mouseMovedOrClicked = true;
}

void Cells::handleMouseButtonReleased(const sf::Event::MouseButtonEvent& mouseButton)
{
    // Finish resizing the cursor
    if (changingSelection &&
        ((!squareSelection && mouseButton.button == sf::Mouse::Left) ||
        (squareSelection && mouseButton.button == sf::Mouse::Right)))
    {
        cursor.setCorner(mousePos, squareSelection);
        std::cout << "mousePos = (" << mousePos.x << ", " << mousePos.y << ")\n";
        auto size = cursor.getSize();
        std::cout << "cursor.getSize() = (" << size.x << ", " << size.y << ")\n";
        changingSelection = false;
    }
}

void Cells::handleMouseMoved(const sf::Event::MouseMoveEvent& mouseMove)
{
    mouseMovedOrClicked = true;
    // TODO: Maybe separate this into 2 variables
}

void Cells::handleMouseWheelMoved(const sf::Event::MouseWheelEvent& mouseWheel)
{
    sf::Vector2f currentCenter = boardView.getCenter();
    if (controlKeyPressed())
    {
        // Handle switching tools
        if (mouseWheel.delta > 0)
            setTool(std::min(currentTool + 1, TotalTools - 1));
        else
            setTool(std::max(currentTool - 1, 0));
    }
    else
    {
        // Handle zooming
        if (mouseWheel.delta > 0)
        {
            boardView.zoom(0.8);
            boardView.setCenter((currentCenter.x * 3 + mousePos.x) / 4, (currentCenter.y * 3 + mousePos.y) / 4);
        }
        else
        {
            boardView.zoom(1.25);
            boardView.setCenter((currentCenter.x * 5 - mousePos.x) / 4, (currentCenter.y * 5 - mousePos.y) / 4);
        }
    }
}

void Cells::handleWindowResized(const sf::Event::SizeEvent& size)
{
    // Update the view, to preserve aspect ratio, and show more/less
    if (size.width > 0 && size.height > 0)
    {
        sf::Vector2f oldSize = boardView.getSize();
        boardView.setSize((static_cast<float>(size.width) / windowSize.x) * oldSize.x,
                          (static_cast<float>(size.height) / windowSize.y) * oldSize.y);
        windowSize.x = size.width;
        windowSize.y = size.height;
    }
}

void Cells::handleInput()
{
    sf::Vector2i lastMousePos = mousePos;

    // Update the mouse position
    if (updateMousePos())
    {
        // Panning (middle click)
        if (hasFocus && sf::Mouse::isButtonPressed(sf::Mouse::Middle))
        {
            if (panning)
            {
                sf::Vector2f currentCenter = boardView.getCenter();
                boardView.setCenter(currentCenter.x + (startPanMousePos.x - worldMousePos.x), currentCenter.y + (startPanMousePos.y - worldMousePos.y));
                mousePos = lastMousePos;
            }
            else
            {
                startPanMousePos = worldMousePos;
                panning = true;
            }
        }
        else
            panning = false;

        // Update the cursor sprite position
        updateCursor(mousePos);
    }

    if (hasFocus)
    {
        // Panning (arrow keys)
        if (!panning)
            handleKeyPanning();

        // Mouse input
        if (!changingSelection && mouseMovedOrClicked)
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                handleMouseClick(true);
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                handleMouseClick(false);
            else
                board.finishLine();
        }

        // Simulating
        if (!changingSelection && currentTool >= NormalSimulator && sf::Mouse::isButtonPressed(sf::Mouse::Left))
            board.simulate(cursor.getRect(), (currentTool == ToroidalSimulator));
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            board.simulate();

        // Zooming
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
            boardView.zoom(1 / (1 + elapsedTime));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
            boardView.zoom(1 + elapsedTime);
    }

    mouseMovedOrClicked = false;
}

void Cells::handleKeyPanning()
{
    int deltaX = 0, deltaY = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        deltaY = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        deltaY = 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        deltaX = -1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        deltaX = 1;
    if (deltaY != 0 || deltaX != 0)
    {
        const auto& boardViewSize = boardView.getSize();
        float offset = (boardViewSize.x * elapsedTime + boardViewSize.y * elapsedTime) / 4;
        boardView.move(deltaX * offset, deltaY * offset);
        mouseMovedOrClicked = true;
    }
}

bool Cells::updateMousePos()
{
    // Get the mouse position
    sf::Vector2f newMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    if (newMousePos != worldMousePos)
    {
        worldMousePos = newMousePos;
        mousePos.x = floor(worldMousePos.x);
        mousePos.y = floor(worldMousePos.y);
        //std::cout << "updateMousePos(" << mousePos.x << ", " << mousePos.y << ")\n";
        return true;
    }
    return false;
}

void Cells::updateCursor(const sf::Vector2i& pos)
{
    // Only update the cursor position if a selection isn't being made
    if (changingSelection)
        cursor.setCorner(pos, squareSelection);
    else
        cursor.setPosition(pos);
    // selectionCursor.setPosition(pos.x, pos.y);
}

bool Cells::controlKeyPressed() const
{
    return (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::RControl));
}

void Cells::setTool(int tool)
{
    currentTool = tool;
    cursor.setColor(toolColors[currentTool]);
}

void Cells::handleMouseClick(bool action)
{
    switch (currentTool)
    {
        case Tool::Paint:
            if (cursor.isMinimumSize())
                board.paintLine(mousePos, action);
            else
                board.paintBlock(cursor.getRect(), action);
            break;

        case Tool::Copy:
            if (action)
                board.pasteBlock(mousePos);
            else
                board.copyBlock(cursor.getRect());
            break;

        case Tool::NormalSimulator:
            if (!action) // Right click
                board.simulate(cursor.getRect(), false);
            break;

        case Tool::ToroidalSimulator:
            if (!action) // Right click
                board.simulate(cursor.getRect(), true);
            break;

        default:
            break;
    }
}
