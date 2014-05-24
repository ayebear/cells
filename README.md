Cells
=====

Cells is a cellular automaton program. It was inspired by Conway's Game of Life (http://en.wikipedia.org/wiki/Conway's_Game_of_Life), and supports a total of 2^18 (262,144) unique rule sets.

More information about CA can be found here: http://en.wikipedia.org/wiki/Cellular_automaton


Features
--------

* GUI for changing settings on the fly
  * Rules
  	* Rule grid and rule string
  	* These automatically sync together
  * Board
  	* Load/save with any filename
  	* Resizable to any size
  * Colors
  	* Preset colors from config file are shown
  	* Can reverse the currently used colors
  * Simulation
  	* Simulation speed can be finely adjusted
  	* Play/pause, clear, and random buttons
  * Tools
  	* Paint/duplicate/simulate/toroidal
* Customizable rule sets
  * The states on the rule grid can be switched on/off
  * These can also be defined in a format like "B3/S23"
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

#### Planned for v0.5.x:
* Add a feature for automatically saving screenshots of each generation
* Add an optional grid to see the cells more easily when drawing specific things (working on this now)
* Make a slider for changing simulation speed
* Allow editing the colors using the GUI
* Add more polish:
  * Improve selection boxes
  * Add icons to buttons, possibly remove their text
  * Restrict zooming/panning so you don't end up somewhere that you don't see anything
  * Have a full-screen mode (togglable on runtime)
  * Have "smooth" zooming

#### Planned for future releases:
* Figure out a way to have a completely dynamic board size so that everything is unlimited
  * Along with this would be a new algorithm that could be more efficient


Controls
--------

There are various hotkeys and mouse controls for manipulating, controlling, and viewing the board.

* GUI:
  * Escape - Toggle visibility of GUI
  * Clicking anywhere on the GUI or the board will switch focus to what was clicked on
  	* This is indicated by a green border around either the GUI or the board
* Tools:
  * Left click - Use current tool's primary action
  * Right click - Use current tool's secondary action
  * 1, 2, 3, etc., or Ctrl + mouse wheel - Switch between the tools
  * Ctrl + left click drag - Change tool selection size (rectangular)
  * Ctrl + right click drag - Change tool selection size (square)
  * Ctrl + click - Reset tool selection size to 1x1
* Simulating:
  * Spacebar - Run continually at current speed
  * Enter - Run a single generation
  * N - Toggle running continually at current speed
  * Q/W - Cycle through preset rules
* Panning:
  * Arrow keys or middle click drag - Pan around the board
  * M - Center the board
* Zooming:
  * + and - keys - Zoom in and out
  * I and O - Zoom in and out 2x
  * Mouse wheel - Zoom in and out to mouse pointer
  * R - Reset the zoom to 1:1
* Board:
  * C - Clear the board
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
