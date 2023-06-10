// Tetris.cpp
// This is the implementation file of the Tetris game
// It contains the definitions of the Block, Board and Tetris classes' functions

#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <fstream>
#include "basic_const.h"
#include "tetris.h"
#include "block.h"
#include "board.h"

extern Board board;
extern Block block;
extern Block nextBlock;
extern Block fallenBlock;

// Define the Tetris class' functions

// The default constructor
Tetris::Tetris()
{
    hwnd = NULL;
    hdc = NULL;
    lastLoop = clock(); // Initialize the last loop time
    for (int i = 0; i < 6; i++)
    {
        lastClick[i] = clock(); // 初始化时按键时间
    }
    lastdraw = clock();
    score = 0;
    level = 1;
    speed = SPEED;
    lines = 0;
    // 初始化六种按键
    button[0] = 'A';
    button[1] = 'D';
    button[2] = 'S';
    button[3] = 'W';
    button[4] = '\040';
    button[5] = 'P';
    drawAgain = 1; // 初始化时画第一次
    _drawAgain = 0;
    drawProm = 0;
    for (int i = 0; i < 6; i++)
    {
        drawBtnAgain[i] = 1; // 初始化时都画一次，而不要和drawAgain一起判断
    }
    isPaused = false;   // 游戏开始时不是暂停状态
    gameQuited = false; // 游戏开始时没有退出
}

// The destructor
Tetris::~Tetris()
{
    // Do nothing
}

// The method to initialize the game
void Tetris::init()
{
    // init的时候还要把drawBtnAgain等在程序运行中会被改变与初始值不同的成员变量再重置一遍，因为tetris不会重复初始化
    lastLoop = clock(); // Initialize the last loop time
    for (int i = 0; i < 6; i++)
    {
        lastClick[i] = clock(); // 初始化时按键时间
    }
    lastdraw = clock();
    score = 0;
    level = 1;
    speed = SPEED;
    lines = 0;
    drawAgain = 1; // 初始化时画第一次
    _drawAgain = 0;
    drawProm = 0;
    for (int i = 0; i < 6; i++)
    {
        drawBtnAgain[i] = 1;
    }
    gameQuited = false; // 游戏开始时没有退出

    // Get the handle of the console window
    hwnd = GetConsoleWindow();
    // Get the handle of the device context
    hdc = GetDC(hwnd);
    drawText(10, ROWS * BLOCK_SIZE + 10, "按键规则：\040\040ESC——返回主菜单", RGB(12, 12, 12), RGB(255, 255, 255));
    drawText(10, ROWS * BLOCK_SIZE + 40, "A——左移\040\040D——右移\040\040S——下移", RGB(12, 12, 12), RGB(255, 255, 255));
    drawText(10, ROWS * BLOCK_SIZE + 70, "W——旋转\040\040Space——下落\040\040P——暂停", RGB(12, 12, 12), RGB(255, 255, 255));
    // Clear the board
    board.clearBoard();
    createBlock();
}

// The method to update the game logic
void Tetris::update()
{
#if 0
    static bool drawAtFirst = true; // 设置控制台大小会导致第一次画背景失效，需要在这里手动画一次也仅画一次
#endif
    // 重画按键
    // 需要让按键变色持续一段时间，否则观察不到，所以引入lastClick变量，记录上一次按键的时间
    // 对每一个按键分别判断，实现流畅的视觉效果

    for (int i = 0; i < 6; i++)
    {
        if (drawBtnAgain[i] && clock() - lastClick[i] >= 100)
        {

            drawButton(button[i], RGB(12, 12, 12), RGB(255, 255, 255));
            drawBtnAgain[i] = 0;
        }
    }

    if (drawProm && clock() - lastdraw >= 1000)
    {
        // 提示信息的延迟消除
        HBRUSH hBrush = CreateSolidBrush(RGB(12, 12, 12));
        SelectObject(hdc, hBrush);
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(12, 12, 12));
        SelectObject(hdc, hPen);
        Rectangle(hdc, COLS * BLOCK_SIZE + 10, 360, COLS * BLOCK_SIZE + 140, 390);
        drawProm = 0;
    }

    if (_drawAgain)
    {
        // 重画方块并且重置时间
        block.erase(hdc);
        fallenBlock.erase(hdc);
        lastLoop = clock();
        block.draw(hdc);
        fallenBlock.project(hdc);
        _drawAgain = 0;
    }
    // Move the block down
    if (!isPaused)
    {
        if (clock() - lastLoop >= speed)
        {
            // Erase the block from the screen
            block.erase(hdc); // 可不可以在必要时才擦除呢，这样可以减少视觉干扰
            fallenBlock.erase(hdc);
            block.moveDown();
            lastLoop = clock();
            // Draw the block on the screen
            block.draw(hdc);
            fallenBlock.project(hdc);
#if 0
            if (drawAtFirst)
            {
                // 写入按键提示
                drawText(10, ROWS * BLOCK_SIZE + 10, "按键规则：\040\040ESC——返回主菜单", RGB(12, 12, 12), RGB(255, 255, 255));
                drawText(10, ROWS * BLOCK_SIZE + 40, "A——左移\040\040D——右移\040\040S——下移", RGB(12, 12, 12), RGB(255, 255, 255));
                drawText(10, ROWS * BLOCK_SIZE + 70, "W——旋转\040\040Space——下落\040\040P——暂停", RGB(12, 12, 12), RGB(255, 255, 255));
                // 画出六个按键
                for (int i = 0; i < 6; i++)
                {
                    drawButton(button[i],RGB(12, 12, 12), RGB(255, 255, 255));
                }
                // draw the background ,the blocks accumulated ,etc. on the screen
                board.draw(hdc);
                drawText(COLS * BLOCK_SIZE + 10, 10, "Score: ", RGB(255, 255, 255), RGB(12, 12, 12));
                drawText(COLS * BLOCK_SIZE + 10, 30, to_string(score).c_str(), RGB(255, 255, 255),RGB(12, 12, 12));
                drawText(COLS * BLOCK_SIZE + 10, 60, "Level: ", RGB(255, 255, 255), RGB(12, 12, 12));
                drawText(COLS * BLOCK_SIZE + 10, 80, to_string(level).c_str(), RGB(255, 255, 255), RGB(12, 12, 12));
                drawText(COLS * BLOCK_SIZE + 10, 110, "Lines: ", RGB(255, 255, 255),RGB(12, 12, 12));
                drawText(COLS * BLOCK_SIZE + 10, 130, to_string(lines).c_str(), RGB(255, 255, 255), RGB(12, 12, 12));
                drawText(COLS * BLOCK_SIZE + 10, 480, "Next Block: ", RGB(255, 255, 255), RGB(12, 12, 12)); // 下一个方块的提示信息放到底部，更方便查看
                drawAtFirst = false;
            }
#endif
        }
    }
}

// The method to render the game graphics
void Tetris::render()
{
    if (drawAgain)
    {
        // draw the background ,the blocks accumulated ,etc. on the screen
        board.draw(hdc);
        drawText(COLS * BLOCK_SIZE + 10, 10, "Score: ", RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 30, to_string(score).c_str(), RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 60, "Level: ", RGB(255, 255, 255),RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 80, to_string(level).c_str(), RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 110, "Lines: ", RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 130, to_string(lines).c_str(), RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 480, "Next Block: ", RGB(255, 255, 255), RGB(12, 12, 12)); // 下一个方块的提示信息放到底部，更方便查看
        drawAgain = 0;
    }
}

// The method to handle the user input
void Tetris::input()
{
    bool preState = isPaused; // 储存上一个暂停与否的状态
    // If there is a key pressed
    if (_kbhit())
    {
        // Get the key code
        int key = _getch();
        // 按钮信息
        int result = 0;
        // Erase the block from the screen
        block.erase(hdc);
        fallenBlock.erase(hdc);
        // Switch on the key code
        switch (key)
        {
        case 'a': // Move left
        case 'A':
            drawButton('A', RGB(255, 255, 255), RGB(12, 12, 12));
            block.moveLeft();
            lastClick[0] = clock();
            drawBtnAgain[0] = 1;
            isPaused = 0;
            break;
        case 'd': // Move right
        case 'D':
            drawButton('D', RGB(255, 255, 255), RGB(12, 12, 12));
            block.moveRight();
            lastClick[1] = clock();
            drawBtnAgain[1] = 1;
            isPaused = 0;
            break;
        case 's': // Move down faster
        case 'S':
            drawButton('S', RGB(255, 255, 255), RGB(12, 12, 12));
            block.moveDown();
            lastClick[2] = clock();
            drawBtnAgain[2] = 1;
            isPaused = 0;
            break;
        case 'w': // Rotate clockwise
        case 'W':
            drawButton('W', RGB(255, 255, 255), RGB(12, 12, 12));
            block.rotate();
            lastClick[3] = clock();
            drawBtnAgain[3] = 1;
            isPaused = 0;
            break;
        case 27: // Exit the game
            result = MessageBoxA(hwnd, "游戏已暂停\n选择\"是\"以保存游戏并继续；\n或选择\"否\"以返回主菜单", "提示", MB_YESNO | MB_ICONINFORMATION);
            switch (result)
            {
            case IDYES: // User chose "Yes"
                // Restart the game
                saveGame();
                break;
            case IDNO: // User chose "No"
                exit();
                break;
            }
            isPaused = 0;
            break;
        case 32: // Drop to the bottom
            drawButton('\040', RGB(255, 255, 255), RGB(12, 12, 12));
            while (!block.moveDown())
            {
                // 好神奇的逻辑
            }
            lastClick[4] = clock();
            drawBtnAgain[4] = 1;
            isPaused = 0;
            break;
        case 'p': // Pause
        case 'P':
            drawButton('P', RGB(255, 255, 255), RGB(12, 12, 12));
            isPaused = !isPaused;
            lastClick[5] = clock();
            drawBtnAgain[5] = 1;
            break;
        }
        fallenBlock = block;
        // Draw the block on the screen
        block.draw(hdc);
        fallenBlock.project(hdc);
        // 在画了方块后操作，避免被覆盖
        if (preState == 0 && isPaused == 1) // mark,mark.未完成
        {
            drawText(COLS * BLOCK_SIZE / 2 - 45, ROWS * BLOCK_SIZE / 2, "\040按任意键以继续\040", RGB(255, 255, 255), RGB(12, 12, 12));
        }
        if (preState == 1 && isPaused == 0)
        {
            drawAgain = 1;  // 解除暂停后要再画一遍
            _drawAgain = 1; // 解除暂停时马上重画一次方块
        }
    }
}

// The method to exit the game
void Tetris::exit()
{
    // Release the device context
    ReleaseDC(hwnd, hdc);
    // Exit the program
    gameQuited = 1;
}

// The method to check if the game is over
bool Tetris::isGameOver()
{
    // Return the result of the board's isGameOver method
    return board.isGameOver();
}

// The method to create a new block
void Tetris::createBlock()
{
    static bool isFirstBlock = true; // Check if it is the first block
    if (isFirstBlock)
    {
        // Randomize a type for the first block
        int type = rand() % 7;
        // Create a new block object with the given type and initial position
        block = Block(COLS / 2 - 2, 0, type);
        isFirstBlock = false;
    }
    else
    {
        // Update the current type with the next type
        block = nextBlock;
    }
    // Randomize a type for the next block
    int nextType = rand() % 7;
    // Formulate the next block in advance
    nextBlock = Block(COLS / 2 - 2, 0, nextType);
    fallenBlock = block;
    drawAgain = 1;  // 每次创建新方块都要再画一次
    _drawAgain = 1; // 每次创建新方块都立刻重画一次，并重新计时，否则每次看到方块都是已经落下一格的
    // Check if the new block is valid on the board[mark]原来这里也会导致强退
    // if (!board.isValid(block))
    // {
    //     // If not, exit the game
    //     exit();
    // }
}

// The method to play sound effects
void Tetris::playSound(int type)
{
    // Switch on the type of sound effect
    switch (type)
    {
    case 1:             // Clearing a row
        Beep(440, 100); // Play a beep sound with frequency 440 Hz and duration 100 ms
        break;
    case 2:             // Leveling up
        Beep(880, 200); // Play a beep sound with frequency 880 Hz and duration 200 ms
        break;
    }
}

// The method to draw text on the screen
void Tetris::drawText(int x, int y, const char *text, COLORREF bkColor, COLORREF textColor)
{
    // Set the text color to black
    SetTextColor(hdc, textColor);
    // Set the background color to white
    SetBkColor(hdc, bkColor);
    // Draw the text at the given position
    HFONT hFont = CreateFont(24, 8, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Times New Roman"));
    SelectObject(hdc, hFont);
    TextOutA(hdc, x, y, text, strlen(text));
    DeleteObject(hFont);
}

#if 0
void Tetris::drawButton()
{
    // 绘制按钮，实现动态效果
    HBRUSH hBrush = CreateSolidBrush(RGB(12, 12, 12));
    SelectObject(hdc, hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    SelectObject(hdc, hPen);
    // Create a font object with height 24 and width 8
    HFONT hFont = CreateFont(24, 8, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Times New Roman"));
    // Select the font object into the device context
    SelectObject(hdc, hFont);
    RoundRect(hdc, COLS * BLOCK_SIZE + 10, 255, COLS * BLOCK_SIZE + 50, 295, 5, 5);   // A
    RoundRect(hdc, COLS * BLOCK_SIZE + 60, 255, COLS * BLOCK_SIZE + 100, 295, 5, 5);  // S
    RoundRect(hdc, COLS * BLOCK_SIZE + 110, 255, COLS * BLOCK_SIZE + 150, 295, 5, 5); // D
    RoundRect(hdc, COLS * BLOCK_SIZE + 60, 205, COLS * BLOCK_SIZE + 100, 245, 5, 5);  // W
    RoundRect(hdc, COLS * BLOCK_SIZE + 10, 305, COLS * BLOCK_SIZE + 150, 345, 5, 5);  // Space
    Ellipse(hdc, COLS * BLOCK_SIZE + 110, 205, COLS * BLOCK_SIZE + 150, 245);         // P
    //  Set the text color to black
    SetTextColor(hdc, RGB(255, 255, 255));
    // Set the background color to white
    SetBkColor(hdc, RGB(12, 12, 12));
    // Draw the text at the given position
    TextOutA(hdc, COLS * BLOCK_SIZE + 25, 265, "A", strlen("A"));
    TextOutA(hdc, COLS * BLOCK_SIZE + 75, 265, "S", strlen("S"));
    TextOutA(hdc, COLS * BLOCK_SIZE + 125, 265, "D", strlen("D"));
    TextOutA(hdc, COLS * BLOCK_SIZE + 70, 215, "W", strlen("W"));
    TextOutA(hdc, COLS * BLOCK_SIZE + 125, 215, "P", strlen("P"));
    TextOutA(hdc, COLS * BLOCK_SIZE + 50, 315, "SPACE", strlen("SPACE"));
    // Delete the font object
    DeleteObject(hFont);
    // Delete the brush and pen objects
    DeleteObject(hBrush);
    DeleteObject(hPen);
}
#endif

void Tetris::drawButton(char button, COLORREF bkgColor, COLORREF textColor)
{
    // 绘制按钮，实现动态效果
    HBRUSH hBrush = CreateSolidBrush(bkgColor);
    SelectObject(hdc, hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
    SelectObject(hdc, hPen);
    // Create a font object with height 24 and width 8
    HFONT hFont = CreateFont(24, 8, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Times New Roman"));
    // Select the font object into the device context
    SelectObject(hdc, hFont);
    //  Set the text color to black
    SetTextColor(hdc, textColor);
    // Set the background color to white
    SetBkColor(hdc, bkgColor);
    // Draw the text at the given position
    switch (button)
    {
    case 'A':
        RoundRect(hdc, COLS * BLOCK_SIZE + 10, 255, COLS * BLOCK_SIZE + 50, 295, 5, 5); // A
        TextOutA(hdc, COLS * BLOCK_SIZE + 25, 265, "A", strlen("A"));
        break;
    case 'D':
        RoundRect(hdc, COLS * BLOCK_SIZE + 110, 255, COLS * BLOCK_SIZE + 150, 295, 5, 5); // D
        TextOutA(hdc, COLS * BLOCK_SIZE + 125, 265, "D", strlen("D"));
        break;
    case 'S':
        RoundRect(hdc, COLS * BLOCK_SIZE + 60, 255, COLS * BLOCK_SIZE + 100, 295, 5, 5); // S
        TextOutA(hdc, COLS * BLOCK_SIZE + 75, 265, "S", strlen("S"));
        break;
    case 'W':
        RoundRect(hdc, COLS * BLOCK_SIZE + 60, 205, COLS * BLOCK_SIZE + 100, 245, 5, 5); // W
        TextOutA(hdc, COLS * BLOCK_SIZE + 70, 215, "W", strlen("W"));
        break;
    case '\040':
        RoundRect(hdc, COLS * BLOCK_SIZE + 10, 305, COLS * BLOCK_SIZE + 150, 345, 5, 5); // Space
        TextOutA(hdc, COLS * BLOCK_SIZE + 50, 315, "SPACE", strlen("SPACE"));
        break;
    case 'P':
        Ellipse(hdc, COLS * BLOCK_SIZE + 110, 205, COLS * BLOCK_SIZE + 150, 245); // P
        TextOutA(hdc, COLS * BLOCK_SIZE + 125, 215, "P", strlen("P"));
        break;
    default:
        break;
    }
    // Delete the font object
    DeleteObject(hFont);
    // Delete the brush and pen objects
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

// 保存游戏
void Tetris::saveGame()
{
    // Create a GameState object and assign the current game state to it
    GameState gameState;
    gameState.score = score;
    gameState.level = level;
    gameState.speed = speed;
    gameState.lines = lines;
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            gameState.grid[i][j] = board.grid[i][j];
        }
    }
    gameState.block = block;
    gameState.nextBlock = nextBlock;

    // Create an destFiletream object and open a file named "save.dat" in binary mode
    ofstream destFile;
    try
    {
        destFile.open("/C++/tetris_sydney/save.dat", ios_base::binary | ios_base::out);
        if (!destFile)
        {
            throw runtime_error("Error: Failed to create a file!");
        }
        // Write the GameState object to the file
        destFile.write((char *)&gameState, sizeof(GameState));
        // Display a message to indicate the game is saved successfully
        drawText(COLS * BLOCK_SIZE + 15, 360, "Game Saved!", RGB(255, 255, 255), RGB(12, 12, 12));
        lastdraw = clock();
        drawProm = 1;
    }
    catch (const std::exception &e)
    {
        drawText(COLS * BLOCK_SIZE + 15, 360, e.what(), RGB(255, 255, 255),RGB(12, 12, 12));
    }
    // Close the file
    destFile.close();
}

// 载入游戏
void Tetris::loadGame()
{
    char *fileName = new char[256];
    char *fileAddress = new char[256];
    strcpy(fileAddress, "/C++/tetris_sydney/");
    // Create a GameState object to store the loaded game state
    GameState gameState;
    // Create an ifstream object and open a file named "save.dat" in binary mode
    ifstream savedFile;
    cout << endl
         << "请输入保存的文件名：";
    cin.getline(fileName, strlen(fileName));
    strcat(fileAddress, fileName);
    try
    {
        savedFile.open(fileAddress, ios_base::binary | ios_base::in);
        if (!savedFile)
        {
            throw runtime_error("Error: Failed to load the file!");
        }
        // Write the GameState object to the file
        savedFile.read((char *)&gameState, sizeof(GameState));
        // Assign the loaded game state to the current game state
        score = gameState.score;
        level = gameState.level;
        speed = gameState.speed;
        lines = gameState.lines;
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                board.grid[i][j] = gameState.grid[i][j];
            }
        }
        block = gameState.block;
        nextBlock = gameState.nextBlock;
        fallenBlock = block;
    }
    catch (const std::exception &e)
    {
        drawText(COLS * BLOCK_SIZE + 15, 360, e.what(), RGB(255, 255, 255), RGB(12, 12, 12));
    }
    // Close the file
    savedFile.close();
    delete[] fileName;
}

Tetris tetris; // The Tetris game object
