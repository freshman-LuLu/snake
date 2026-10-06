#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>

#define WIDTH 40
#define HEIGHT 20

int snakeX[100], snakeY[100];
int len = 3;
int foodX, foodY;
int dir; // 方向 1上 2右 3下 4左
int gameOver = 0;


// 设置光标位置
void Gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// 隐藏光标
void HideCursor()
{
    CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void InitGame()
{
    gameOver = 0;
    dir = 2; // 默认向右
    snakeX[0] = WIDTH / 2;
    snakeY[0] = HEIGHT / 2;
    snakeX[1] = WIDTH / 2 - 1;
    snakeY[1] = HEIGHT / 2;
    snakeX[2] = WIDTH / 2 - 2;
    snakeY[2] = HEIGHT / 2;

    // 随机食物
    foodX = rand() % (WIDTH - 4) + 3;
    foodY = rand() % (HEIGHT - 4) + 2;
}

void Draw()
{
    Gotoxy(0, 0);
    // 上边框
    for (int i = 0; i < WIDTH + 2; i++)
        printf("#");
    printf("\n");

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH + 2; x++)
        {
            if (x == 0 || x == WIDTH + 1)  // 左右边框
                printf("#");
            else if (x == snakeX[0] && y == snakeY[0])
                printf("O"); //蛇头
            else if (x == foodX && y == foodY)
                printf("F"); //食物
            else
            {
                int isBody = 0;
                for (int i = 1; i < len; i++)
                {
                    if (snakeX[i] == x && snakeY[i] == y)
                    {
                        printf("o"); //蛇身体
                        isBody = 1;
                        break;
                    }
                }
                if (!isBody)
                    printf(" ");
            }
        }
        printf("\n");
    }

    //下边框
    for (int i = 0; i < WIDTH + 2; i++)
        printf("#");
    printf("\n");
    printf("  长度：%d\n", len);
    printf("  本游戏只可以通过英文输入状态下小写的wsad控制，X退出游戏\n");
}

void Input()
{
    if (_kbhit()) //检测按键
    {
        switch (_getch())
        {
        case 'w': if (dir != 3) dir = 1; break;
        case 'd': if (dir != 4) dir = 2; break;
        case 's': if (dir != 1) dir = 3; break;
        case 'a': if (dir != 2) dir = 4; break;
        case 'x': gameOver = 1; break;
        }
    }
}

void Logic()
{
    //身体跟随移动，从尾巴往前覆盖
    for (int i = len - 1; i > 0; i--)
    {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    //蛇头移动
    switch (dir)
    {
    case 1: snakeY[0]--; break;
    case 2: snakeX[0]++; break;
    case 3: snakeY[0]++; break;
    case 4: snakeX[0]--; break;
    }

    //撞墙
    if (snakeX[0] <= 0 || snakeX[0] >= WIDTH + 1 || snakeY[0] <= 0 || snakeY[0] >= HEIGHT)
        gameOver = 1;

    //撞到自己
    for (int i = 1; i < len; i++)
    {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i])
            gameOver = 1;
    }

    //吃到食物
    if (snakeX[0] == foodX && snakeY[0] == foodY)
    {
        len++;
        foodX = rand() % (WIDTH - 4) + 3;  // 随机生成新的食物位置 
        foodY = rand() % (HEIGHT - 4) + 2;
    }
}

int main()
{
    srand((unsigned int)time(NULL));
    printf("===========================================================\n");
    printf("                 欢迎来到贪吃蛇小游戏\n");
    printf("===========================================================\n");
    printf("  本游戏只可以通过英文输入状态下小写的wsad控制，X退出游戏\n");
    printf ("请仔细阅读游戏规则，按下Y键以开始游戏，按下其他键退出游戏...\n");
    
    while (1)
    {
        int que = _getch();
        if (que=='y'||que=='Y')
            {
            system("cls");
            break;
        }
        else
        {
            return 0;
        }

    }
    while (1)
    {
        HideCursor();
        InitGame();
        while (!gameOver)
        {
            Draw();
            Input();
            Logic();
            Sleep(100); //控制速度
        }
        Gotoxy(0, HEIGHT + 3);
        printf("游戏结束！最终长度：%d\n", len);
        printf ("按R重新开始，按其他键退出游戏...\n");

        int an=_getch();
        if (an == 'r' || an == 'R')
        {
            system("cls");
        }
        else
        {
            break;
        }
   
    }
    return 0;

}
