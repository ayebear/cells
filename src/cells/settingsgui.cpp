// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "settingsgui.h"
#include <algorithm>
#include "board.h"
#include "tool.h"

SettingsGUI::SettingsGUI(Board& board, cfg::File& config, Tool& tool, CallbackType callback):
    board(board),
    config(config),
    tool(tool),
    showGridCallback(callback)
{
    using std::placeholders::_1;
    visible = false;
    setFocus(false);
    width = 736;
    height = 146;
    int top = 32;
    int left = 10;
    sf::Vector2u groupSize(156, height - 8);
    int maxElemWidth = groupSize.x - 12;
    int halfElemWidth = (maxElemWidth / 2) - 4;
    int elemHeight = 20;
    font.loadFromFile("data/fonts/Ubuntu-B.ttf");

    // Groups and panels

    panel.setFillColor(sf::Color(64, 64, 64, 128));
    panel.setOutlineThickness(2);
    panel.setSize(sf::Vector2f(width, height));
    panel.setPosition(0, 0);
    collisionBox = sf::FloatRect(0, 0, width, height);

    ruleGroup.setFont(font);
    ruleGroup.setText("Rules");
    ruleGroup.setPosition(sf::Vector2f(4, 4));
    ruleGroup.setSize(groupSize);

    boardGroup.setFont(font);
    boardGroup.setText("Board");
    boardGroup.setPosition(sf::Vector2f(groupSize.x + 10, 4));
    boardGroup.setSize(groupSize);

    colorGroup.setFont(font);
    colorGroup.setText("Colors");
    colorGroup.setPosition(sf::Vector2f((groupSize.x + 6) * 2 + 4, 4));
    colorGroup.setSize(groupSize);

    simulationGroup.setFont(font);
    simulationGroup.setText("Simulation");
    simulationGroup.setPosition(sf::Vector2f((groupSize.x + 6) * 3 + 4, 4));
    simulationGroup.setSize(groupSize);

    toolGroup.setFont(font);
    toolGroup.setText("Tools");
    toolGroup.setPosition(sf::Vector2f((groupSize.x + 6) * 4 + 4, 4));
    toolGroup.setSize(sf::Vector2u(80, height - 8));

    // Rule group

    ruleGrid.setFont(font);
    ruleGrid.setPosition(sf::Vector2f(left, top));

    ruleText.setFont(font);
    ruleText.setPosition(sf::Vector2f(left, top + 84));
    ruleText.setSize(sf::Vector2u(maxElemWidth, elemHeight));
    ruleText.setLabel("Rule String");
    ruleText.setTextEnteredCallback(std::bind(&SettingsGUI::changeRules, this, _1));

    // Board group

    left += groupSize.x + 6;

    boardFilename.setFont(font);
    boardFilename.setPosition(sf::Vector2f(left, top));
    boardFilename.setSize(sf::Vector2u(maxElemWidth, elemHeight));
    boardFilename.setLabel("Board Filename");

    buttons["load"].setup(font, sf::Vector2f(left, top + 28), sf::Vector2u(halfElemWidth, elemHeight), "Load");
    buttons["load"].setPressedCallback(std::bind(&SettingsGUI::loadBoard, this, _1));

    buttons["save"].setup(font, sf::Vector2f(left + 76, top + 28), sf::Vector2u(halfElemWidth, elemHeight), "Save");
    buttons["save"].setPressedCallback(std::bind(&SettingsGUI::saveBoard, this, _1));

    widthInput.setFont(font);
    widthInput.setPosition(sf::Vector2f(left, top + 28 * 2));
    widthInput.setSize(sf::Vector2u(halfElemWidth, elemHeight));
    widthInput.setLabel("Width");

    heightInput.setFont(font);
    heightInput.setPosition(sf::Vector2f(left + 76, top + 28 * 2));
    heightInput.setSize(sf::Vector2u(halfElemWidth, elemHeight));
    heightInput.setLabel("Height");

    buttons["changeSize"].setup(font, sf::Vector2f(left, top + 28 * 3), sf::Vector2u(maxElemWidth, elemHeight), "Change Size");
    buttons["changeSize"].setPressedCallback(std::bind(&SettingsGUI::changeSize, this, _1));

    // Color group

    left += groupSize.x + 6;

    buttons["reverseColors"].setup(font, sf::Vector2f(left, top), sf::Vector2u(maxElemWidth, elemHeight), "Reverse Colors");
    buttons["reverseColors"].setPressedCallback(std::bind(&SettingsGUI::reverseColors, this, _1));
    // The rest are setup during loadSettings()

    // Simulation group

    left += groupSize.x + 6;

    buttons["slow"].setup(font, sf::Vector2f(left, top), sf::Vector2u(halfElemWidth, elemHeight), "Slow");
    buttons["slow"].setPressedCallback(std::bind(&SettingsGUI::changeSpeedPreset, this, _1, 10.0f));
    buttons["medium"].setup(font, sf::Vector2f(left, top + 28), sf::Vector2u(halfElemWidth, elemHeight), "Medium");
    buttons["medium"].setPressedCallback(std::bind(&SettingsGUI::changeSpeedPreset, this, _1, 20.0f));
    buttons["fast"].setup(font, sf::Vector2f(left, top + 28 * 2), sf::Vector2u(halfElemWidth, elemHeight), "Fast");
    buttons["fast"].setPressedCallback(std::bind(&SettingsGUI::changeSpeedPreset, this, _1, 60.0f));
    buttons["play"].setup(font, sf::Vector2f(left + 76, top), sf::Vector2u(halfElemWidth, elemHeight), "Play");
    buttons["play"].setPressedCallback(std::bind(&SettingsGUI::playBoard, this, _1));
    buttons["play"].setMode(Button::Mode::Toggle);
    buttons["clear"].setup(font, sf::Vector2f(left + 76, top + 28), sf::Vector2u(halfElemWidth, elemHeight), "Clear");
    buttons["clear"].setPressedCallback(std::bind(&SettingsGUI::clearBoard, this, _1));
    buttons["random"].setup(font, sf::Vector2f(left + 76, top + 28 * 2), sf::Vector2u(halfElemWidth, elemHeight), "Random");
    buttons["random"].setPressedCallback(std::bind(&SettingsGUI::randomBoard, this, _1));
    buttons["grid"].setup(font, sf::Vector2f(left + 76, top + 28 * 3), sf::Vector2u(halfElemWidth, elemHeight), "Grid");
    buttons["grid"].setPressedCallback(std::bind(&SettingsGUI::updateShowGrid, this, _1));
    buttons["grid"].setMode(Button::Mode::Toggle);

    speedInput.setFont(font);
    speedInput.setPosition(sf::Vector2f(left, top + 28 * 3));
    speedInput.setSize(sf::Vector2u(halfElemWidth, elemHeight));
    speedInput.setLabel("Simulation Speed");
    speedInput.setTextEnteredCallback(std::bind(&SettingsGUI::changeSpeed, this, _1));

    // Tools group

    left += groupSize.x + 6;

    const std::string toolNames[] = {
        "Paint",
        "Copy",
        "Simulate",
        "Toroidal"
    };

    for (int i = 0; i < 4; ++i)
    {
        toolButtons.emplace_back();
        toolButtons.back().setup(font, sf::Vector2f(left, top + i * 28), sf::Vector2u(halfElemWidth, elemHeight), toolNames[i]);
        toolButtons.back().setPressedCallback(std::bind(&SettingsGUI::changeTool, this, _1, i));
        toolButtons.back().setMode(Button::Mode::Sticky);
        toolButtons.back().linkButtons(toolButtons);
    }
}

void SettingsGUI::loadSettings()
{
    ruleGrid.setRules(board.accessRules());
    ruleText.setText(board.getRules());
    boardFilename.setText(config("lastFilename", "Board").toString());
    widthInput.setNumber(board.width());
    heightInput.setNumber(board.height());
    changeSpeedPreset(buttons["fast"], config("speed", "Simulation").toFloat());
    config.useSection("Tool");
    tool.setTool(config("tool").toInt());
    tool.cursor.setSize(config("width").toInt(), config("height").toInt());
    updateToolButtons();
    buttons["grid"].setPressed(config("show", "Grid").toBool());
    unsigned row = 0;
    unsigned col = 0;
    for (const auto& opt: config.getSection("PresetColors"))
    {
        colorButtons.emplace_back();
        colorButtons.back().setFont(font);
        colorButtons.back().setPosition(sf::Vector2f(334 + (col * 76), 60 + (row * 28)));
        colorButtons.back().setSize(sf::Vector2u(68, 20));
        colorButtons.back().setText(opt.first);
        colorButtons.back().setPressedCallback(std::bind(&SettingsGUI::changeColors, this, std::placeholders::_1));
        colorButtons.back().setMode(Button::Mode::Sticky);
        colorButtons.back().linkButtons(colorButtons);

        // Check if this was the last preset used
        if (config("lastPresetColor", "Board").toString() == opt.first)
            colorButtons.back().setPressed(true);

        ++row;
        if (row >= 3)
        {
            row = 0;
            ++col;
        }
    }
}

void SettingsGUI::saveSettings()
{
    config("lastFilename", "Board") = boardFilename.getText();
    config("speed", "Simulation") = speedInput.getNumber();
    config.useSection("Tool");
    config("tool") = tool.getTool();
    config("width") = tool.cursor.getSize().x;
    config("height") = tool.cursor.getSize().y;
    // Re-parse the colors
    for (auto& opt: config.getSection("PresetColors"))
    {
        for (auto& elem: opt.second)
            elem = ColorCode(elem.toString()).toString();
    }
    // Save the last color preset used
    for (Button& button: colorButtons)
    {
        if (button.isPressed())
            config("lastPresetColor", "Board") = button.getText();
    }
}

void SettingsGUI::toggle()
{
    setVisible(!visible);
}

void SettingsGUI::setVisible(bool state)
{
    visible = state;
    if (!visible)
    {
        setFocus(false);

        // Make sure none of the inputboxes have focus when we hide the GUI,
        // so that when the GUI is shown again, it won't be stealing input
        ruleText.setFocus(false);
        boardFilename.setFocus(false);
        widthInput.setFocus(false);
        heightInput.setFocus(false);
        speedInput.setFocus(false);
    }
}

bool SettingsGUI::isVisible() const
{
    return visible;
}

void SettingsGUI::setFocus(bool state)
{
    focus = state;
    if (focus)
        panel.setOutlineColor(sf::Color::Green);
    else
        panel.setOutlineColor(sf::Color(128, 128, 128, 128));
}

bool SettingsGUI::hasFocus() const
{
    return focus;
}

int SettingsGUI::getWidth() const
{
    return width;
}

void SettingsGUI::setRules(const std::string& str)
{
    board.setRules(str);
    ruleGrid.setRules(board.accessRules());
    ruleText.setText(board.getRules());
}

void SettingsGUI::updatePlayButton()
{
    const std::string buttonStrings[] = {"Play", "Pause"};
    bool playing = board.isPlaying();
    buttons["play"].setText(buttonStrings[playing]);
    buttons["play"].setPressed(playing);
}

void SettingsGUI::updateToolButtons()
{
    for (auto& button: toolButtons)
        button.setPressed(false);
    toolButtons[tool.getTool()].setPressed(true);
}

void SettingsGUI::handleEvent(const sf::Event& event)
{
    ruleText.handleEvent(event);
    boardFilename.handleEvent(event);
    widthInput.handleEvent(event);
    heightInput.handleEvent(event);
    speedInput.handleEvent(event);
}

void SettingsGUI::handleMouseEvent(const sf::Event& event, const sf::Vector2f& pos)
{
    // Update focus
    if (event.type == sf::Event::MouseButtonPressed)
        setFocus(collisionBox.contains(pos));

    // Only pass events to the rule grid if the GUI has focus
    if (focus && ruleGrid.handleMouseEvent(event, pos, board.accessRules()))
        ruleText.setText(board.getRules());
    // Update the inputbox if the grid was changed

    ruleText.handleMouseEvent(event, pos);
    boardFilename.handleMouseEvent(event, pos);
    widthInput.handleMouseEvent(event, pos);
    heightInput.handleMouseEvent(event, pos);
    speedInput.handleMouseEvent(event, pos);

    for (auto& button: colorButtons)
        button.handleMouseEvent(event, pos);
    for (auto& button: toolButtons)
        button.handleMouseEvent(event, pos);

    buttons.handleMouseEvent(event, pos);
}

void SettingsGUI::update()
{
    ruleText.update();
    boardFilename.update();
    widthInput.update();
    heightInput.update();
    speedInput.update();
}

void SettingsGUI::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    if (visible)
    {
        window.draw(panel);
        window.draw(ruleGroup);
        window.draw(boardGroup);
        window.draw(colorGroup);
        window.draw(simulationGroup);
        window.draw(toolGroup);
        window.draw(ruleGrid);
        window.draw(ruleText);
        window.draw(boardFilename);
        window.draw(widthInput);
        window.draw(heightInput);
        window.draw(speedInput);

        for (auto& button: colorButtons)
            window.draw(button);
        for (auto& button: toolButtons)
            window.draw(button);

        window.draw(buttons);
    }
}

void SettingsGUI::changeRules(InputBox& input)
{
    board.setRules(input.getText());
    ruleGrid.setRules(board.accessRules());
}

void SettingsGUI::loadBoard(Button& button)
{
    board.loadFromFile(boardFilename.getText());
}

void SettingsGUI::saveBoard(Button& button)
{
    board.saveToFile(boardFilename.getText());
}

void SettingsGUI::changeSize(Button& button)
{
    board.resize(widthInput.getNumber(), heightInput.getNumber(), true);
}

void SettingsGUI::changeColors(Button& button)
{
    board.setColors(config(button.getText(), "PresetColors"), true);
}

void SettingsGUI::reverseColors(Button& button)
{
    board.reverseColors();
}

void SettingsGUI::changeSpeed(InputBox& input)
{
    board.setMaxSpeed(input.getNumber());
}

void SettingsGUI::changeSpeedPreset(Button& button, float num)
{
    speedInput.setNumber(num);
    board.setMaxSpeed(num);
}

void SettingsGUI::playBoard(Button& button)
{
    board.play();
    updatePlayButton();
}

void SettingsGUI::clearBoard(Button& button)
{
    board.clear();
}

void SettingsGUI::randomBoard(Button& button)
{
    board.addRandom();
}

void SettingsGUI::updateShowGrid(Button& button)
{
    showGridCallback(button.isPressed());
}

void SettingsGUI::changeTool(Button& button, int t)
{
    tool.setTool(t);
}
