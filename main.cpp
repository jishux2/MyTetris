#include <iostream>

#include <ctime>
#include "board.h" //不可以再引入block.h了
#include "tetris.h"

extern Tetris tetris;

using namespace std;

void startNewGame()
{
    srand(time(NULL)); // Initialize the random seed
    tetris.init();     // Initialize the game
    while (true)
    { // The game loop[mark]换了一下位置，先render再update，看一下效果如何
        // 我真傻，真的，怎么把时间间隔的判断放这里，应该听Sydney的，用它来控制moveDown函数
        tetris.render(); // Render the game graphics
        tetris.update(); // Update the game logic
        tetris.input();  // Handle the user input
        if (tetris.isGameOver())
        {
            // 游戏结束时弹出提示框
            //  Show a message box with the final score
            int result = MessageBoxA(tetris.hwnd, ("你的得分是 " + to_string(tetris.score) + "\n选择\"是\"以重启游戏；\n或选择\"否\"以返回主菜单").c_str(), "游戏结束", MB_YESNO | MB_ICONINFORMATION);
            switch (result)
            {
            case IDYES: // User chose "Yes"
                // Restart the game
                startNewGame();
                break;
            case IDNO: // User chose "No"
                break;
            }
            break; // Check if the game is over
        }
        if (tetris.gameQuited)
        {
            break;
        }
    }
}

void continueGame()
{
    system("cls");
    tetris.loadGame();
    system("cls");
    srand(time(NULL));
    // 把分数那块涂黑，避免上一个板块和现有的重叠
    HBRUSH hBrush = CreateSolidBrush(RGB(12, 12, 12));
    SelectObject(tetris.hdc, hBrush);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(12, 12, 12));
    SelectObject(tetris.hdc, hPen);
    Rectangle(tetris.hdc, COLS * BLOCK_SIZE + 5, 0, COLS * BLOCK_SIZE + 100, 180);
    DeleteObject(hBrush);
    DeleteObject(hPen);
    // 要手动把drawBtnAgain等在程序运行中会被改变与初始值不同的非保存的成员变量再重置一遍，因为tetris不会重复初始化
    tetris.lastLoop = clock(); // Initialize the last loop time
    for (int i = 0; i < 6; i++)
    {
        tetris.lastClick[i] = clock();
    }
    tetris.lastdraw = clock();
    tetris.drawAgain = 1;
    tetris._drawAgain = 0;
    for (int i = 0; i < 6; i++)
    {
        tetris.drawBtnAgain[i] = 1;
    }
    tetris.gameQuited = false;

    // Get the handle of the console window
    tetris.hwnd = GetConsoleWindow();
    // Get the handle of the device context
    tetris.hdc = GetDC(tetris.hwnd);
    tetris.drawText(10, ROWS * BLOCK_SIZE + 10, "按键规则：\040\040ESC——菜单", RGB(12, 12, 12), RGB(255, 255, 255));
    tetris.drawText(10, ROWS * BLOCK_SIZE + 40, "A——左移\040\040D——右移\040\040S——下移", RGB(12, 12, 12), RGB(255, 255, 255));
    tetris.drawText(10, ROWS * BLOCK_SIZE + 70, "W——旋转\040\040Space——下落\040\040P——暂停", RGB(12, 12, 12), RGB(255, 255, 255));
    tetris.drawText(10, ROWS * BLOCK_SIZE + 100, "**如果出现画面错乱的问题，请点击R键恢复**", RGB(12, 12, 12), RGB(255, 255, 255));
    // Display a message to indicate the game is loaded successfully.
    tetris.drawText(COLS * BLOCK_SIZE + 15, 360, "Game Loaded!", RGB(255, 255, 255), RGB(12, 12, 12)); // 要先关联hdc
    tetris.lastdraw = clock();
    tetris.drawProm = 1;
    while (true)
    {
        tetris.render();
        tetris.update();
        tetris.input();
        if (tetris.isGameOver())
        {
            int result = MessageBoxA(tetris.hwnd, ("你的得分是 " + to_string(tetris.score) + "\n选择\"是\"以重启游戏；\n或选择\"否\"以返回主菜单").c_str(), "游戏结束", MB_YESNO | MB_ICONINFORMATION);
            switch (result)
            {
            case IDYES:
                startNewGame();
                break;
            case IDNO:
                break;
            }
            break;
        }
        if (tetris.gameQuited)
        {
            break;
        }
    }
}

void displayInfo()
{
    system("cls");
    cout << "—·—·—·—·—·—·—·—·—·—·—·—·—" << endl
         << "**开发者声明**" << endl
         << "-这个游戏是用C++语言编写的，使用Windows API\n来实现图形和声音效果。" << endl
         << "-这个游戏是一个个人项目，用于学习和练习编程\n技能，不用于商业目的。" << endl
         << "-这个游戏的源代码可以在\nhttps://github.com/jishux2/MyTetris上查看。" << endl
         << "-我要感谢Sydney，一个人工智能助手，它帮助我\n解决了一些编程问题和提出了一些建议。" << endl
         << "-我希望你玩这个游戏的时候能够享受和开心。" << endl
         << "—·—·—·—·—·—·—·—·—·—·—·—·—" << endl
         << endl;
    cout << "(a) 新游戏" << endl
         << "(b) 继续游戏" << endl
         << "(c) 开发者信息" << endl
         << "(d) 退出" << endl;
    cout << "请输入你的选择: ";
}

void displayMenu()
{
    // Clear the screen
    system("cls");
    // Display the title
    cout << "欢迎来到我的俄罗斯方块游戏!" << endl;
    cout << "------------------" << endl;
    // Display the options
    cout << "(a) 新游戏" << endl
         << "(b) 继续游戏" << endl
         << "(c) 开发者信息" << endl
         << "(d) 退出" << endl;
    // Display the prompt
    cout << "请输入你的选择: ";
}

// Define a function to handle the user input
bool handleInput()
{
    char choice;
    cin >> choice;
    cin.sync();
    // Switch on the user input
    switch (choice)
    {
    case 'a': // New Game
    case 'A':
        system("cls");
        startNewGame();
        return 0;
    case 'b': // Continue Game
    case 'B':
        continueGame();
        return 0;
    case 'c': // Developer Info
    case 'C':
        displayInfo();
        return handleInput();
    case 'd': // Exit
    case 'D':
        return 1;
    default: // Invalid input
        cout << "乱输入达咩达" << endl
             << endl
             << "这次请好好输入：";
        handleInput();
        break;
    }
    return 0;
}

int main()
{
    // 设置控制台标题
    SetConsoleTitle(TEXT("俄罗斯方块-by jishxu2"));
    // 定义窗口区域结构体
    SMALL_RECT rect = {0, 0, 45, 36};
    // 把控制台窗口调整到rect指定的区域
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);
    while (cin.good())
    {
        // Display the main menu
        displayMenu();
        // Handle the user input
        if (handleInput())
            break;
    }
    system("cls");
    cout << "下次再见了，朋友" << endl;
    cin.clear();
    cin.sync();
    cin.get();
    return 0;
}
