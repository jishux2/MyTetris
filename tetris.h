#ifndef TETRIS_H // 如果没有定义TETRIS_H这个宏
#define TETRIS_H // 就定义它，这样可以避免重复包含这个头文件

#include <ctime>
#include <windows.h>

// Define the Tetris class
class Tetris
{
private:
    HWND hwnd;            // The handle of the window
    HDC hdc;              // The handle of the device context
    PAINTSTRUCT ps;       // The paint structure
    clock_t lastLoop;     // The time of the last loop
    clock_t lastClick[6]; // The time of the last click of each button
    clock_t lastDraw;     // 上一次在屏幕写入提示信息的时间(用于ESC在屏幕上输出信息后的延迟恢复)
    int score;            // The score of the game
    int level;            // The level of the game
    int speed;            // The speed of the game
    int lines;            // The number of lines cleared
    char button[6];       // 六种按键
    bool drawAgain;       // 判断是否要再画一次背景、堆积方块和数据
    bool _drawAgain;      // 判断是否要立马重画一次方块
    bool drawProm;        // 针对提示信息的绘画判断
    bool drawBtnAgain[6]; // 判断是否要重画按键。为了更流畅的视觉效果，让每个按键的变色各自独立
    bool isPaused;        // 判断游戏是否暂停
    bool gameQuited;      // 判断是否强行退出游戏
    bool gameOver;        // 判断游戏是否自然结束
    bool isFirstBlock;    // 检查是否是第一个方块

public:
    Tetris();                                                                            // The default constructor
    ~Tetris();                                                                           // The destructor
    void restore();                                                                      // The method to restore the game
    void init();                                                                         // The method to initialize the game
    void update();                                                                       // The method to update the game logic
    void render();                                                                       // The method to render the game graphics
    void input();                                                                        // The method to handle the user input
    void exit();                                                                         // The method to exit the game
    void createBlock();                                                                  // The method to create a new block
    void playSound(int type);                                                            // The method to play sound effects
    void drawText(int x, int y, const char *text, COLORREF bkColor, COLORREF textColor); // The method to draw text on the screen
    void drawButton(char button, COLORREF BKGCOLOR, COLORREF PENCOLOR);                  // 画出按键
    void saveGame();                                                                     // 保存游戏内容
    void loadGame();                                                                     // 加载游戏内容
    friend class Board;                                                                  // Declare Board as a friend class
    friend bool continueGame();                                                          // 声明继续游戏函数为友元函数
    friend void tetrisRun();                                                             // 声明运行游戏函数为友元函数
};

#endif
