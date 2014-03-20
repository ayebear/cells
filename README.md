Cells
=====

Cells is a cellular automaton program. It was inspired by Conway's Game of Life (http://en.wikipedia.org/wiki/Conway's_Game_of_Life), and supports a total of 2^18 (262,144) unique rule sets.

More information about CA can be found here: http://en.wikipedia.org/wiki/Cellular_automaton


Features
--------

* Customizable rule sets
  * These can be defined in a format like "B3/S23"
  * B means birth, or the number of neighboring cells that must be live for a dead cell to become live
  * S means survival, or the number of neighboring cells that must be live for a live cell to stay live
* Multiple tools
  1. Paint on/off (blue)
  2. Copy/paste (red)
  3. Normal simulator (yellow)
  4. Toroidal simulator (green)
* Customizable color palette
  * This is specified as a list of colors in #RGB or #RRGGBB format
  * The first color is used for dead cells, and the rest of the colors are used for live cells
  * Each generation a cell is still live, it will switch to the next color. Once it reaches the last color, it will stay that color until it dies.

### Future plans

#### Planned for v0.4.x:
* GUI for changing settings
  * Currently you must modify "cells.cfg" to change most of the settings
* Have a way of easily changing the size of the board while the program is running
* Have a way to change the max speed of simulating
* Add a feature for automatically saving screenshots of each generation

#### Planned for v0.5.x:
* Add an optional grid to see the cells more easily when drawing specific things
* Add more polish:
  * Improve selection boxes
  * Restrict zooming/panning so you don't end up somewhere that you don't see anything
  * Have a full-screen mode

#### Planned for future releases:
* Figure out a way to have a completely dynamic board size so that everything is unlimited
  * Along with this would be a new algorithm that would be much more efficient


Controls
--------

There are various hotkeys and mouse controls for manipulating, controlling, and viewing the board.

* Tools:
  * Left click - Use current tool's primary action
  * Right click - Use current tool's secondary action
  * 1, 2, 3, etc., or Ctrl + mouse wheel - Switch between the tools
  * Ctrl + left click drag - Change tool selection size (rectangular)
  * Ctrl + right click drag - Change tool selection size (square)
  * Ctrl + click - Reset tool selection size to 1x1
* Simulating:
  * Spacebar - Run continually at max speed
  * Enter - Run a single generation
  * N - Toggle running continually at max speed
* Panning:
  * Arrow keys or middle click drag - Pan around the board
  * M - Center the board
* Zooming:
  * + and - keys - Zoom in and out
  * I and O - Zoom in and out 2x
  * Mouse wheel - Zoom in and out to mouse pointer
* Board:
  * C - Clear the board
  * S - Save the board to a file
  * L - Loads the board from a file
  * Y - Save the board to an image


Installation Instructions
-------------------------

Download the archive for your OS under releases, then extract it, and run the program.
You can also compile the code yourself using a recent version of GCC or another C++11 compliant compiler.


Code
----

The code is written in C++11, and uses SFML 2.1 (https://github.com/LaurentGomila/SFML).

The code is licensed under the GPLv3 license. See LICENSE.txt for more details.


Author
------

Eric Hebert
