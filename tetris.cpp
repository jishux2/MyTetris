// Tetris.cpp

#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <fstream>
#include "basic_const.h"
#include "tetris.h"
#include "block.h"
#include "board.h"

extern Board board;       // 声明一个Board对象，表示游戏面板
extern Block block;       // 声明一个Block对象，表示当前控制的方块
extern Block nextBlock;   // 声明一个Block对象，表示下一个出现的方块
extern Block fallenBlock; // 声明一个Block对象，表示当前方块下落后的位置

// 定义Tetris类函数

// 默认构造函数
Tetris::Tetris()
{
    hwnd = NULL;        // 初始化控制台窗口句柄
    hdc = NULL;         // 初始化设备上下文句柄
    lastLoop = clock(); // 初始化上一次循环时间
    for (int i = 0; i < 6; i++)
    {
        lastClick[i] = clock(); // 初始化按键时间
    }
    lastDraw = clock();
    score = 0;     // 初始化分数
    level = 1;     // 初始化等级
    speed = SPEED; // 初始化速度
    lines = 0;     // 初始化消除行数
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
    isPaused = false;    // 游戏开始时不是暂停状态
    gameQuited = false;  // 游戏开始时没有退出
    gameOver = false;    // 游戏开始时没有自然结束
    isFirstBlock = true; // 游戏开始时是第一个方块
}

// 析构函数
Tetris::~Tetris()
{
    // 不做任何事情
}

// 复原游戏
void Tetris::restore()
{
    // 复原游戏数据
    score = 0;
    level = 1;
    speed = SPEED;
    lines = 0;
    drawProm = 0;
    // 清空游戏面板
    board.clearBoard();
    createBlock(); // 新游戏开始时要创建方块
}

// 初始化游戏
void Tetris::init()
{
    srand(time(NULL));
    // 把分数那块涂黑，避免上一个板块和现有的重叠
    HBRUSH hBrush = CreateSolidBrush(RGB(12, 12, 12));
    SelectObject(hdc, hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(12, 12, 12));
    SelectObject(hdc, hPen);
    Rectangle(hdc, COLS * BLOCK_SIZE + 5, 0, COLS * BLOCK_SIZE + 100, 180);
    DeleteObject(hBrush);
    DeleteObject(hPen);
    // init的时候还要把drawBtnAgain等在程序运行中会被改变与初始值不同的成员变量再重置一遍，因为tetris不会重复初始化
    lastLoop = clock(); // 初始化上一次循环时间
    for (int i = 0; i < 6; i++)
    {
        lastClick[i] = clock(); // 初始化按键时间
    }
    lastDraw = clock();
    drawAgain = 1;  // 初始化时画第一次
    _drawAgain = 1; // 进入游戏时就应该立刻画一遍了
    for (int i = 0; i < 6; i++)
    {
        drawBtnAgain[i] = 1;
    }
    gameQuited = false;  // 游戏开始时没有退出
    gameOver = false;    // 游戏开始时没有自然结束
    isFirstBlock = true; // 游戏开始时是第一个方块

    // 获取控制台窗口句柄
    hwnd = GetConsoleWindow();
    // 获取设备上下文句柄
    hdc = GetDC(hwnd);
}

// 更新游戏逻辑
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

    if (drawProm && clock() - lastDraw >= 1000)
    {
        // 提示信息的延迟消除
        HBRUSH hBrush = CreateSolidBrush(RGB(12, 12, 12));
        SelectObject(hdc, hBrush);
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(12, 12, 12));
        SelectObject(hdc, hPen);
        Rectangle(hdc, COLS * BLOCK_SIZE + 10, 360, COLS * BLOCK_SIZE + 140, 390);
        drawProm = 0;
        DeleteObject(hBrush);
        DeleteObject(hPen);
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

    // 如果没有暂停，就让方块下落
    if (!isPaused)
    {
        if (clock() - lastLoop >= speed)
        {
            // 擦除屏幕上的方块
            block.erase(hdc);
            fallenBlock.erase(hdc);
            block.moveDown(); // 方块下移一格
            lastLoop = clock();
            if (gameOver)
            { // 如果游戏已经要结束了，就只画一次背景
                board.draw(hdc);
            }
            else
            {
                // 在屏幕上画出方块
                block.draw(hdc);
                fallenBlock.project(hdc); // 绘制方块下落后的位置
            }
        }
    }
}

// 渲染游戏图形
void Tetris::render()
{
    if (drawAgain)
    {
        // 在屏幕上画出背景，已经堆积的方块，等等
        board.draw(hdc);
        drawText(COLS * BLOCK_SIZE + 10, 10, "Score: ", RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 30, to_string(score).c_str(), RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 60, "Level: ", RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 80, to_string(level).c_str(), RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 110, "Lines: ", RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 130, to_string(lines).c_str(), RGB(255, 255, 255), RGB(12, 12, 12));
        drawText(COLS * BLOCK_SIZE + 10, 480, "Next Block: ", RGB(255, 255, 255), RGB(12, 12, 12)); // 下一个方块的提示信息放到底部，更方便查看
        drawAgain = 0;
    }
}

// 处理用户输入
void Tetris::input()
{
    bool preState = isPaused; // 储存上一个暂停与否的状态
    // 如果有按键被按下
    if (_kbhit())
    {
        // 获取按键的代码
        int key = _getch();
        // 按钮信息
        int result = 0;
        // 擦除屏幕上的方块
        block.erase(hdc);
        fallenBlock.erase(hdc);
        // 根据按键的代码进行判断
        switch (key)
        {
        case 'a': // 左移
        case 'A':
            drawButton('A', RGB(255, 255, 255), RGB(12, 12, 12));
            block.moveLeft();
            lastClick[0] = clock();
            drawBtnAgain[0] = 1;
            isPaused = false; // 按下任意键都会解除暂停状态
            break;
        case 'd': // 右移
        case 'D':
            drawButton('D', RGB(255, 255, 255), RGB(12, 12, 12));
            block.moveRight();
            lastClick[1] = clock();
            drawBtnAgain[1] = 1;
            isPaused = false;
            break;
        case 's': // 加速下移
        case 'S':
            drawButton('S', RGB(255, 255, 255), RGB(12, 12, 12));
            block.moveDown();
            lastClick[2] = clock();
            drawBtnAgain[2] = 1;
            isPaused = false;
            break;
        case 'w': // 顺时针旋转
        case 'W':
            drawButton('W', RGB(255, 255, 255), RGB(12, 12, 12));
            block.rotate();
            lastClick[3] = clock();
            drawBtnAgain[3] = 1;
            isPaused = false;
            break;
        case 27: // 退出游戏
            result = MessageBoxA(hwnd, "游戏已暂停\n选择\"是\"以保存游戏；\n选择\"否\"以返回主菜单；\n或选择\"取消\"以继续游戏", "提示", MB_YESNOCANCEL | MB_ICONINFORMATION);
            switch (result)
            {
            case IDYES: // 用户选择了"是"
                // 保存游戏
                saveGame();
                break;
            case IDNO: // 用户选择了"否"
                exit();
                break;
            case IDCANCEL: // 用户选择"取消"
                break;     // 什么都不做，继续游戏
            }
            isPaused = false;
            break;
        case 32: // 下落到底部
            drawButton('\040', RGB(255, 255, 255), RGB(12, 12, 12));
            while (!block.moveDown())
            {
                // 不断下移直到不能移动为止
            }
            lastClick[4] = clock();
            drawBtnAgain[4] = 1;
            isPaused = false;
            break;
        case 'p': // 暂停
        case 'P':
            drawButton('P', RGB(255, 255, 255), RGB(12, 12, 12));
            isPaused = !isPaused; // 切换暂停状态
            lastClick[5] = clock();
            drawBtnAgain[5] = 1;
            break;
        case 'r': // 重画
        case 'R':
            for (int i = 0; i < 6; i++)
            {
                drawButton(button[i], RGB(12, 12, 12), RGB(255, 255, 255));
            }
            drawText(COLS * BLOCK_SIZE, ROWS * BLOCK_SIZE + 10, "已重画", RGB(12, 12, 12), RGB(255, 165, 0));
            drawText(10, ROWS * BLOCK_SIZE + 10, "按键规则：\040\040ESC——菜单", RGB(12, 12, 12), RGB(255, 255, 255));
            drawText(10, ROWS * BLOCK_SIZE + 40, "A——左移\040\040D——右移\040\040S——下移", RGB(12, 12, 12), RGB(255, 255, 255));
            drawText(10, ROWS * BLOCK_SIZE + 70, "W——旋转\040\040Space——下落\040\040P——暂停", RGB(12, 12, 12), RGB(255, 255, 255));
            drawText(10, ROWS * BLOCK_SIZE + 100, "**如果出现画面错乱的问题，请点击R键恢复**", RGB(12, 12, 12), RGB(255, 255, 255));
            drawAgain = true;
            _drawAgain = true;
            break;
        }
        if (!gameOver) // 如果游戏要结束了，这里也不要再画了
        {
            fallenBlock = block; // 更新下落后的位置
            fallenBlock.project(hdc);
            // 在屏幕上画出方块
            block.draw(hdc);
        }
        // 在画了方块后操作，避免被覆盖
        if (preState == false && isPaused == true) // 如果刚刚进入暂停状态，就显示提示信息
        {
            drawText(COLS * BLOCK_SIZE / 2 - 45, ROWS * BLOCK_SIZE / 2, "\040按任意键以继续\040", RGB(255, 255, 255), RGB(12, 12, 12));
        }
        if (preState == true && isPaused == false) // 如果刚刚解除暂停状态，就重画一遍
        {
            drawAgain = true;
            _drawAgain = true; // 解除暂停时马上重画一次方块
        }
    }
}

// 退出游戏
void Tetris::exit()
{
    // 释放设备上下文句柄
    ReleaseDC(hwnd, hdc);
    // 设置游戏退出标志为真
    gameQuited = true;
}

// 创建一个新的方块
void Tetris::createBlock()
{
    if (isFirstBlock)
    {
        // 随机生成一个类型给第一个方块
        int type = rand() % 7;
        // 根据给定的类型和初始位置创建一个新的方块对象
        block = Block(COLS / 2 - 2, 0, type);
        isFirstBlock = false;
    }
    else
    {
        // 将下一个类型更新为当前类型
        block = nextBlock;
    }
    gameOver = !board.isValid(block);
    if (!gameOver)
    { // 如果游戏要结束了，下面这些都不用做，尤其不要更新下一个方块，要给玩家看到为什么下一个方块导致游戏结束
        // 随机生成一个类型给下一个方块
        int nextType = rand() % 7;
        // 提前生成下一个方块对象
        nextBlock = Block(COLS / 2 - 2, 0, nextType);
        fallenBlock = block;
        drawAgain = true;  // 每次创建新方块都要再画一次
        _drawAgain = true; // 每次创建新方块都立刻重画一次，并重新计时，否则每次看到方块都是已经落下一格的
    }
}

// 播放声音效果
void Tetris::playSound(int type)
{
    // 根据声音效果的类型进行判断
    switch (type)
    {
    case 1:             // 消除一行
        Beep(440, 100); // 播放一个频率为440 Hz，持续时间为100 ms的蜂鸣声
        break;
    case 2:             // 升级
        Beep(880, 200); // 播放一个频率为880 Hz，持续时间为200 ms的蜂鸣声
        break;
    }
}

// 在屏幕上画出文本
void Tetris::drawText(int x, int y, const char *text, COLORREF bkColor, COLORREF textColor)
{
    // 设置文本颜色
    SetTextColor(hdc, textColor);
    // 设置背景颜色
    SetBkColor(hdc, bkColor);
    // 在给定的位置画出文本
    HFONT hFont = CreateFont(24, 8, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Times New Roman"));
    SelectObject(hdc, hFont);
    TextOutA(hdc, x, y, text, strlen(text));
    DeleteObject(hFont);
}

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
    // 创建一个GameState类型的对象，用于存储当前的游戏状态
    GameState gameState;
    // 将当前的分数、等级、速度、消除行数等赋值给gameState对象的相应成员变量
    gameState.score = score;
    gameState.level = level;
    gameState.speed = speed;
    gameState.lines = lines;
    // 将当前的棋盘网格数据复制给gameState对象的grid二维数组
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            gameState.grid[i][j] = board.grid[i][j];
        }
    }
    // 将当前的方块和下一个方块对象赋值给gameState对象的block和nextBlock成员变量
    gameState.block = block;
    gameState.nextBlock = nextBlock;

    // 创建一个ofstream类型的对象，用于向文件写入数据
    ofstream destFile;
    try
    {
        // 以二进制模式打开一个名为"save.dat"的文件，如果文件不存在则创建它
        destFile.open("/C++/tetris_sydney/save.dat", ios_base::binary | ios_base::out);
        // 如果打开或创建文件失败，抛出一个运行时错误异常
        if (!destFile)
        {
            throw runtime_error("Error: Failed to create a file!");
        }
        // 将gameState对象转换为字节流，写入到文件中
        destFile.write((char *)&gameState, sizeof(GameState));
        // 在屏幕上显示一条消息，提示用户游戏已经保存成功
        drawText(COLS * BLOCK_SIZE + 15, 360, "Game Saved!", RGB(255, 255, 255), RGB(12, 12, 12));
        // 记录当前的时间，用于控制提示消息的显示时长
        lastDraw = clock();
        // 表示需要在一定时间后擦除提示消息
        drawProm = 1;
    }
    catch (const std::exception &e) // 捕获异常
    {
        // 在屏幕上显示异常信息
        drawText(COLS * BLOCK_SIZE + 15, 360, e.what(), RGB(255, 255, 255), RGB(12, 12, 12));
    }
    // 关闭文件
    destFile.close();
}

// 载入游戏
void Tetris::loadGame()
{
    char *fileName = new char[256];             // 存储文件名
    char *fileAddress = new char[256];          // 存储文件地址
    strcpy(fileAddress, "/C++/tetris_sydney/"); // 将文件地址的前缀复制到fileAddress中
    // 创建一个GameState对象，用于存储从文件中读取的游戏状态
    GameState gameState;
    // 创建一个ifstream对象，并以二进制模式打开文件
    ifstream savedFile;
    cout << "请输入保存的文件名：";          // 输出提示信息
    cin.getline(fileName, strlen(fileName)); // 从标准输入读取一行字符串，作为文件名
    strcat(fileAddress, fileName);           // 将文件名拼接到文件地址后面
    try
    {
        savedFile.open(fileAddress, ios_base::binary | ios_base::in); // 以二进制和输入模式打开指定的文件
        if (!savedFile)                                               // 如果打开失败
        {
            throw runtime_error("Error: Failed to load the file!"); // 抛出一个运行时错误异常，包含错误信息
        }
        // 从文件中读取sizeof(GameState)个字节，存储到gameState对象中
        savedFile.read((char *)&gameState, sizeof(GameState));
        // 将读取到的游戏状态赋值给当前的游戏状态
        score = gameState.score;
        level = gameState.level;
        speed = gameState.speed;
        lines = gameState.lines;
        for (int i = 0; i < ROWS; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                board.grid[i][j] = gameState.grid[i][j]; // 将读取到的棋盘格子赋值给当前的棋盘格子
            }
        }
        block = gameState.block;         // 当前方块
        nextBlock = gameState.nextBlock; // 次方块
        fallenBlock = block;             // 当前方块在底部的投影
        lastDraw = clock();
        drawProm = 1;
    }
    catch (const std::exception &e) // 捕获异常
    {
        // 在屏幕上绘制异常信息，使用白色文字和黑色背景
        drawText(100, 360, e.what(), RGB(12, 12, 12), RGB(255, 255, 255));
        gameQuited = 1;
    }
    // 关闭文件流对象
    savedFile.close();
    delete[] fileName; // 释放动态分配的字符数组内存空间
}

Tetris tetris; // The Tetris game object
