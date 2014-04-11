// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "cells.h"
#include <iostream>

const char* Cells::title = "Cells v0.4.0 Alpha";

const cfg::File::ConfigMap Cells::defaultOptions = {
    {"Window",{
        {"fullscreen", cfg::makeOption(false)},
        {"width", cfg::makeOption(640, 0)},
        {"height", cfg::makeOption(480, 0)},
        {"vsync", cfg::makeOption(true)}
        }
    },
    {"Board",{
        {"width", cfg::makeOption(640, 1)},
        {"height", cfg::makeOption(480, 1)},
        {"rules", cfg::makeOption(Board::defaultRuleString)},
        {"autosave", cfg::makeOption(true)}
        }
    },
    {"",{
        {"autosaveConfig", cfg::makeOption(true)}
        }
    }
};

Cells::Cells():
    config(defaultOptions),
    gui(board, config)
{
    config.loadFromFile("cells.cfg");
    // Set board settings from config file
    config.useSection("Board");
    // Setup some default colors if none exist
    auto& colorOpt = config("colors");
    if (colorOpt.size() == 0)
    {
        colorOpt.push() = "#000";
        colorOpt.push() = "#FFF";
    }
    // Set the colors and rules
    board.setColors(colorOpt);
    board.setRules(config("rules").toString());
    currentPresetRule = 0;

    // Load the last board or make a new one of the configured size
    bool status = false;
    if (config("autosave").toBool())
        status = board.loadFromFile("board");
    if (!status)
        board.resize(config("width").toInt(), config("height").toInt(), false);

    // Set booleans
    running = false;
    hasFocus = true;
    panning = false;
    mouseMoved = false;
    mouseClicked = false;
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

    gui.loadSettings();

    createWindow();
}

Cells::~Cells()
{
    if (config("autosaveConfig", "").toBool())
    {
        gui.saveSettings();
        config.useSection("Board");

        // Save rules
        config("rules") = board.getRules();

        // Re-parse preset rules
        for (auto& opt: config("presetRules"))
        {
            board.setRules(opt.toString());
            opt = board.getRules();
        }

        // Save colors
        config("colors").clear();
        auto colorStrings = board.getColorStrings();
        for (auto& str: colorStrings)
            config("colors").push() = str;

        if (config("autosave").toBool())
            board.saveToFile("board");
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

            case sf::Event::Resized:
                handleWindowResized(event.size);
                break;

            default:
                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Escape)
                        gui.toggle();
                    else if (event.key.code == sf::Keyboard::F4 && event.key.alt)
                        running = false;
                }
                if (gui.isVisible())
                {
                    // Have the GUI handle events
                    if (event.type == sf::Event::MouseButtonPressed ||
                        event.type == sf::Event::MouseButtonReleased ||
                        event.type == sf::Event::MouseMoved)
                    {
                        sf::Vector2i pos;
                        if (event.type == sf::Event::MouseMoved)
                            pos = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
                        else
                            pos = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
                        sf::Vector2f mappedPos = window.mapPixelToCoords(pos);
                        gui.handleMouseEvent(event, mappedPos);
                    }
                    else
                        gui.handleEvent(event);
                }
                // Have the program handle events
                if (!board.setBoardState(!gui.hasFocus()) && !gui.hasFocus())
                    handleOtherEvent(event);
                break;
        }
    }
    if (!gui.hasFocus())
        handleInput();
}

void Cells::handleOtherEvent(const sf::Event& event)
{
    switch (event.type)
    {
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

        default:
            break;
    }
}

void Cells::update()
{
    board.update();
    board.updateTexture();
    if (gui.isVisible())
        gui.update();
    /*if (fpsTimer.getElapsedTime().asMilliseconds() >= 500)
    {
        window.setTitle(strlib::toString(1 / elapsedTime));
        fpsTimer.restart();
    }*/
}

void Cells::draw()
{
    window.clear(board.getFirstColor());

    window.setView(boardView);
    window.draw(board);
    if (!gui.hasFocus())
        window.draw(cursor);

    window.setView(uiView);
    window.draw(gui);

    window.display();
}

void Cells::createWindow()
{
    // Read settings from config file
    config.useSection("Window");
    int width = config("width").toInt();
    int height = config("height").toInt();
    bool fullscreen = config("fullscreen").toBool();
    bool vsync = config("vsync").toBool();
    config.useSection();

    // Get the proper video mode and style
    sf::VideoMode vidMode;
    if (width > 0 && height > 0)
        vidMode = sf::VideoMode(width, height);
    else
        vidMode = sf::VideoMode::getDesktopMode();
    auto style = (fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
    windowSize.x = width;
    windowSize.y = height;

    // Set anti-aliasing
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
    boardView.setCenter(board.width() / 2, board.height() / 2);
    uiView.setCenter(gui.getWidth() / 2, uiView.getCenter().y);
    //uiView.move(-gui.getWidth() / 2, 0);
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
            boardView.setSize(windowSize.x, windowSize.y); // Reset zoom to 1:1
            break;

        case sf::Keyboard::M:
            if (!panning)
                boardView.setCenter(sf::Vector2f(board.width() / 2, board.height() / 2)); // Reset the view's position to the center
            break;

        case sf::Keyboard::C:
            board.clear(); // Clear the board
            break;

        case sf::Keyboard::N:
            board.play();
            gui.updatePlayButton();
            break;

        case sf::Keyboard::Y:
            board.saveToImageFile("board.png"); // Save a screenshot
            break;

        case sf::Keyboard::Q:
            --currentPresetRule;
            loadPresetRule();
            break;

        case sf::Keyboard::W:
            ++currentPresetRule;
            loadPresetRule();
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
        //std::cout << "mousePos = (" << mousePos.x << ", " << mousePos.y << ")\n";
        //auto size = cursor.getSize();
        //std::cout << "cursor.getSize() = (" << size.x << ", " << size.y << ")\n";
    }
    mouseClicked = true;
}

void Cells::handleMouseButtonReleased(const sf::Event::MouseButtonEvent& mouseButton)
{
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
    }
    else if (mouseButton.button == sf::Mouse::Left || mouseButton.button == sf::Mouse::Right)
        mouseClicked = true;
}

void Cells::handleMouseMoved(const sf::Event::MouseMoveEvent& mouseMove)
{
    mouseMoved = true;
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
        int offset = (static_cast<int>(size.width) - gui.getWidth()) / 2;
        uiView.reset(sf::FloatRect(-offset, 0, size.width, size.height));
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
        if (!changingSelection && (mouseClicked || mouseMoved))
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                handleMouseClick(true);
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                handleMouseClick(false);
            else if (mouseClicked)
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

    mouseMoved = false;
    mouseClicked = false;
}

void Cells::handleKeyPanning()
{
    int deltaX = 0, deltaY = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        deltaY -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        deltaY += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        deltaX -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        deltaX += 1;
    if (deltaY != 0 || deltaX != 0)
    {
        const auto& boardViewSize = boardView.getSize();
        float offset = (boardViewSize.x * elapsedTime + boardViewSize.y * elapsedTime) / 4;
        boardView.move(deltaX * offset, deltaY * offset);
        mouseMoved = true;
    }
}

bool Cells::updateMousePos()
{
    // Get the mouse position
    sf::Vector2f newMousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window), boardView);
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
        cursor.setPosition(sf::Vector2i(pos.x - (cursor.getSize().x / 2), pos.y - (cursor.getSize().y / 2)));
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
                board.pasteBlock(cursor.getPosition());
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

void Cells::loadPresetRule()
{
    auto& presetRules = config("presetRules", "Board");
    if (presetRules.size() >= 1)
    {
        if (currentPresetRule < 0)
            currentPresetRule = presetRules.size() - 1;
        else if (currentPresetRule > static_cast<int>(presetRules.size() - 1))
            currentPresetRule = 0;
        gui.setRules(presetRules[currentPresetRule].toString());
    }
}
