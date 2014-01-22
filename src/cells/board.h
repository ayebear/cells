// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <SFML/Graphics.hpp>
#include "matrix.h"
#include "ruleset.h"

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
        void resize(unsigned width, unsigned height, bool preserve = true); // Resizes the board, can be non-destructive
        unsigned width() const;
        unsigned height() const;

        // Simulation
        void simulate(unsigned count = 1); // Runs a number of generations
        void setRules(const std::string& ruleString = defaultRuleString); // Sets the rules from a rule string (see the RuleSet class for more information)
        const std::string& getRules() const; // Returns the rules in the same string format as above

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
        bool saveToFile(const std::string& filename) const; // Saves the board to a file
        bool loadFromFile(const std::string& filename); // Loads the board from a file
        bool saveToImageFile(const std::string& filename) const; // Saves the graphical board to an image file

        // Rendering
        void updateImage(); // Updates the pixels of the image from the logical array
        void updateTexture(); // Copies the image to the texture if necessary
        void draw(sf::RenderTarget& window, sf::RenderStates states) const; // Draw to the window

    private:
        // These are used for simulation
        short countCellsNormal(const sf::Vector2u& pos); // Counts neighboring cells at a position, this is not toroidal and does no bounds checking
        short countCellsToroidal(const sf::Vector2u& pos); // Counts neighboring cells at a position, this is toroidal
        void determineState(const sf::Vector2u& pos, short count); // Determines the next state of the cell based on the number of neighboring cells

        // Other functions
        void setCell(const sf::Vector2u& pos, bool state); // Sets the state of a cell
        bool inBounds(const sf::Vector2i& pos) const; // Returns if the coordinates are in bounds of the board
        void toggle(unsigned& val) const; // Toggles an unsigned int like a bool
        sf::Rect<unsigned> fixRectangle(const sf::IntRect& rect) const; // Takes any rectangle and returns one within bounds of the board

        // The rule set
        RuleSet rules;

        // Logical board
        BoolMatrix board[2]; // Holds the logical states of the cells
        unsigned readBoard; // Which layer should be read from
        unsigned writeBoard; // Which layer should be written to
        // Needs to switch between layers to properly simulate everything
        // Note that both of these variables are the same when a simulation is not in progress

        // Graphical board
        sf::Image boardImage; // Graphical image in system memory
        sf::Texture boardTexture; // Graphical image in GPU memory
        sf::Sprite boardSprite; // Drawable sprite
        sf::Color cellColors[2]; // The colors used for the cells
        bool needToUpdateTexture;

        // Other variables
        sf::Vector2i lastLinePos;
        bool paintingLine;
        BoolMatrix copiedCells;
};

#endif
