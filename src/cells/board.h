// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <SFML/Graphics.hpp>
#include "matrix.h"
#include "ruleset.h"
#include "colorcode.h"
#include "configoption.h"

/*
This class is used for simulating cellular automata.
It handles simulating, drawing, saving, and loading.
It also supports custom rule sets.
*/
class Board: public sf::Drawable
{
    public:
        static const char* defaultRuleString;

        Board();
        Board(unsigned width, unsigned height);

        // Board size
        void resize(unsigned width, unsigned height, bool preserve = true); // Resizes the board, can be non-destructive
        unsigned width() const;
        unsigned height() const;

        // Cell colors
        void setColors(const std::vector<sf::Color>& colors);
        void setColors(const std::vector<std::string>& colors);
        void setColors(const cfg::Option& colors, bool updateBoard = false);
        std::vector<std::string> getColorStrings() const;
        const sf::Color& getFirstColor() const;
        void resetColors(); // Resets the colors to the default of black and white
        void reverseColors(); // Reverses the order of the current colors

        // Rule settings
        void setRules(const std::string& ruleString = defaultRuleString); // Sets the rules from a rule string (see the RuleSet class for more information)
        void setRules(const RuleSet& newRules); // Sets the rules from another rule set object
        const std::string& getRules() const; // Returns the rules in the same string format as above
        RuleSet& accessRules(); // Returns a reference to the rule set

        // Simulation
        void simulate(bool toroidal = true); // Runs a single generation on the entire board
        void simulate(const sf::IntRect& rect, bool toroidal = true); // Runs a single generation on the specified area
        void setMaxSpeed(float speed);
        bool play(); // Returns true if playing, false if paused
        bool isPlaying() const;
        void update(); // Simulates the board if playing is true

        // Cell manipulation
        void paintCell(const sf::Vector2i& pos, bool state); // Sets the state of a cell, does bounds checking
        void paintLine(const sf::Vector2i& startPos, const sf::Vector2i& endPos, bool state); // Paints a line of cells
        void paintLine(const sf::Vector2i& pos, bool state); // Paints a line of cells (good for user input, see implementation for more details)
        void finishLine(); // Ends the line being painted, so that a new line can be painted
        void paintBlock(const sf::IntRect& rect, bool state); // Paints a rectangular block of any size
        void copyBlock(const sf::IntRect& rect); // Stores the cells into a temporary buffer
        void pasteBlock(const sf::Vector2i& pos); // Pastes the cells from the buffer into this position

        // Board loading/saving
        void clear(); // Clears the entire board
        void addRandom(); // Adds some random cells
        bool saveToFile(const std::string& filename) const; // Saves the board to a file
        bool loadFromFile(const std::string& filename); // Loads the board from a file
        bool saveToImageFile(const std::string& filename) const; // Saves the graphical board to an image file

        // Rendering
        bool setBoardState(bool state); // Sets the color of the border (returns true if changed)
        void setGridColor(const std::string& color = ""); // Sets the color of the grid
        void showGrid(bool state = true); // Show/hide the grid
        void updateImage(); // Updates the pixels of the image from the logical array
        void updateTexture(); // Copies the image to the texture if necessary
        void draw(sf::RenderTarget& window, sf::RenderStates states) const; // Draw to the window

    private:
        // These are used for simulation
        unsigned countCellsFast(const sf::Vector2u& pos); // Counts neighboring cells at a position, this is not toroidal and does no bounds checking
        unsigned countCellsNormal(const sf::Vector2u& pos); // Counts neighboring cells at a position, this is not toroidal
        unsigned countCellsToroidal(const sf::Vector2u& pos, sf::Rect<unsigned>& rect); // Counts neighboring cells at a position, this is toroidal
        void determineState(const sf::Vector2u& pos, unsigned count); // Determines the next state of the cell based on the number of neighboring cells

        // Other functions
        void setCell(const sf::Vector2u& pos, bool state); // Sets the state of a cell
        void incrementCell(const sf::Vector2u& pos, bool state); // Sets the state of a cell (also increments the color)
        void setPixel(unsigned x, unsigned y, char state); // Set the graphical state of a cell
        bool inBounds(const sf::Vector2i& pos) const; // Returns if the coordinates are in bounds of the board
        void toggle(unsigned& val) const; // Toggles an unsigned int like a bool
        void updateBorderSize(); // Updates the size of the border
        sf::Rect<unsigned> fixRectangle(const sf::IntRect& rect) const; // Takes any rectangle and returns one within bounds of the board
        void updateMaxState();
        void updateGrid();

        // The rule set
        RuleSet rules;

        // Logical board
        Matrix<char> board[2]; // Holds the logical states of the cells
        unsigned readBoard; // Which layer should be read from
        unsigned writeBoard; // Which layer should be written to
        // Needs to switch between layers to properly simulate everything
        // Note that both of these variables are the same when a simulation is not in progress
        bool playing;

        // Graphical board
        sf::Image boardImage; // Graphical image in system memory
        sf::Texture boardTexture; // Graphical image in GPU memory
        sf::Sprite boardSprite; // Drawable sprite
        sf::RectangleShape border; // The box that shows where the borders are
        static const sf::Color borderColors[2];
        bool borderState;
        std::vector<ColorCode> cellColors; // The colors used for the cells
        bool needToUpdateTexture;
        sf::VertexArray grid;
        ColorCode gridColor;
        bool gridShown;

        // Simulation speed limiter
        sf::Clock simTimer;
        float maxSpeed;
        float maxTime;
        static const float unlimitedSpeed;

        // Other variables
        sf::Vector2i lastLinePos;
        bool paintingLine;
        Matrix<char> copiedCells;
        char maxState;
        int maxStateInt;
};

#endif
