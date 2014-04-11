// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef CELLS_H
#define CELLS_H

#include <string>
#include <SFML/Graphics.hpp>
#include "configfile.h"
#include "board.h"
#include "selectionbox.h"
#include "settingsgui.h"

/*
This class handles the window, input, and output.
*/
class Cells
{
    public:
        Cells();
        ~Cells();
        void start();

    private:
        void handleEvents();
        void handleOtherEvent(const sf::Event& event);
        void update();
        void draw();

        void createWindow();
        void handleKeyPressed(const sf::Event::KeyEvent& key);
        void handleMouseButtonPressed(const sf::Event::MouseButtonEvent& mouseButton);
        void handleMouseButtonReleased(const sf::Event::MouseButtonEvent& mouseButton);
        void handleMouseMoved(const sf::Event::MouseMoveEvent& mouseMove);
        void handleMouseWheelMoved(const sf::Event::MouseWheelEvent& mouseWheel);
        void handleWindowResized(const sf::Event::SizeEvent& size);
        void handleInput();
        void handleKeyPanning();
        bool updateMousePos(); // Returns true if the mouse position changed
        void updateCursor(const sf::Vector2i& pos);
        void updateBorderSize();
        bool controlKeyPressed() const;
        void setTool(int tool);
        void handleMouseClick(bool action); // Action is left/right click
        void loadPresetRule();

        bool running; // Used to determine when the main loop ends
        bool hasFocus; // To determine whether to handle input or not
        bool panning; // Used for middle click panning
        bool mouseMoved;
        bool mouseClicked;
        float elapsedTime; // Elapsed time between frames
        //sf::Clock fpsTimer;

        // Main objects
        sf::RenderWindow window; // The main window
        cfg::File config; // The main configuration file
        Board board; // The cellular automata board
        SettingsGUI gui; // The interface for the settings

        // Views
        sf::View boardView; // View for the board
        sf::View uiView; // View for the interface
        sf::Vector2u windowSize; // The window size, used for resizing purposes

        // Positions
        sf::Vector2f worldMousePos; // The current float position of the mouse
        sf::Vector2i mousePos; // The current integer position of the mouse (which cell is selected)
        sf::Vector2f startPanMousePos; // The starting position of the mouse when middle click panning

        // Tools
        enum Tool {Paint = 0, Copy, NormalSimulator, ToroidalSimulator, TotalTools};
        enum Selection {NoSelection = 0, RectSelection, SquareSelection};
        int currentTool; // The current tool being used
        bool changingSelection; // If the cursor size is being changed by the user
        bool squareSelection; // Square selection if true, otherwise rectangular selection
        sf::Color toolColors[TotalTools];
        SelectionBox cursor;

        // Other
        int currentPresetRule;

        // Constants
        static const char* title;
        static const cfg::File::ConfigMap defaultOptions;
};

#endif
