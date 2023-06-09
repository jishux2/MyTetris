#ifndef _BLOCK_H
#define _BLOCK_H

#include <windows.h> // 包含Windows API库，用于图形界面和声音

// Define the Block class
class Block
{
private:
    int x;            // The x coordinate of the block's center
    int y;            // The y coordinate of the block's center
    int type;         // The type of the block (0-6)
    int state;        // The state of the block (0-3)
    char shape[4][4]; // The shape of the block
    COLORREF color;   // The color of the block[mark]
    HBRUSH hBrush;    // The handle of the brush
    HPEN hPen;        // The handle of the pen
public:
    Block();                       // The default constructor
    Block(int x, int y, int type); // The parameterized constructor
    ~Block();                      // The destructor
    void rotate();                 // The method to rotate the block clockwise
    void moveLeft();               // The method to move the block left
    void moveRight();              // The method to move the block right
    bool moveDown();               // The method to move the block down.为了实现快速下降的逻辑，这里改为返回bool类型，之后看看有没有别的替代方法
    void draw(HDC hdc);            // The method to draw the block on the screen
    void project(HDC hdc);         // The method to draw the reflection of the block on the bottom
    void erase(HDC hdc);           // The method to erase the block from the screen
    friend class Board;            // Declare Board as a friend class
    friend class Tetris;           // Declare Tetris as a friend class
};

#endif