// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "settingsgui.h"
#include "board.h"

SettingsGUI::SettingsGUI(Board& board, cfg::File& config):
    board(board),
    config(config)
{
	using std::placeholders::_1;
	visible = false;
	setFocus(false);
	width = 740;
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
	toolGroup.setSize(sf::Vector2u(84, height - 8));

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

	loadButton.setFont(font);
	loadButton.setPosition(sf::Vector2f(left, top + 28));
	loadButton.setSize(sf::Vector2u(halfElemWidth, elemHeight));
	loadButton.setText("Load");
	loadButton.setPressedCallback(std::bind(&SettingsGUI::loadBoard, this, _1));

	saveButton.setFont(font);
	saveButton.setPosition(sf::Vector2f(left + 76, top + 28));
	saveButton.setSize(sf::Vector2u(halfElemWidth, elemHeight));
	saveButton.setPressedCallback(std::bind(&SettingsGUI::saveBoard, this, _1));
	saveButton.setText("Save");

	widthInput.setFont(font);
	widthInput.setPosition(sf::Vector2f(left, top + 28 * 2));
	widthInput.setSize(sf::Vector2u(halfElemWidth, elemHeight));
	widthInput.setLabel("Width");

	heightInput.setFont(font);
	heightInput.setPosition(sf::Vector2f(left + 76, top + 28 * 2));
	heightInput.setSize(sf::Vector2u(halfElemWidth, elemHeight));
	heightInput.setLabel("Height");

	changeSizeButton.setFont(font);
	changeSizeButton.setPosition(sf::Vector2f(left, top + 28 * 3));
	changeSizeButton.setSize(sf::Vector2u(maxElemWidth, elemHeight));
	changeSizeButton.setPressedCallback(std::bind(&SettingsGUI::changeSize, this, _1));
	changeSizeButton.setText("Change Size");

	// Color group

	left += groupSize.x + 6;

    reverseColorsButton.setFont(font);
	reverseColorsButton.setPosition(sf::Vector2f(left, top));
	reverseColorsButton.setSize(sf::Vector2u(maxElemWidth, elemHeight));
	reverseColorsButton.setPressedCallback(std::bind(&SettingsGUI::reverseColors, this, _1));
	reverseColorsButton.setText("Reverse Colors");
	// The rest are setup during loadSettings()

	// Simulation group

	left += groupSize.x + 6;

	slowSpeed.setFont(font);
	slowSpeed.setPosition(sf::Vector2f(left, top));
	slowSpeed.setSize(sf::Vector2u(halfElemWidth, elemHeight));
	slowSpeed.setText("Slow");
	slowSpeed.setPressedCallback(std::bind(&SettingsGUI::changeSpeedPreset, this, _1, 10.0f));

	mediumSpeed.setFont(font);
	mediumSpeed.setPosition(sf::Vector2f(left, top + 28));
	mediumSpeed.setSize(sf::Vector2u(halfElemWidth, elemHeight));
	mediumSpeed.setText("Medium");
	mediumSpeed.setPressedCallback(std::bind(&SettingsGUI::changeSpeedPreset, this, _1, 20.0f));

	fastSpeed.setFont(font);
	fastSpeed.setPosition(sf::Vector2f(left, top + 28 * 2));
	fastSpeed.setSize(sf::Vector2u(halfElemWidth, elemHeight));
	fastSpeed.setText("Fast");
	fastSpeed.setPressedCallback(std::bind(&SettingsGUI::changeSpeedPreset, this, _1, 60.0f));

	playButton.setFont(font);
	playButton.setPosition(sf::Vector2f(left + 76, top));
	playButton.setSize(sf::Vector2u(halfElemWidth, elemHeight));
	playButton.setText("Play");
	playButton.setPressedCallback(std::bind(&SettingsGUI::playBoard, this, _1));

	clearButton.setFont(font);
	clearButton.setPosition(sf::Vector2f(left + 76, top + 28));
	clearButton.setSize(sf::Vector2u(halfElemWidth, elemHeight));
	clearButton.setText("Clear");
	clearButton.setPressedCallback(std::bind(&SettingsGUI::clearBoard, this, _1));

	randomButton.setFont(font);
	randomButton.setPosition(sf::Vector2f(left + 76, top + 28 * 2));
	randomButton.setSize(sf::Vector2u(halfElemWidth, elemHeight));
	randomButton.setText("Random");
	randomButton.setPressedCallback(std::bind(&SettingsGUI::randomBoard, this, _1));

	speedInput.setFont(font);
	speedInput.setPosition(sf::Vector2f(left, top + 28 * 3));
	speedInput.setSize(sf::Vector2u(maxElemWidth, elemHeight));
	speedInput.setLabel("Simulation Speed");
	speedInput.setTextEnteredCallback(std::bind(&SettingsGUI::changeSpeed, this, _1));

	// Tools group
    // Will do this later
}

void SettingsGUI::loadSettings()
{
    ruleGrid.setRules(board.accessRules());
    ruleText.setText(board.getRules());
    boardFilename.setText(config("lastFilename", "Board").toString());
    widthInput.setNumber(board.width());
    heightInput.setNumber(board.height());
    speedInput.setNumber(60);
    unsigned row = 0;
    unsigned col = 0;
    for (auto& opt: config.getSection("PresetColors"))
    {
        colorButtons.emplace_back();
        colorButtons.back().setFont(font);
        colorButtons.back().setPosition(sf::Vector2f(334 + (col * 76), 60 + (row * 28)));
        colorButtons.back().setSize(sf::Vector2u(68, 20));
        colorButtons.back().setText(opt.first);
        colorButtons.back().setPressedCallback(std::bind(&SettingsGUI::changeColors, this, std::placeholders::_1));
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
    // Re-parse the colors
    for (auto& opt: config.getSection("PresetColors"))
    {
        for (auto& elem: opt.second)
            elem = ColorCode(elem.toString()).toString();
    }
}

void SettingsGUI::toggle()
{
	visible = !visible;
	if (!visible)
        setFocus(false);
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
    if (board.isPlaying())
        playButton.setText("Pause");
    else
        playButton.setText("Play");
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

    if (ruleGrid.handleMouseEvent(event, pos, board.accessRules()))
        ruleText.setText(board.getRules());
    ruleText.handleMouseEvent(event, pos);
    boardFilename.handleMouseEvent(event, pos);
    loadButton.handleMouseEvent(event, pos);
    saveButton.handleMouseEvent(event, pos);
    widthInput.handleMouseEvent(event, pos);
    heightInput.handleMouseEvent(event, pos);
    changeSizeButton.handleMouseEvent(event, pos);
    slowSpeed.handleMouseEvent(event, pos);
    mediumSpeed.handleMouseEvent(event, pos);
    fastSpeed.handleMouseEvent(event, pos);
    playButton.handleMouseEvent(event, pos);
    clearButton.handleMouseEvent(event, pos);
    randomButton.handleMouseEvent(event, pos);
    speedInput.handleMouseEvent(event, pos);
    reverseColorsButton.handleMouseEvent(event, pos);
    for (auto& button: colorButtons)
        button.handleMouseEvent(event, pos);
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
		window.draw(loadButton);
		window.draw(saveButton);
		window.draw(widthInput);
		window.draw(heightInput);
		window.draw(changeSizeButton);
		window.draw(slowSpeed);
		window.draw(mediumSpeed);
		window.draw(fastSpeed);
		window.draw(playButton);
		window.draw(clearButton);
		window.draw(randomButton);
		window.draw(speedInput);
		window.draw(reverseColorsButton);
		for (auto& button: colorButtons)
            window.draw(button);
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
