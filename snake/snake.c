#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdlib.h>
#include<string.h>


#define WIDTH 40
#define HEIGHT 20
#define SCORE_FILE "snake_scores.txt"  //设置临时储存文件
#define MAX_RECORDS 10
#define WIN_LEN 20

int snakeX[100], snakeY[100];
int len = 3;
int foodX, foodY;
int dir; // 方向 1上 2右 3下 4左
int gameOver = 0;
int win = 0;

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

// 读取文件，打印历史成绩
int LoadScores()
{
    FILE* fp = fopen(SCORE_FILE, "r");
    if (fp == NULL)
    {
        printf("                    暂无历史成绩记录\n");
        return 0;
    }

    int slen, swin, count = 0;
    printf("-------------------历史成绩（最多%d条）--------------------\n", MAX_RECORDS);

    printf("%-10s%-10s%-10s\n", "序号", "长度", "结果");

    printf("------------------------------------------------------------\n");
    while (count < MAX_RECORDS && fscanf_s(fp, "%d %d", &slen, &swin) == 2)  //
    {
        printf("%-10d%-10d%-10s\n", count + 1, slen, swin ? "胜利" : "失败");
        count++;
        printf("------------------------------------------------------------\n");
    }


    printf("------------------------------------------------------------\n");
    fclose(fp);
    return count;
}

// 保存一条成绩，最多保留最新的 MAX_RECORDS 条
void SaveScore(int newLen, int newWin)
{
    int lens[MAX_RECORDS + 1];
    int wins[MAX_RECORDS + 1];
    int count = 0;

    // 读取已有记录
    FILE* fp = fopen(SCORE_FILE, "r");
    if (fp != NULL)
    {
        while (count < MAX_RECORDS && fscanf_s(fp, "%d %d", &lens[count], &wins[count]) == 2)
        {
            count++;
        }
        fclose(fp);
    }

    // 添加新记录
    lens[count] = newLen;
    wins[count] = newWin;
    count++;

    // 超过 MAX_RECORDS 条时，只保留最新的 MAX_RECORDS 条
    int start = 0;
    if (count > MAX_RECORDS)
    {
        start = count - MAX_RECORDS;
    }

    // 写回文件
    fp = fopen(SCORE_FILE, "w");  //ss
    if (fp == NULL)
    {
        printf("                无法保存成绩文件！\n");
        return;
    }
    for (int i = start; i < count; i++)
    {
        fprintf(fp, "%d %d\n", lens[i], wins[i]);
    }
    fclose(fp);
}

// 程序退出时删除成绩文件
void CleanupScoreFile()
{
    remove(SCORE_FILE);
}



void InitGame()
{
    int win = 0;
    int len = 3;
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
        if (len >= 15)
        {
            gameOver = 1;
            win = 1;
        }
        foodX = rand() % (WIDTH - 4) + 3;  // 随机生成新的食物位置 
        foodY = rand() % (HEIGHT - 4) + 2;
    }
}

int main()
{
    atexit(CleanupScoreFile); //注册退出函数，程序结束时删除成绩文件)
    remove(SCORE_FILE); //删除旧的成绩文件

    srand((unsigned int)time(NULL));
    printf("===========================================================\n");
    printf("                 欢迎来到贪吃蛇小游戏\n");
    printf("===========================================================\n");
    printf("  本游戏只可以通过英文输入状态下小写的wsad控制，X退出游戏\n");
    printf("                  贪吃蛇长度达到15获胜\n");
    printf("                贪吃蛇会随着获取食物而加速\n");
    printf("===========================================================\n");

    LoadScores();

    printf("===========================================================\n");
    printf("请仔细阅读游戏规则，按下Y键以开始游戏，按下其他键退出游戏...\n");

    while (1)
    {
        int que = _getch();
        if (que == 'y' || que == 'Y')
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
            Sleep(100 - len * 3); //控制速度
        }
        if (win == 0)
        {
            Gotoxy(0, HEIGHT + 3);
            printf("游戏结束！最终长度：%d\n", len);
            SaveScore(len, 0); //保存失败成绩
            printf("按R重新开始，按其他键退出游戏...\n");
        }

        else
        {
            Gotoxy(0, HEIGHT + 3);
            printf("恭喜你，你赢了！最终长度：%d\n", len);
            SaveScore(len, 1); // 保存胜利成绩
            printf("按R重新开始，按其他键退出游戏...\n");
        }


        int an = _getch();
        if (an == 'r' || an == 'R')
        {
            system("cls");
            LoadScores();              // 【修改】新增：重开前再显示一次历史成绩
            printf("按Y键开始新一局...\n");
            while (1)
            {
                int q = _getch();
                if (q == 'y' || q == 'Y')
                {
                    system("cls");
                    break;
                }
                else
                {
                    return 0;
                }
            }
        }
        else
        {
            break;
        }
    }

    return 0;

}
