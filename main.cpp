#include <iostream>

#include <ctime>
#include "board.h" //不可以再引入block.h了
#include "tetris.h"

extern Tetris tetris;
extern void startNewGame();

using namespace std;

void tetrisRun()
{ // 运行俄罗斯方块游戏的主要逻辑
    // 绘制按键规则，若放在init函数里会导致绘画失误，若单独写到continueGame函数里会增加冗余代码，最好的方法还是放在这里
    tetris.drawText(10, ROWS * BLOCK_SIZE + 10, "按键规则：\040\040ESC——菜单", RGB(12, 12, 12), RGB(255, 255, 255));
    tetris.drawText(10, ROWS * BLOCK_SIZE + 40, "A——左移\040\040D——右移\040\040S——下移", RGB(12, 12, 12), RGB(255, 255, 255));
    tetris.drawText(10, ROWS * BLOCK_SIZE + 70, "W——旋转\040\040Space——下落\040\040P——暂停", RGB(12, 12, 12), RGB(255, 255, 255));
    tetris.drawText(10, ROWS * BLOCK_SIZE + 100, "**如果出现画面错乱的问题，请点击R键恢复**", RGB(12, 12, 12), RGB(255, 255, 255));
    while (true)
    {                    // 用一个无限循环来实现游戏循环
        tetris.render(); // 调用Tetris类的render方法，绘制游戏画面
        tetris.update(); // 调用Tetris类的update方法，更新游戏状态
        tetris.input();  // 调用Tetris类的input方法，处理用户输入
        if (tetris.gameOver)
        {
            // 如果Tetris类的gameOver属性为真，说明游戏已经结束
            // 弹出一个消息框，显示最终得分，并询问用户是否重新开始游戏
            int result = MessageBoxA(tetris.hwnd, ("游戏已无法继续\n你的得分是 " + to_string(tetris.score) + "\n选择\"是\"以重启游戏；\n或选择\"否\"以返回主菜单").c_str(), "游戏结束", MB_YESNO | MB_ICONINFORMATION);
            switch (result)
            {
            case IDYES: // 如果用户选择了"是"
                // 调用startNewGame函数，重新开始游戏
                startNewGame();
                break;
            case IDNO: // 如果用户选择了"否"
                // 不做任何操作
                break;
            }
            break; // 跳出循环，结束函数
        }
        if (tetris.gameQuited)
        {
            // 如果Tetris类的gameQuited属性为真，说明用户已经退出游戏
            break; // 跳出循环，结束函数
        }
    }
}

void startNewGame()
{
    tetris.init();    // Initialize the game
    tetris.restore(); // Restore the game
    tetrisRun();
}

bool continueGame()
{
    tetris.init();
    system("cls");
    tetris.loadGame();
    if (!tetris.gameQuited)
    {
        system("cls");
        tetris.drawText(COLS * BLOCK_SIZE + 15, 360, "Game Loaded!", RGB(255, 255, 255), RGB(12, 12, 12));
        tetrisRun();
        return 1;
    }
    else
    {
        return 0;
    }
}

void displayInfo()
{
    system("cls");
    cout << "—·—·—·—·—·—·—·—·—·—·—·—·—" << endl
         << "**开发者声明**" << endl
         << "-这是一个由武汉大学的学生林俊宏开发的俄罗斯\n方块游戏。" << endl
         << "-学号：2022302111485" << endl
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
        if (continueGame())
        {
            return 0;
        }
        else
        {
            cout << endl
                 << "(a) 新游戏" << endl
                 << "(b) 继续游戏" << endl
                 << "(c) 开发者信息" << endl
                 << "(d) 退出" << endl;
            // Display the prompt
            cout << "请输入你的选择: ";
            return handleInput();
        }
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
    SetConsoleTitle(TEXT("俄罗斯方块-by 林俊宏"));
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
