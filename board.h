#ifndef _BOARD_H
#define _BOARD_H

#include <windows.h>
#include "basic_const.h"
#include "block.h"

// Define the Board class
class Board
{
private:
    char grid[ROWS][COLS]; // The grid of the board
    HBRUSH hBrush;         // The handle of the brush
    HPEN hPen;             // The handle of the pen
public:
    Board();                        // The default constructor
    ~Board();                       // The destructor
    bool isFull(int row);           // The method to check if a row is full
    bool isEmpty(int row);          // The method to check if a row is empty
    void clearRow(int row);         // The method to clear a row
    void dropRows(int row);         // The method to drop the rows above a row
    void clearBoard();              // The method to clear the board
    bool isValid(Block &block);     // The method to check if a block is valid on the board
    void placeBlock(Block &block);  // The method to place a block on the board
    void removeBlock(Block &block); // The method to remove a block from the board
    void draw(HDC hdc);             // The method to draw the blocks accumulated ,the background and the next block on the screen
    friend class Block;             // Declare Block as a friend class
    friend class Tetris;            // Declare Tetris as a friend class
};

#endif
