// See the file COPYRIGHT.txt for authors and copyright information.
// See the file LICENSE.txt for copying conditions.

#include "board.h"
#include <iostream>

const char* Board::defaultRuleString = "B3/S23";

Board::Board()
{
    cellColors[0] = sf::Color::Black;
    cellColors[1] = sf::Color::White;
    readBoard = 0;
    writeBoard = 0;
    needToUpdateTexture = true;
    paintingLine = false;
    boardSprite.setPosition(0, 0);
    setRules();
}

Board::Board(unsigned width, unsigned height):
    Board()
{
    resize(width, height, false);
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
        boardImage.create(width, height);
        if (preserve)
            updateImage();
        updateTexture();
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

void Board::simulate(unsigned count)
{
    sf::Vector2u cellPos;
    for (unsigned generation = 0; generation < count; ++generation)
    {
        //std::cout << "Simulating...\n";
        toggle(writeBoard);
        // Go through the main part of the cells except for the edges
        for (cellPos.y = 1; cellPos.y < board[readBoard].height() - 1; ++cellPos.y)
            for (cellPos.x = 1; cellPos.x < board[readBoard].width() - 1; ++cellPos.x)
                determineState(cellPos, countCellsNormal(cellPos));
        // Top and bottom rows
        for (cellPos.y = 0; cellPos.y < board[readBoard].height(); cellPos.y += board[readBoard].height() - 1)
            for (cellPos.x = 0; cellPos.x < board[readBoard].width(); ++cellPos.x)
                determineState(cellPos, countCellsToroidal(cellPos));
        // Left and right columns
        for (cellPos.x = 0; cellPos.x < board[readBoard].width(); cellPos.x += board[readBoard].width() - 1)
            for (cellPos.y = 1; cellPos.y < board[readBoard].height() - 1; ++cellPos.y)
                determineState(cellPos, countCellsToroidal(cellPos));
        readBoard = writeBoard; // Same as toggle(readBoard), but doing this just to be safe
    }
}

void Board::setRules(const std::string& ruleString)
{
    rules.setFromString(ruleString.empty() ? defaultRuleString : ruleString);
}

const std::string& Board::getRules() const
{
    return rules.toString();
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

    // Paint the cells
    for (unsigned y = 0; y < fixedRect.height; ++y)
        for (unsigned x = 0; x < fixedRect.width; ++x)
            setCell(sf::Vector2u(fixedRect.left + x, fixedRect.top + y), state);
}

void Board::copyBlock(const sf::IntRect& rect)
{
    // Calculate the proper block of cells to copy
    auto fixedRect = fixRectangle(rect);

    // Resize the buffer
    copiedCells.resize(fixedRect.width, fixedRect.height);

    // Copy the cells into the buffer
    for (unsigned y = 0; y < fixedRect.height; ++y)
        for (unsigned x = 0; x < fixedRect.width; ++x)
            copiedCells(x, y) = board[readBoard](fixedRect.left + x, fixedRect.top + y);

    std::cout << fixedRect.width << " x " << fixedRect.height << " block copied at {" << fixedRect.left << ", " << fixedRect.top << "}\n";
}

void Board::pasteBlock(const sf::Vector2i& pos)
{
    // Paint the cells
    for (unsigned y = 0; y < copiedCells.height(); ++y)
        for (unsigned x = 0; x < copiedCells.width(); ++x)
            paintCell(sf::Vector2i(pos.x + x, pos.y + y), copiedCells(x, y));

    std::cout << copiedCells.width() << " x " << copiedCells.height() << " block pasted.\n";
}

void Board::clear()
{
    sf::Vector2u cellPos;
    for (cellPos.y = 0; cellPos.y < board[writeBoard].height(); ++cellPos.y)
        for (cellPos.x = 0; cellPos.x < board[writeBoard].width(); ++cellPos.x)
            setCell(cellPos, false);
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
    }
    return status;
}

bool Board::saveToImageFile(const std::string& filename) const
{
    return boardImage.saveToFile(filename);
}

void Board::updateImage()
{
    needToUpdateTexture = true;
    sf::Vector2u cellPos;
    for (cellPos.y = 0; cellPos.y < board[readBoard].height(); ++cellPos.y)
        for (cellPos.x = 0; cellPos.x < board[readBoard].width(); ++cellPos.x)
            boardImage.setPixel(cellPos.x, cellPos.y, cellColors[board[readBoard](cellPos)]);
}

void Board::updateTexture()
{
    if (needToUpdateTexture)
    {
        //std::cout << "Updating texture...\n";
        boardTexture.loadFromImage(boardImage); // Copy the image into the texture
        boardSprite.setTexture(boardTexture, true);
        needToUpdateTexture = false;
    }
}

void Board::draw(sf::RenderTarget& window, sf::RenderStates states) const
{
    window.draw(boardSprite);
}

short Board::countCellsNormal(const sf::Vector2u& pos)
{
    // WARNING: This function assumes the following is true:
    // if (pos.x > 0 && pos.y > 0 && pos.x < board.width() - 1 && pos.y < board.height() - 1)
    // Which means that you should not use this function on cells that are on the edge of the board!
    // This is only done for performance reasons.
    short count = 0;
    sf::Vector2u startPos(pos.x - 1, pos.y - 1);
    sf::Vector2u endPos(pos.x + 1, pos.y + 1);
    sf::Vector2u tempPos;

    // Iterate through a 3x3 grid around the cell
    for (tempPos.y = startPos.y; tempPos.y <= endPos.y; ++tempPos.y)
        for (tempPos.x = startPos.x; tempPos.x <= endPos.x; ++tempPos.x)
            if (tempPos != pos) // Ignore the center cell
                count += board[readBoard](tempPos); // Increment the count if the cell is live

    return count;
}

short Board::countCellsToroidal(const sf::Vector2u& pos)
{
    short count = 0;
    sf::Vector2u tempPos;

    // Pre-calculate the ranges of cells to count
    unsigned rangeX[3] = {pos.x - 1, pos.x, pos.x + 1};
    unsigned rangeY[3] = {pos.y - 1, pos.y, pos.y + 1};
    if (pos.x == 0)
        rangeX[0] = board[readBoard].width() - 1;
    else if (pos.x == board[readBoard].width() - 1)
        rangeX[2] = 0;
    if (pos.y == 0)
        rangeY[0] = board[readBoard].height() - 1;
    else if (pos.y == board[readBoard].height() - 1)
        rangeY[2] = 0;

    // Iterate through the ranges as a wrapped-around 3x3 grid
    for (unsigned y: rangeY)
        for (unsigned x: rangeX)
            if (x != pos.x || y != pos.y) // Ignore the center cell
                count += board[readBoard](x, y); // Increment the count if the cell is live

    return count;
}

void Board::determineState(const sf::Vector2u& pos, short count)
{
    // Uses the rule set lookup table, and updates the state of the cell to the new state
    bool currentState = board[readBoard](pos);
    setCell(pos, rules.getRule(currentState, count));
}

void Board::setCell(const sf::Vector2u& pos, bool state)
{
    board[writeBoard](pos) = state;
    boardImage.setPixel(pos.x, pos.y, cellColors[state]);
    needToUpdateTexture = true;
}

bool Board::inBounds(const sf::Vector2i& pos) const
{
    return (pos.x >= 0 && pos.y >= 0 && pos.x < static_cast<int>(board[writeBoard].width()) && pos.y < static_cast<int>(board[writeBoard].height()));
}

void Board::toggle(unsigned& val) const
{
    val = !(static_cast<bool>(val));
}

sf::Rect<unsigned> Board::fixRectangle(const sf::IntRect& rect) const
{
    // The position should be the top-left corner of the rectangle,
    // and the size should be the size of the rectangle without going out of bounds.
    // Then, make sure the position and size are within bounds of the board.
    unsigned boardWidth = board[readBoard].width();
    unsigned boardHeight = board[readBoard].height();
    sf::Rect<unsigned> fixedRect(std::min(std::max(std::min(rect.left, rect.left + rect.width), 0), (int)boardWidth - 1),
                           std::min(std::max(std::min(rect.top, rect.top + rect.height), 0), (int)boardHeight - 1),
                           std::max(std::max(-rect.width, rect.width), 0),
                           std::max(std::max(-rect.height, rect.height), 0));
    if (fixedRect.left + fixedRect.width >= boardWidth)
        fixedRect.width = boardWidth - fixedRect.left;
    if (fixedRect.top + fixedRect.height >= boardHeight)
        fixedRect.height = boardHeight - fixedRect.top;
    return fixedRect;
}
