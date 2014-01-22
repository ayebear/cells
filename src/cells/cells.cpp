// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "cells.h"
#include <iostream>

const ConfigFile::ConfigMap Cells::defaultOptions = {
    {"Window",{
        {"fullscreen", makeOption(false)},
        {"width", makeOption(512, 0)},
        {"height", makeOption(512, 0)},
        {"vsync", makeOption(true)}
        }
    },
    {"Board",{
        {"width", makeOption(512, 1)},
        {"height", makeOption(512, 1)},
        {"rules", makeOption<std::string>(Board::defaultRuleString)}
        }
    }
};

Cells::Cells(const std::string& title):
    config("cells.cfg", defaultOptions)
{
    config.setSection("Board");
    board.resize(config["width"].asInt(), config["height"].asInt(), false);
    board.setRules(config["rules"].asString());
    running = false;
    hasFocus = true;
    simulating = false;
    panning = false;
    setupShapes();
    createWindow(title);
}

Cells::~Cells()
{
    config.setSection("Board");
    config["rules"] = board.getRules();
    config.writeToFile();
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
    window.draw(border);
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
            updateBorderSize(); // Change the border size in case the board changed size
            break;

        case sf::Keyboard::N:
            simulating = !simulating; // Toggle simulating
            break;

        case sf::Keyboard::Y:
            board.saveToImageFile("board.png"); // Save a screenshot
            break;

        case sf::Keyboard::Q:
            board.copyBlock(sf::IntRect(mousePos.x, mousePos.y, 128, 128));
            break;

        case sf::Keyboard::W:
            board.pasteBlock(mousePos);
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
}

void Cells::handleMouseButtonReleased(const sf::Event::MouseButtonEvent& mouseButton)
{
}

void Cells::handleMouseMoved(const sf::Event::MouseMoveEvent& mouseMove)
{
}

void Cells::handleMouseWheelMoved(const sf::Event::MouseWheelEvent& mouseWheel)
{
    sf::Vector2f currentCenter = boardView.getCenter();
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
        // Mouse input
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            board.paintLine(mousePos, true);
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            board.paintLine(mousePos, false);
        else
            board.finishLine();

        // Simulating
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            board.simulate();

        // Zooming
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
            boardView.zoom(1 / (1 + elapsedTime));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))
            boardView.zoom(1 + elapsedTime);

        // Panning (arrow keys)
        if (!panning)
        {
            float offset = (boardView.getSize().x * elapsedTime + boardView.getSize().y * elapsedTime) / 4;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                boardView.move(0, -offset);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                boardView.move(0, offset);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                boardView.move(-offset, 0);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                boardView.move(offset, 0);
        }
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

void Cells::setupShapes()
{
    // Normal cursor
    cursor.setPosition(0, 0);
    cursor.setSize(sf::Vector2f(1, 1));
    cursor.setFillColor(sf::Color::Transparent);
    cursor.setOutlineColor(sf::Color(0, 0, 255, 128));
    cursor.setOutlineThickness(0.5);

    // Selection cursor
    selectionCursor.setPosition(0, 0);
    selectionCursor.setSize(sf::Vector2f(1, 1));
    selectionCursor.setFillColor(sf::Color::Transparent);
    selectionCursor.setOutlineColor(sf::Color(0, 0, 255, 128));
    selectionCursor.setOutlineThickness(1); // Maybe make this -1

    // Border
    border.setPosition(0, 0);
    updateBorderSize();
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color::Green);
    border.setOutlineThickness(1);
}

void Cells::updateCursor(const sf::Vector2i& pos)
{
    cursor.setPosition(pos.x, pos.y);
    selectionCursor.setPosition(pos.x, pos.y);
}

void Cells::updateBorderSize()
{
    border.setSize(sf::Vector2f(board.width(), board.height()));
}
