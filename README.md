Cells
=====

Cells is a cellular automaton program. It was inspired by Conway's Game of Life (http://en.wikipedia.org/wiki/Conway's_Game_of_Life), and supports a total of 2^18 (262,144) unique rule sets.

More information about CA can be found here: http://en.wikipedia.org/wiki/Cellular_automaton

* [Features](https://github.com/ayebear/Cells/tree/master/README.md#features)
  * [Future Plans](https://github.com/ayebear/Cells/tree/master/README.md#future-plans)
* [Controls](https://github.com/ayebear/Cells/tree/master/README.md#controls)
* [Installation](https://github.com/ayebear/Cells/tree/master/README.md#installation-instructions)
  * [Dependencies](https://github.com/ayebear/Cells/tree/master/README.md#dependencies)
  * [Compiler Support](https://github.com/ayebear/Cells/tree/master/README.md#compiler-support)
  * [Building](https://github.com/ayebear/Cells/tree/master/README.md#building)
    * [Windows](https://github.com/ayebear/Cells/tree/master/README.md#windows)
    * [Linux](https://github.com/ayebear/Cells/tree/master/README.md#linux)
* [Code](https://github.com/ayebear/Cells/tree/master/README.md#code)
* [Author](https://github.com/ayebear/Cells/tree/master/README.md#author)

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

### Future plans

#### Planned for v0.5.x:
* Make a slider for changing simulation speed
  * Get rid of the 3 speed buttons
* Use a list box instead of buttons for the colors
  * This way you could have any amount of color palettes and could scroll through them
* Improve selection boxes (so they don't move after resizing them)
* Add icons to buttons, possibly remove their text
* Restrict panning so you don't end up somewhere that you don't see anything
* Have a full-screen mode (togglable on runtime, would need to re-create the window)
* Have "smooth" zooming, so it zooms over a short period of time instead of instantly
* Add some kind of list box or dropdown that shows all of the preset rules and their names
  * Will need to update the config to have the names of the rules

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

#### Dependencies
| Library | Minimum Version   | Architecture   |
| ------- | :---------------: | :------------: |
| SFML    | `2.1`             | x32 *or* x64   |

#### Compiler Support
| Compilers                    | Recommended Version   | Minimum Version   |
| ---------                    | :-------------------: | :---------------: |
| MSVC                         | 'v120'                | 'v120'            |
| GNU Compiler Collection(G++) | `4.9.0`               | `4.8.0`           | 
| Clang                        | `3.4.0`               | `3.1.0`           |
  
| Build System | Recommended Version   | Minimum Version   |
| ------------ | :-------------------: | :---------------: |
| CMake        | `2.8.4`               | `2.8.4`           |
  
#### Building

Before beginning to build this application, make sure you have downloaded the prerequisites above.

#### Windows
1. Clone the master Cells repository in a directory of your choice

2. Create a build folder. This project requires an out-of-tree build. This means you ~~will be unable to~~ **should not** run CMake inside the repository.

3. Open up the CMake GUI. In the input box labeled "Where is the source code:", enter the full path to the source folder. In the input box labeled "Where to build the binaries:", enter the full path to the build folder you created in step 2

4. Add CMake variable definitions, you will need just one variable to configure and generate the project:
  * `SFML_ROOT` - The root folder of SFML (e.g. *c:/local/sfml-2.1*)

5. Press the "Configure" button. A window will pop up asking you which compiler to use. Select your x64 version of your preferred compiler. Note that it is possible to build x86 without running into any errors, feel free to modify your build to build x86 if you wish.

6. If the "Generate" button is not clickable, press "Configure" again. Repeat this step until the "Generate" button becomes clickable.

7. Press "Generate".

8. Open up the build folder, and double-click Cells.sln

9. Build the **All Build** target. **Only** build in debug if you *actually* want to debug something; without optimizations the program will run extremely slow. CMake copies the source dependencies to the executable directory, in this case you will find the cfg file and font in `{BINARY_DIR}/data/fonts/`.

10. Copy the required DLL's into your Debug/Release folder, these would be
  * `sfml-graphics-2.dll`
  * `sfml-system-2.dll`
  * `sfml-window-2.dll`
  * For Debug mode make sure you use the debug SFML DLLS, the one's with a trailing `-d`.

#### Linux
1. Install dependencies and supported compiler. Packages should be named something like libsfml-dev and g++, but this may vary depending on your distribution.

2. Follow steps 1 to 7 above. The default SFML_INCLUDE_DIR should be correct (something like /usr/include).

3. Compile the code using the method you chose to generate. For Unix makefiles, open a terminal in the build directory and type "make".

4. Run the program by typing "./Cells", or by double-clicking the binary file.

Code
----

The code is written in C++11, and uses SFML 2.1 (https://github.com/LaurentGomila/SFML).

The code is licensed under the GPLv3 license. See LICENSE.txt for more details.


Author
------

Eric Hebert
