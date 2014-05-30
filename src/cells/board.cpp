// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "board.h"
#include <algorithm>

const char* Board::defaultRuleString = "B3/S23";
const float Board::unlimitedSpeed = 60.0f;
const sf::Color Board::borderColors[] = {
    sf::Color(128, 128, 128),
    sf::Color::Green
};

Board::Board():
    readBoard(0),
    writeBoard(0),
    playing(false),
    borderState(true),
    needToUpdateTexture(true),
    grid(sf::Lines),
    gridShown(false),
    autosaveImages(false),
    autosavePartialImages(false),
    paintingLine(false)
{
    resetColors();
    boardSprite.setPosition(0, 0);
    setMaxSpeed(unlimitedSpeed);
    setRules();

    // Setup border
    border.setPosition(0, 0);
    updateBorderSize();
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(borderColors[borderState]);
    border.setOutlineThickness(1);
}

Board::Board(unsigned width, unsigned height):
    Board()
{
    resize(width, height, false);
}

void Board::setupScreenshots(const std::string& format, bool save, bool savePartial)
{
    filenameGen.setFormat(format);
    autosaveImages = save;
    autosavePartialImages = savePartial;
}

void Board::resize(unsigned width, unsigned height, bool preserve)
{
    // Only resize if the new size is different
    if (width != board[readBoard].width() || height != board[readBoard].height())
    {
        // Clear the logical arrays if specified
        if (!preserve)
        {
            board[0].clear();
            board[1].clear();
        }

        // Resize the logical arrays
        board[0].resize(width, height);
        board[1].resize(width, height);

        // Create a new image with this size
        boardImage.create(width, height, cellColors.front().toColor());
        if (preserve)
            updateImage();
        updateTexture();
        updateBorderSize();
        updateGrid();
    }
}

unsigned Board::width() const
{
    return board[readBoard].width();
}

unsigned Board::height() const
{
    return board[readBoard].height();
}

void Board::setColors(const std::vector<sf::Color>& colors)
{
    cellColors.clear();
    for (const auto& col: colors)
        cellColors.emplace_back(col);
    if (cellColors.size() < 2)
        resetColors();
    updateMaxState();
}

void Board::setColors(const std::vector<std::string>& colors)
{
    cellColors.clear();
    for (const std::string& colStr: colors)
        cellColors.emplace_back(colStr);
    if (cellColors.size() < 2)
        resetColors();
    updateMaxState();
}

void Board::setColors(const cfg::Option& colors, bool updateBoard)
{
    cellColors.clear();
    for (const auto& opt: colors)
        cellColors.emplace_back(opt.toString());
    if (cellColors.size() < 2)
        resetColors();
    updateMaxState();
    if (updateBoard)
    {
        updateImage();
        updateTexture();
    }
}

std::vector<std::string> Board::getColorStrings() const
{
    std::vector<std::string> colorStrs;
    for (const auto& col: cellColors)
        colorStrs.push_back(col.toString());
    return colorStrs;
}

const sf::Color& Board::getFirstColor() const
{
    return cellColors[0].toColor();
}

void Board::resetColors()
{
    cellColors.clear();
    cellColors.emplace_back(sf::Color::Black);
    cellColors.emplace_back(sf::Color::White);
    updateMaxState();
}

void Board::reverseColors()
{
    std::reverse(cellColors.begin(), cellColors.end());
    updateImage();
    updateTexture();
}

void Board::setRules(const std::string& ruleString)
{
    rules.setFromString(ruleString.empty() ? defaultRuleString : ruleString);
}

void Board::setRules(const RuleSet& newRules)
{
    rules = newRules;
}

const std::string& Board::getRules() const
{
    return rules.toString();
}

RuleSet& Board::accessRules()
{
    return rules;
}

void Board::simulate(bool toroidal)
{
    simulate(sf::IntRect(0, 0, width(), height()), toroidal, false);
}

void Board::simulate(const sf::IntRect& rect, bool toroidal, bool partial)
{
    auto fixedRect = fixRectangle(rect);
    // Make sure the simulation area is at least 3x3
    if (fixedRect.width >= 3 && fixedRect.height >= 3 &&
        (maxSpeed >= (unlimitedSpeed - 2.0f) || simTimer.getElapsedTime().asSeconds() >= maxTime))
    {
        /*
        The order in which the cells are simulated:
        2 2 2 2 2
        3 1 1 1 3
        3 1 1 1 3
        3 1 1 1 3
        2 2 2 2 2
        This is so we can skip bounds checking and toroidal wrap-around algorithms for most of the cells.
        */

        simTimer.restart();
        toggle(writeBoard);
        sf::Vector2u cellPos;
        unsigned bottom = fixedRect.top + fixedRect.height;
        unsigned right = fixedRect.left + fixedRect.width;
        unsigned totalCells = 0;

        // This fixes a bug where partial simulations cause not all cells to be copied
        if (partial || fixedRect.width < width() || fixedRect.height < height()) // If this is a partial simulation
            board[writeBoard] = board[readBoard]; // Copy the latest board to the board being written to

        // 1) Go through the main part of the cells except for the edges
        for (cellPos.y = fixedRect.top + 1; cellPos.y < bottom - 1; ++cellPos.y)
            for (cellPos.x = fixedRect.left + 1; cellPos.x < right - 1; ++cellPos.x, ++totalCells)
                determineState(cellPos, countCellsFast(cellPos));
        // 2) Top and bottom rows
        for (cellPos.y = fixedRect.top; cellPos.y < bottom; cellPos.y += fixedRect.height - 1)
            for (cellPos.x = fixedRect.left; cellPos.x < right; ++cellPos.x, ++totalCells)
                determineState(cellPos, (toroidal ? countCellsToroidal(cellPos, fixedRect) : countCellsNormal(cellPos)));
        // 3) Left and right columns
        for (cellPos.x = fixedRect.left; cellPos.x < right; cellPos.x += fixedRect.width - 1)
            for (cellPos.y = fixedRect.top + 1; cellPos.y < bottom - 1; ++cellPos.y, ++totalCells)
                determineState(cellPos, (toroidal ? countCellsToroidal(cellPos, fixedRect) : countCellsNormal(cellPos)));
        readBoard = writeBoard;

        // Save a screenshot
        if (partial && autosavePartialImages)
            saveToImageFile(rect);
        else if (!partial && autosaveImages)
            saveToImageFile();
    }
}

void Board::setMaxSpeed(float speed)
{
    maxSpeed = speed;
    if (maxSpeed == 0)
        maxSpeed = unlimitedSpeed;
    maxTime = 1.0f / maxSpeed;
}

bool Board::play()
{
    playing = !playing;
    return playing;
}

bool Board::isPlaying() const
{
    return playing;
}

void Board::update()
{
    if (playing)
        simulate();
}

void Board::paintCell(const sf::Vector2i& pos, bool state)
{
    if (inBounds(pos))
        setCell(sf::Vector2u(pos.x, pos.y), state);
}

void Board::paintLine(const sf::Vector2i& startPos, const sf::Vector2i& endPos, bool state)
{
    // This function assumes that the coordinates passed in are within bounds of the board
    // Calculate the slope of the line
    float slope = static_cast<float>(endPos.y - startPos.y) / static_cast<float>(endPos.x - startPos.x);
    int yIntercept = startPos.y - (slope * startPos.x);
    if (slope >= -1 && slope <= 1)
    {
        sf::Vector2i tempPos;
        int first = std::min(startPos.x, endPos.x);
        int last = std::max(startPos.x, endPos.x);
        for (tempPos.x = first; tempPos.x <= last; ++tempPos.x)
        {
            tempPos.y = tempPos.x * slope + yIntercept;
            if (tempPos.y == 0)
                tempPos.y = startPos.y;
            paintCell(tempPos, state);
        }
    }
    else
    {
        sf::Vector2i tempPos;
        int first = std::min(startPos.y, endPos.y);
        int last = std::max(startPos.y, endPos.y);
        for (tempPos.y = first; tempPos.y <= last; ++tempPos.y)
        {
            tempPos.x = (tempPos.y - yIntercept) / slope;
            if (tempPos.x == 0)
                tempPos.x = startPos.x;
            paintCell(tempPos, state);
        }
    }
}

void Board::paintLine(const sf::Vector2i& pos, bool state)
{
    if (paintingLine)
        paintLine(lastLinePos, pos, state);
    else
    {
        paintCell(pos, state);
        paintingLine = true;
    }
    lastLinePos = pos;
}

void Board::finishLine()
{
    paintingLine = false;
}

void Board::paintBlock(const sf::IntRect& rect, bool state)
{
    // Calculate the proper block of cells to paint
    auto fixedRect = fixRectangle(rect);
    if (fixedRect.width > 0 && fixedRect.height > 0)
    {
        // Paint the cells
        for (unsigned y = 0; y < fixedRect.height; ++y)
            for (unsigned x = 0; x < fixedRect.width; ++x)
                setCell(sf::Vector2u(fixedRect.left + x, fixedRect.top + y), state);
    }
}

void Board::copyBlock(const sf::IntRect& rect)
{
    // Calculate the proper block of cells to copy
    auto fixedRect = fixRectangle(rect);
    if (fixedRect.width > 0 && fixedRect.height > 0)
    {
        // Resize the buffer
        copiedCells.resize(fixedRect.width, fixedRect.height);

        // Copy the cells into the buffer
        for (unsigned y = 0; y < fixedRect.height; ++y)
            for (unsigned x = 0; x < fixedRect.width; ++x)
                copiedCells(x, y) = board[readBoard](fixedRect.left + x, fixedRect.top + y);
    }
}

void Board::pasteBlock(const sf::Vector2i& pos)
{
    // Paste the cells exactly as they were copied
    for (unsigned y = 0; y < copiedCells.height(); ++y)
    {
        for (unsigned x = 0; x < copiedCells.width(); ++x)
        {
            sf::Vector2u absolutePos(pos.x + x, pos.y + y);
            // Make sure the state is valid, since the colors could change
            char state = std::min(copiedCells(x, y), maxState);
            setCell(absolutePos, state);
        }
    }
}

void Board::clear()
{
    sf::Vector2u cellPos;
    for (cellPos.y = 0; cellPos.y < board[writeBoard].height(); ++cellPos.y)
        for (cellPos.x = 0; cellPos.x < board[writeBoard].width(); ++cellPos.x)
            setCell(cellPos, 0);
}

void Board::addRandom()
{
    unsigned w = width();
    unsigned h = height();
    unsigned iterations = (w * h) / 8;
    for (unsigned i = 0; i < iterations; ++i)
        paintCell(sf::Vector2i(rand() % w, rand() % h), true);
}

bool Board::saveToFile(const std::string& filename) const
{
    return board[readBoard].saveToFile(filename);
}

bool Board::loadFromFile(const std::string& filename)
{
    bool status = board[writeBoard].loadFromFile(filename);
    if (status)
    {
        // Needs to copy the newly loaded board to the other board
        unsigned newWidth = board[writeBoard].width();
        unsigned newHeight = board[writeBoard].height();
        board[(writeBoard + 1) % 2] = board[writeBoard];
        boardImage.create(newWidth, newHeight);
        updateImage();
        updateTexture();
        updateBorderSize();
        updateGrid();
    }
    return status;
}

bool Board::saveToImageFile(const std::string& filename) const
{
    return boardImage.saveToFile(filename.empty() ? filenameGen.getNextFilename() : filename);
}

bool Board::saveToImageFile(const sf::IntRect& rect, const std::string& filename) const
{
    sf::Image partialImage;
    partialImage.create(rect.width, rect.height);
    partialImage.copy(boardImage, 0, 0, rect);
    return partialImage.saveToFile(filename.empty() ? filenameGen.getNextFilename() : filename);
}

bool Board::setBoardState(bool state)
{
    bool status = false;
    if (state != borderState)
    {
        border.setOutlineColor(borderColors[state]);
        borderState = state;
        status = true;
    }
    return status;
}

void Board::setGridColor(const std::string& color)
{
    if (!color.empty())
        gridColor = color;
    for (unsigned i = 0; i < grid.getVertexCount(); i++)
        grid[i].color = gridColor.toColor();
}

void Board::showGrid(bool state)
{
    gridShown = state;
}

void Board::updateImage()
{
    needToUpdateTexture = true;
    sf::Vector2u cellPos;
    for (cellPos.y = 0; cellPos.y < board[readBoard].height(); ++cellPos.y)
        for (cellPos.x = 0; cellPos.x < board[readBoard].width(); ++cellPos.x)
            setPixel(cellPos.x, cellPos.y, std::min(board[readBoard](cellPos), maxState));
}

void Board::updateTexture()
{
    if (needToUpdateTexture)
    {
        boardTexture.loadFromImage(boardImage); // Copy the image into the texture
        boardSprite.setTexture(boardTexture, true);
        needToUpdateTexture = false;
    }
}

void Board::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    window.draw(boardSprite);
    window.draw(border);
    if (gridShown)
        window.draw(grid);
}

unsigned Board::countCellsFast(const sf::Vector2u& pos)
{
    // WARNING: This function assumes the following is true:
    // if (pos.x > 0 && pos.y > 0 && pos.x < board.width() - 1 && pos.y < board.height() - 1)
    // Which means that you should not use this function on cells that are on the edge of the board!
    // This is only done for performance reasons.
    unsigned count = 0;
    sf::Vector2u startPos(pos.x - 1, pos.y - 1);
    sf::Vector2u endPos(pos.x + 1, pos.y + 1);
    sf::Vector2u tempPos;

    // Iterate through a 3x3 grid around the cell
    for (tempPos.y = startPos.y; tempPos.y <= endPos.y; ++tempPos.y)
        for (tempPos.x = startPos.x; tempPos.x <= endPos.x; ++tempPos.x)
            if (tempPos != pos) // Ignore the center cell
                count += (board[readBoard](tempPos) != 0); // Increment the count if the cell is live

    return count;
}

unsigned Board::countCellsNormal(const sf::Vector2u& pos)
{
    unsigned count = 0;
    sf::Vector2u startPos(std::max(pos.x - 1, 0U), std::max(pos.y - 1, 0U));
    sf::Vector2u endPos(std::min(pos.x + 1, width() - 1), std::min(pos.y + 1, height() - 1));
    sf::Vector2u tempPos;

    // Iterate through a 3x3 grid around the cell
    for (tempPos.y = startPos.y; tempPos.y <= endPos.y; ++tempPos.y)
        for (tempPos.x = startPos.x; tempPos.x <= endPos.x; ++tempPos.x)
            if (tempPos != pos) // Ignore the center cell
                count += (board[readBoard](tempPos) != 0); // Increment the count if the cell is live

    return count;
}

unsigned Board::countCellsToroidal(const sf::Vector2u& pos, sf::Rect<unsigned>& rect)
{
    unsigned count = 0;
    sf::Vector2u tempPos;
    unsigned bottom = rect.top + rect.height - 1;
    unsigned right = rect.left + rect.width - 1;

    // Pre-calculate the ranges of cells to count
    unsigned rangeX[3] = {pos.x - 1, pos.x, pos.x + 1};
    unsigned rangeY[3] = {pos.y - 1, pos.y, pos.y + 1};
    if (pos.x == rect.left)
        rangeX[0] = right;
    else if (pos.x == right)
        rangeX[2] = rect.left;
    if (pos.y == rect.top)
        rangeY[0] = bottom;
    else if (pos.y == bottom)
        rangeY[2] = rect.top;

    // Iterate through the ranges as a wrapped-around 3x3 grid
    for (unsigned y: rangeY)
        for (unsigned x: rangeX)
            if (x != pos.x || y != pos.y) // Ignore the center cell
                count += (board[readBoard](x, y) != 0); // Increment the count if the cell is live

    return count;
}

void Board::determineState(const sf::Vector2u& pos, unsigned count)
{
    // Uses the rule set lookup table, and updates the state of the cell to the new state
    bool currentState = (board[readBoard](pos) != 0);
    incrementCell(pos, rules.getRule(currentState, count));
}

void Board::setCell(const sf::Vector2u& pos, char state)
{
    board[writeBoard](pos) = state;
    setPixel(pos.x, pos.y, state);
    needToUpdateTexture = true;
}

void Board::incrementCell(const sf::Vector2u& pos, bool state)
{
    char& cell = board[writeBoard](pos);
    if (state)
        cell = std::min(static_cast<char>(board[readBoard](pos) + 1), maxState);
    else
        cell = 0;
    setPixel(pos.x, pos.y, cell);
    needToUpdateTexture = true;
}

void Board::setPixel(unsigned x, unsigned y, char state)
{
    boardImage.setPixel(x, y, cellColors[state].toColor());
}

bool Board::inBounds(const sf::Vector2i& pos) const
{
    return (pos.x >= 0 && pos.y >= 0 && pos.x < static_cast<int>(board[writeBoard].width()) && pos.y < static_cast<int>(board[writeBoard].height()));
}

void Board::toggle(unsigned& val) const
{
    val = !(static_cast<bool>(val));
}

void Board::updateBorderSize()
{
    border.setSize(sf::Vector2f(width(), height()));
}

sf::Rect<unsigned> Board::fixRectangle(const sf::IntRect& rect) const
{
    // TODO: Make sure this works properly on negatively sized rectangles!
    // (Doesn't apply until the selection box can go negative)

    // Convert the rectangle into coordinates of 2 corners
    // Also make sure everything is within bounds
    int boardWidth = board[readBoard].width();
    int boardHeight = board[readBoard].height();
    sf::Vector2u topLeft(std::min(std::max(rect.left, 0), boardWidth),
                         std::min(std::max(rect.top, 0), boardHeight));
    sf::Vector2u bottomRight(std::min(std::max(rect.left + rect.width, 0), boardWidth),
                             std::min(std::max(rect.top + rect.height, 0), boardHeight));

    // Then convert the coordinates back into a rectangle
    return sf::Rect<unsigned>(topLeft.x, topLeft.y,
        bottomRight.x - topLeft.x,
        bottomRight.y - topLeft.y);
}

void Board::updateMaxState()
{
    maxState = static_cast<char>(cellColors.size() - 1);
}

void Board::updateGrid()
{
    unsigned boardWidth = width();
    unsigned boardHeight = height();
    unsigned gridSize = boardWidth * 2 + boardHeight * 2 + 4;
    grid.resize(gridSize);
    // Generate columns
    for (unsigned col = 0; col <= boardWidth; ++col)
    {
        grid[col * 2].position = sf::Vector2f(col, 0);
        grid[col * 2 + 1].position = sf::Vector2f(col, boardHeight);
    }
    // Generate rows
    unsigned pos = (boardWidth * 2) + 2;
    for (unsigned row = 0; row <= boardHeight; ++row)
    {
        grid[pos + row * 2].position = sf::Vector2f(0, row);
        grid[pos + row * 2 + 1].position = sf::Vector2f(boardWidth, row);
    }
    setGridColor();
}
