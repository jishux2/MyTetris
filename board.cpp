#include "board.h"
#include "tetris.h"
#include "basic_const.h"

extern Tetris tetris;
extern Block nextBlock;

// Define the Board class' functions

// The default constructor
Board::Board()
{
    // Initialize the grid to empty cells
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            grid[i][j] = 'N';
        }
    }
}

// The destructor
Board::~Board()
{
    // Do nothing
}

// The method to check if a row is full
bool Board::isFull(int row)
{
    // Loop through each column of the row
    for (int j = 0; j < COLS; j++)
    {
        // If there is an empty cell, return false
        if (grid[row][j] == 'N')
            return false;
    }
    // If there is no empty cell, return true
    return true;
}

// The method to check if a row is empty
bool Board::isEmpty(int row)
{
    // Loop through each column of the row
    for (int j = 0; j < COLS; j++)
    {
        // If there is a filled cell, return false
        if (grid[row][j] != 'N')
            return false;
    }
    // If there is no filled cell, return true
    return true;
}

// The method to clear a row
void Board::clearRow(int row) // 注意这个函数在init函数中就调用了
{
    // Loop through each column of the row
    for (int j = 0; j < COLS; j++)
    {
        // Set the cell to empty
        grid[row][j] = 'N';
    }
    // Play the sound effect for clearing a row
    //    tetris.playSound(1);
}

// The method to drop the rows above a row
void Board::dropRows(int row)
{
    // Loop through each row above the given row
    for (int i = row - 1; i >= 0; i--)
    {
        // If the row is not empty
        if (!isEmpty(i))
        {
            // Loop through each column of the row
            for (int j = 0; j < COLS; j++)
            {
                // Copy the cell value to the row below
                grid[i + 1][j] = grid[i][j];
                // Set the cell to empty
                grid[i][j] = 'N';
            }
        }
    }
}

// The method to clear the board
void Board::clearBoard()
{
    // Loop through each row of the board
    for (int i = 0; i < ROWS; i++)
    {
        // Clear the row
        clearRow(i);
    }
}

// The method to check if the game is over
bool Board::isGameOver()
{
    // Loop through each column of the top row
    for (int j = 0; j < COLS; j++)
    {
        // If there is a filled cell, return true
        if (grid[0][j] != 'N')
            return true;
    }
    // If there is no filled cell, return false
    return false;
}

// The method to check if a block is valid on the board
bool Board::isValid(Block &block)
{
    // Loop through each cell of the block
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // If the cell is filled
            if (block.shape[i][j] == '1')
            {
                // Calculate the row and column index of the cell on the board
                int row = block.y + i;
                int col = block.x + j;
                // Check if the index is out of bounds or the cell is occupied
                if (row < 0 || row >= ROWS || col < 0 || col >= COLS || grid[row][col] != 'N')
                {
                    // If yes, return false
                    return false;
                }
            }
        }
    }
    // If no, return true
    return true;
}

// The method to place a block on the board
void Board::placeBlock(Block &block)
{
    // Loop through each cell of the block
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // If the cell is filled
            if (block.shape[i][j] == '1')
            {
                // Calculate the row and column index of the cell on the board
                int row = block.y + i;
                int col = block.x + j;
                // Set the cell to filled
                grid[row][col] = (char)(block.type + '0'); // 把堆积方块的颜色设为与之前相同
            }
        }
    }
    int count = 0; // keep a record of the number of rows cleared
    // Loop through each row of the board from bottom to top
    for (int i = ROWS - 1; i >= 0; i--)
    {
        // If the row is full
        while (isFull(i)) // 将if改为while，实现连续消除
        {
            count++;
            // Clear the row
            clearRow(i);
            // Drop the rows above it
            dropRows(i);
            // Increase the score by 10 points
            tetris.score += 10 * count; // 多行消除奖励机制
            // Increase the number of lines cleared by 1
            tetris.lines++;
            // 由于引入了奖励机制，需要循环判断是否升级
            while (tetris.score / LEVELUP >= tetris.level)
            {
                // Increase the level by 1
                tetris.level++;
                // Decrease the speed by 50 milliseconds
                tetris.speed -= 50;
                // Play the sound effect for leveling up
                tetris.playSound(2);
            }
        }
    }
}

// The method to remove a block from the board
void Board::removeBlock(Block &block)
{
    // Loop through each cell of the block
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // If the cell is filled
            if (block.shape[i][j] == '1')
            {
                // Calculate the row and column index of the cell on the board
                int row = block.y + i;
                int col = block.x + j;
                // Set the cell to empty
                grid[row][col] = 'N';
            }
        }
    }
}

// The method to draw the board on the screen
void Board::draw(HDC hdc)
{
    // 画出下一个方块和背景
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (nextBlock.shape[i][j] == '1')
            {
                hBrush = CreateSolidBrush(nextBlock.color);
                SelectObject(hdc, hBrush);
                hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                SelectObject(hdc, hPen);
                int px = COLS * BLOCK_SIZE + 15 + j * BLOCK_SIZE * 2 / 3;
                int py = 510 + i * BLOCK_SIZE * 2 / 3;
                Rectangle(hdc, px, py,
                          px + BLOCK_SIZE * 2 / 3,
                          py + BLOCK_SIZE * 2 / 3);
                DeleteObject(hBrush);
                DeleteObject(hPen);
            }
            else
            {
                // Set the brush color to gray[mark]
                hBrush = CreateSolidBrush(RGB(128, 128, 128));
                SelectObject(hdc, hBrush);
                // Set the pen color to black
                hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                SelectObject(hdc, hPen);
                int px = COLS * BLOCK_SIZE + 15 + j * BLOCK_SIZE * 2 / 3;
                int py = 510 + i * BLOCK_SIZE * 2 / 3;
                Rectangle(hdc, px, py,
                          px + BLOCK_SIZE * 2 / 3,
                          py + BLOCK_SIZE * 2 / 3);
                DeleteObject(hBrush);
                DeleteObject(hPen);
            }
        }
    }

    // Loop through each cell of the board[mark]检查一下到底哪里出错了
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            // If the cell is filled
            if (grid[i][j] != 'N')
            {
                // 画出堆积的方块(颜色一致)
                hBrush = CreateSolidBrush(COLOR[(int)(grid[i][j] - '0')]);
                SelectObject(hdc, hBrush);
                // Set the pen color to gray.感觉还是要有点区分度的
                hPen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
                SelectObject(hdc, hPen);
                // Calculate the pixel coordinates of the cell
                int px = j * BLOCK_SIZE;
                int py = i * BLOCK_SIZE;
                // Draw a rectangle with the brush and pen color
                Rectangle(hdc, px, py, px + BLOCK_SIZE, py + BLOCK_SIZE);
                // Delete the brush and pen objects
                DeleteObject(hBrush);
                DeleteObject(hPen);
            }
            // 如果不是已经的堆积的方块，即背景
            else
            {
                // 画出背景辅助线(顺便也是画出背景了)
                //  Set the brush color to gray(background color)
                hBrush = CreateSolidBrush(RGB(128, 128, 128));
                SelectObject(hdc, hBrush);
                // Set the pen color to black
                hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
                SelectObject(hdc, hPen);
                // Calculate the pixel coordinates of the cell
                int px = j * BLOCK_SIZE;
                int py = i * BLOCK_SIZE;
                // Draw a rectangle with the brush and pen color
                Rectangle(hdc, px, py, px + BLOCK_SIZE, py + BLOCK_SIZE);
                // Delete the brush and pen objects
                DeleteObject(hBrush);
                DeleteObject(hPen);
            }
        }
    }
}

Board board; // The game board object