#include "block.h"
#include "board.h"
#include "tetris.h"
#include "basic_const.h"

extern Board board;
extern Tetris tetris;

// The default constructor
Block::Block()
{
    x = 0;
    y = 0;
    type = 0;
    state = 0;
    color = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            shape[i][j] = '0';
        }
    }
}

// The parameterized constructor
Block::Block(int x, int y, int type)
{
    hBrush = NULL;
    hPen = NULL;
    this->x = x;
    this->y = y;
    this->type = type;
    this->state = rand() % 4;  // Randomize the initial state
    this->color = COLOR[type]; // Set the color according to the type
    // Copy the shape from the SHAPE array according to the type and state
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            shape[i][j] = SHAPE[type][state][i][j];
        }
    }
}

// The destructor
Block::~Block()
{
    // Do nothing
}

// The method to rotate the block clockwise
void Block::rotate()
{
    // Save the current state and shape
    int oldState = state;
    char oldShape[4][4];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            oldShape[i][j] = shape[i][j];
        }
    }
    // Update the state and shape
    state = (state + 1) % 4;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            shape[i][j] = SHAPE[type][state][i][j];
        }
    }
    // Check if the new shape is valid on the board
    if (!board.isValid(*this))
    {
        // If not, restore the old state and shape
        state = oldState;
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                shape[i][j] = oldShape[i][j];
            }
        }
    }
}

// The method to move the block left
void Block::moveLeft()
{
    // Save the current x coordinate
    int oldX = x;
    // Update the x coordinate
    x--;
    // Check if the new position is valid on the board
    if (!board.isValid(*this))
    {
        // If not, restore the old x coordinate
        x = oldX;
    }
}

// The method to move the block right
void Block::moveRight()
{
    // Save the current x coordinate
    int oldX = x;
    // Update the x coordinate
    x++;
    // Check if the new position is valid on the board
    if (!board.isValid(*this))
    {
        // If not, restore the old x coordinate
        x = oldX;
    }
}

// The method to move the block down
bool Block::moveDown()
{
    // Save the current y coordinate
    int oldY = y;
    // Update the y coordinate
    y++;
    bool isBottom = !board.isValid(*this);
    // Check if the new position is valid on the board
    if (isBottom)
    {
        // If not, restore the old y coordinate
        y = oldY;
        // Place the block on the board
        board.placeBlock(*this);
        // Create a new block
        tetris.createBlock();
    }
    return isBottom;
}

// The method to draw the block on the screen
void Block::draw(HDC hdc)
{
    // Set the brush color according to the block color
    hBrush = CreateSolidBrush(color);
    SelectObject(hdc, hBrush);
    // Set the pen color to black
    hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    SelectObject(hdc, hPen);
    // Draw each cell of the block according to its shape and position
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (shape[i][j] == '1')
            {
                // Calculate the pixel coordinates of the cell
                int px = (x + j) * BLOCK_SIZE;
                int py = (y + i) * BLOCK_SIZE;
                // Draw a rectangle with the brush and pen color
                Rectangle(hdc, px, py, px + BLOCK_SIZE, py + BLOCK_SIZE);
            }
        }
    }
    // Delete the brush and pen objects
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

// The method to draw the reflection of the block on the bottom
void Block::project(HDC hdc)
{
    // 实现投影效果，方便玩家预测落处
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, BLOCK_SIZE, BLOCK_SIZE);
    HDC hMemDC = CreateCompatibleDC(hdc);
    hBrush = CreateSolidBrush(color);
    SelectObject(hMemDC, hBrush);
    hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));//还是设为实线效果更好
    SelectObject(hMemDC, hPen);
    SelectObject(hMemDC, hBitmap);
    int oldY = y;
    y++;
    while (board.isValid(*this))
    {
        oldY = y;
        y++;
    }
    y = oldY;
    BLENDFUNCTION bf;
    bf.BlendOp = AC_SRC_OVER;
    bf.BlendFlags = 0;
    bf.AlphaFormat = 0;
    bf.SourceConstantAlpha = 75; // 透明度包括画笔和画刷
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (shape[i][j] == '1')
            {
                int px = (x + j) * BLOCK_SIZE;
                int py = (y + i) * BLOCK_SIZE;
                Rectangle(hMemDC, 0, 0, BLOCK_SIZE, BLOCK_SIZE);                                           // Draw the projection on the memory device context using relative coordinates
                AlphaBlend(hdc, px, py, BLOCK_SIZE, BLOCK_SIZE, hMemDC, 0, 0, BLOCK_SIZE, BLOCK_SIZE, bf); // Copy the bitmap to the screen device context using absolute coordinates
            }
        }
    }
    DeleteObject(hBrush);
    DeleteObject(hPen);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);
}

// The method to erase the block from the screen
void Block::erase(HDC hdc)
{
    // Set the brush color to white[mark]和背景颜色相同，先设为gray
    hBrush = CreateSolidBrush(RGB(128, 128, 128));
    SelectObject(hdc, hBrush);
    // Set the pen color to white(避免辅助线被擦除)
    hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    SelectObject(hdc, hPen);
    // Erase each cell of the block according to its shape and position
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (shape[i][j] == '1')
            {
                // Calculate the pixel coordinates of the cell
                int px = (x + j) * BLOCK_SIZE;
                int py = (y + i) * BLOCK_SIZE;
                // Draw a rectangle with the brush and pen color[mark]这里是要清除，还而不是画出原始图形
                Rectangle(hdc, px, py, px + BLOCK_SIZE, py + BLOCK_SIZE);
            }
        }
    }
    // Delete the brush and pen objects
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

Block block;       // The current falling block object
Block nextBlock;   // The next block object to fall
Block fallenBlock; // The reflection of the falling block on the bottom