// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef SETTINGSGUI_H
#define SETTINGSGUI_H

#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>
#include "inputbox.h"
#include "button.h"
#include "groupbox.h"
#include "configfile.h"
#include "rulegrid.h"
#include "buttonmap.h"

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
        using CallbackType = std::function<void(bool)>;
        SettingsGUI(Board& board, cfg::File& config, Tool& tool, CallbackType callback);
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
        void updateToolButtons();

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
        void updateShowGrid(Button& button);
        void changeTool(Button& button, int t);

        bool visible;
        bool focus;
        Board& board;
        cfg::File& config;
        Tool& tool;
        CallbackType showGridCallback;
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

        // All of the buttons (later there will be some kind of WidgetMap to handle everything)
        ButtonMap buttons;

        // Rule group
        RuleGrid ruleGrid;
        InputBox ruleText;

        // Board group
        InputBox boardFilename;
        InputBox widthInput;
        InputBox heightInput;
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

        // Simulation group
        // Slider simSpeedSlider;
        // InputBox simSpeedInput;
        InputBox speedInput;

        // Tools group
        std::vector<Button> toolButtons;
};

#endif
