// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef SETTINGSGUI_H
#define SETTINGSGUI_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "inputbox.h"
#include "button.h"
#include "groupbox.h"
#include "configfile.h"
#include "rulegrid.h"

class Board;
class Tool;

/*
This class contains the main GUI for all of the settings.
Currently it is managing all of the widgets manually. It will use a GUI factory after it is implemented.

TODO:
    Handle window resizing within this class
    Handle mouse events within this class
*/
class SettingsGUI: public sf::Drawable
{
    public:
        SettingsGUI(Board& board, cfg::File& config, Tool& tool);
        void loadSettings();
        void saveSettings();
        void toggle();
        void setVisible(bool state = true);
        bool isVisible() const;
        void setFocus(bool state = true);
        bool hasFocus() const;
        int getWidth() const;
        void setRules(const std::string& str);
        void updatePlayButton();

        void handleEvent(const sf::Event& event);
        void handleMouseEvent(const sf::Event& event, const sf::Vector2f& pos);
        void update();
        void draw(sf::RenderTarget& window, sf::RenderStates states) const;

    private:
        void changeRules(InputBox& input);
        void loadBoard(Button& button);
        void saveBoard(Button& button);
        void changeSize(Button& button);
        void changeColors(Button& button);
        void reverseColors(Button& button);
        void changeSpeed(InputBox& input);
        void changeSpeedPreset(Button& button, float num);
        void playBoard(Button& button);
        void clearBoard(Button& button);
        void randomBoard(Button& button);
        void changeTool(Button& button, int t);

    	bool visible;
    	bool focus;
        Board& board;
        cfg::File& config;
        Tool& tool;
    	sf::Font font;
    	int width;
    	int height;

    	// Groups and stuff
    	sf::RectangleShape panel;
    	sf::FloatRect collisionBox;
    	GroupBox ruleGroup;
    	GroupBox boardGroup;
    	GroupBox colorGroup;
    	GroupBox simulationGroup;
    	GroupBox toolGroup;

    	// Rule group
    	RuleGrid ruleGrid;
    	InputBox ruleText;

    	// Board group
    	InputBox boardFilename;
    	Button loadButton;
    	Button saveButton;
    	InputBox widthInput;
    	InputBox heightInput;
    	Button changeSizeButton;
    	// Would be nice to fit screenshot buttons here

    	// Color group
    	// Will need scrollable window of:
            // InputBox (color)
            // Button (remove: "-")
        // InputBox (new color)
        // Button (add: "+")
        // The inputboxes could be outlined with the color entered, same with the text
        // For now just have a few preset buttons:
        std::vector<Button> colorButtons;
        Button reverseColorsButton;

    	// Simulation group
        // Slider simSpeedSlider;
        // InputBox simSpeedInput;
        Button slowSpeed;
        Button mediumSpeed;
        Button fastSpeed;
        Button playButton; // Switches to pause when running
        Button clearButton;
        Button randomButton;
        InputBox speedInput;

    	// Tools group
    	std::vector<Button> toolButtons;
};

#endif
