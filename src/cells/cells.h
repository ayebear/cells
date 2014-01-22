// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef CELLS_H
#define CELLS_H

#include <string>
#include <SFML/Graphics.hpp>
#include "configfile.h"
#include "board.h"

/*
This class handles the window, input, and output.
*/
class Cells
{
    public:
        Cells(const std::string& title);
        ~Cells();
        void start();

    private:
        void handleEvents();
        void update();
        void draw();

        void createWindow(const std::string& title);
        void handleKeyPressed(const sf::Event::KeyEvent& key);
        void handleMouseButtonPressed(const sf::Event::MouseButtonEvent& mouseButton);
        void handleMouseButtonReleased(const sf::Event::MouseButtonEvent& mouseButton);
        void handleMouseMoved(const sf::Event::MouseMoveEvent& mouseMove);
        void handleMouseWheelMoved(const sf::Event::MouseWheelEvent& mouseWheel);
        void handleWindowResized(const sf::Event::SizeEvent& size);
        void handleInput();
        bool updateMousePos(); // Returns true if the mouse position changed
        void setupShapes();
        void updateCursor(const sf::Vector2i& pos);
        void updateBorderSize();

        bool running; // Used to determine when the main loop ends
        bool hasFocus; // To determine whether to handle input or not
        float elapsedTime; // Elapsed time between frames
        sf::RenderWindow window; // The main window
        ConfigFile config; // The main configuration file
        Board board; // The cellular automata board
        sf::View boardView; // View for the board
        sf::View uiView; // View for the interface
        sf::Vector2u windowSize; // The window size, used for resizing purposes
        sf::RectangleShape cursor; // Normal cursor that follows your mouse pointer
        sf::RectangleShape selectionCursor; // For making selections and copying cells
        sf::RectangleShape border; // The box that shows where the borders are
        bool simulating; // Whether to continue running simulations
        sf::Vector2f worldMousePos; // The current float position of the mouse
        sf::Vector2i mousePos; // The current integer position of the mouse (which cell is selected)
        sf::Vector2f startPanMousePos; // The starting position of the mouse when middle click panning
        bool panning; // Used for middle click panning

        static const ConfigFile::ConfigMap defaultOptions;
};

#endif
