Cells
=====

Cells is a [cellular automaton](http://en.wikipedia.org/wiki/Cellular_automaton) program. It was inspired by [Conway's Game of Life](http://en.wikipedia.org/wiki/Conway's_Game_of_Life), and supports a total of 2^18 (262,144) unique rule sets.

##### Screenshot

![alt tag](http://ayebear.com/data/Cells.png)


Table Of Contents
-----------------

* [Features](https://github.com/ayebear/Cells/blob/master/README.md#features)
* [Controls](https://github.com/ayebear/Cells/blob/master/README.md#controls)
* [Installation](https://github.com/ayebear/Cells/blob/master/README.md#installation)
* [Building](https://github.com/ayebear/Cells/blob/master/README.md#building)
  * [Minimum Requirements](https://github.com/ayebear/Cells/blob/master/README.md#minimum-requirements)
  * [Steps To Build](https://github.com/ayebear/Cells/blob/master/README.md#steps-to-build)
* [License](https://github.com/ayebear/Cells/blob/master/README.md#license)
* [Author](https://github.com/ayebear/Cells/blob/master/README.md#author)


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
    * Can automatically save generations to image files
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


Controls
--------

There are various hotkeys and mouse controls for manipulating, controlling, and viewing the board.

Controls                            | Description
----------------------------------- | --------------------------------------
**GUI:**                            |
  Escape                            | Toggle visibility of GUI
**Tools:**                          |
  Left click                        | Use current tool's primary action
  Right click                       | Use current tool's secondary action
  1, 2, 3, 4, or Ctrl + mouse wheel | Switch between the tools
  Ctrl + left click drag            | Change tool selection size (rectangular)
  Ctrl + right click drag           | Change tool selection size (square)
  Ctrl + click                      | Reset tool selection size to 1x1
**Simulating:**                     |
  Spacebar                          | Run continually at current speed
  Enter                             | Run a single generation
  N                                 | Toggle running continually at current speed
  Q/W                               | Cycle through preset rules
**Panning:**                        |
  Arrow keys or middle click drag   | Pan around the board
  M                                 | Center the board
**Zooming:**                        |
  + and - keys                      | Zoom in and out
  I and O                           | Zoom in and out 2x
  Mouse wheel                       | Zoom in and out to mouse pointer
  R                                 | Reset the zoom to 1:1
**Board:**                          |
  C                                 | Clear the board
  Y                                 | Save the board to an image


Installation
------------

Head over to [releases](https://github.com/ayebear/Cells/releases) to download the latest stable version.


Building
--------

#### Minimum Requirements

* [SFML](http://www.sfml-dev.org/) 2.1
* [GCC](https://gcc.gnu.org/) 4.8 **or** [Clang](http://clang.llvm.org/) 3.1
* [CMake](http://www.cmake.org/) 2.8.12

#### Steps To Build

1. Install all of the dependencies listed above.
2. Use CMake to generate a Makefile or an IDE project.
3. Use the generated files to build Cells.


License
-------

This source code is licensed under the GPLv3 license. See LICENSE.txt for more details.


Author
------

Eric Hebert
