#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

// ==================== 全局变量声明 ====================
int hits = 0;
int correct_hits = 0;    // 击键次数统计（仅英文模式）
int correct = 0;         // 正确次数统计
int paused = 0;          // 暂停状态标志
int exitFlag = 0;        // 退出游戏标志
int difficulty;          // 难度等级（1-3）
int currentMode;         // 当前模式（1-中文，2-英文）
char currentChar[32];    // 当前目标字符（汉字或单词）
char userInput[32] = ""; // 用户输入

void displayStatus(); // 确保在调用 displayStatus 之前已声明（在handle函数中调用了status函数）

// ==================== 汉字和短语库 ====================
typedef struct
{
    char hanzi[16];  // 目标汉字或短语
    char pinyin[16]; // 对应拼音（不带声调）
} ChineseEntry;

ChineseEntry currentEntry;

ChineseEntry chineseLib[] = {
    {"你好", "nihao"},
    {"中国", "zhongguo"},
    {"天气", "tianqi"},
    {"学习", "xuexi"},
    {"电脑", "diannao"},
    {"程序", "chengxu"},
    {"人工智能", "rengongzhineng"},
    {"开发", "kaifa"},
    {"打字", "dazi"},
    {"键盘", "jianpan"},
    {"输入", "shuru"},
    {"速度", "sudu"},
    {"游戏", "youxi"},
    {"挑战", "tiaozhan"},
    {"时间", "shijian"},
};

// ==================== 英文模式单词库 ====================
char *englishLib[] = {
    "Hello", "World", "Typing", "Speed", "Challenge", "AI",
    "Programming", "Computer", "Keyboard", "Development",
    "Function", "Algorithm", "Python", "Debug", "Execute"};

void menu()
{
    system("cls");
    printf("\n\n");
    printf("                           ████████   ██    ██   ██████    ██████    ███    ██     ██████        \n");
    printf("                              ██       ██  ██    ██   ██     ██      ██ █   ██    ██        \n");
    printf("                              ██         ██      ██████      ██      ██  █  ██   ██    ████    \n");
    printf("                              ██         ██      ██          ██      ██   █ ██    ██     ██  \n");
    printf("                              ██         ██      ██        ██████    ██    ███     ██████         \n\n\n\n\n\n\n\n");
    printf("                                          按下 ENTER 键开始你的打字挑战之旅！\n");
    printf("                                      ---------------------------------------------\n");
    printf("                                      | 操作说明：                               |\n");
    printf("                                      | 空格键 - 暂停/继续                       |\n");
    printf("                                      | ESC键  - 退出游戏                        |\n");
    printf("                                      | 回车键 - 提交当前输入                    |\n");
    printf("                                      ---------------------------------------------\n");

    // 等待回车键
    while (1)
    {
        if (_kbhit())
        {
            int key = _getch();
            if (key == 13)
            { // 回车键
                break;
            }
            if (key == 27)
            {
                exitFlag = 1;
                break;
            }
        }
        Sleep(100);
    }
}

// ==================== 控制台光标隐藏 ====================
void hideCursor()
{
    CONSOLE_CURSOR_INFO cursor = {1, 0};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor);
}

//==================== 选择难度和模式 ====================
int selectDifficulty() // 定义一个返回整数类型的函数“ selectDifficulty”，用于选择游戏难度
{
    system("cls"); // 清除控制台屏幕
    printf("请选择难度：\n");
    printf("1-蜗牛模式（10秒/字符）\n");
    printf("2-正常模式（5秒/字符）\n");
    printf("3-闪电模式（3秒/字符）\n");

    while (1) // 进入一个无限循环，直到用户输入有效的选择。
    {
        if (_kbhit()) // 检查是否有键盘输入
        {
            int choice = _getch() - '0'; // 获取用户按下的键，并将其转换为对应的数字
            if (choice >= 1 && choice <= 3)
                return choice;
            else
            {
                printf("前面的区域，以后再来探索吧！\n");
                continue;
            }
        }
        Sleep(100); // 暂停程序执行 100 毫秒，以减少 CPU 使用率
    }
}

int selectMode() // 定义一个返回整数类型的函数 “selectMode”，用于选择游戏模式
{
    system("cls");
    printf("请选择模式：\n");
    printf("1-中文模式（拼音输入）\n");
    printf("2-英文模式（完整单词）\n");

    while (1)
    {
        if (_kbhit())
        {
            int choice = _getch() - '0';
            if (choice == 1 || choice == 2)
                return choice;
        }
        Sleep(100);
    }
}
// ==================== 生成随机目标字符 ====================
void generateCharacter()
{
    if (currentMode == 1) // 生成中文字符
    {
        int index = rand() % (sizeof(chineseLib) / sizeof(ChineseEntry));
        // rand() 函数生成一个随机数，sizeof(chineseLib) 返回 chineseLib 数组的总字节数，sizeof(ChineseEntry) 返回单个 ChineseEntry 结构体的字节数

        currentEntry = chineseLib[index]; // 保存完整中文条目
        strcpy(currentChar, currentEntry.hanzi);
    }
    else
    {
        int index = rand() % (sizeof(englishLib) / sizeof(char *));
        strcpy(currentChar, englishLib[index]);
        // 将 currentEntry 中的汉字复制到 currentChar 字符串中
    }
    memset(userInput, 0, sizeof(userInput));
    // 将 userInput 数组的所有字节设置为0，清空用户输入，以确保没有残留的旧数据
}

// ==================== 获取键盘输入 ====================
int getKeyInput()
{
    if (_kbhit())
    {
        int ch = _getch();
        return ch;
    }
    return -1;
}

// ==================== 检查输入 ====================
void checkInput()
{

    if (currentMode == 1)
    {
        // 直接使用保存的拼音进行比对
        if (strcmp(userInput, currentEntry.pinyin) == 0)
        {
            correct++;
        }
    }
    else
    {
        if (strcmp(userInput, currentChar) == 0)
        {
            correct++;
        }
    }
}

// ==================== 处理输入（非阻塞式） ====================
int handleTypingInput()
{
    int key = getKeyInput();
    if (key == -1)
        return 0;

    // 全局控制按键
    if (key == 27)
    { // ESC 退出
        exitFlag = 1;
        return 1;
    }
    if (key == 32)
    { // 空格键切换暂停
        paused = !paused;
        return 0;
    }

    // 暂停时忽略其他输入
    if (paused)
        return 0;

    // 正常输入处理
    if (key == 13)
    {
        // 回车提交
        checkInput();
        return 1;
    }
    else if (key == 8)
    { // 退格键
        int len = strlen(userInput);
        if (len > 0)
        {
            userInput[len - 1] = '\0';
        }
    }
    else // 统计总击键数和正确击键数
        if (key >= 32 && key <= 126)
        { // 可打印字符
            int len = strlen(userInput);
            if (len < sizeof(userInput) - 1)
            {
                userInput[len] = key;
                userInput[len + 1] = '\0';

                // 获取当前模式对应的目标字符串
                const char *target = (currentMode == 1) ? currentEntry.pinyin : currentChar;

                hits++; // 总击键数
                if (len < strlen(target) && tolower(key) == target[len])
                {
                    correct_hits++;
                    // 正确击键数
                }
            }
        }

    displayStatus();
    return 0;
}

void generateDisplayString(char *display, const char *input, const char *target)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        if (i < strlen(input) && i < strlen(target))
        {
            // 中英文通用比对逻辑
            display[i] = (input[i] == target[i]) ? input[i] : '_';
        }
        else if (i < strlen(input))
        {
            // 输入长度超过目标时显示下划线
            display[i] = '_';
        }
        else
        {
            break;
        }
    }
    display[i] = '\0';
}

// ==================== 显示状态 ====================
void displayStatus()
{

    COORD pos = {0, 1};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); // 设置控制台光标定位到第0列第1行（控制台坐标系）

    // 创建格式化输入缓冲区
    char displayInput[32] = {0};
    // 根据当前模式选择目标内容（中文模式显示拼音，英文模式显示单词）
    const char *target = (currentMode == 1) ? currentEntry.pinyin : currentChar;

    // 生成带格式的输入显示字符串（包含颜色标记）
    generateDisplayString(displayInput, userInput, target); // 生成带格式的输入显示字符串（包含颜色标记）

    printf("当前目标：\033[33m%-16s\033[0m\n", currentChar);
    printf("输入内容：\033[32m%-32s\033[0m\n", displayInput);
    printf("模式：%s\t难度：%s\t状态：%s\n",
           currentMode == 1 ? "中文拼音输入" : "英文单词输入",
           difficulty == 1 ? "蜗牛" : difficulty == 2 ? "正常"
                                                      : "闪电",
           paused ? "\033[31m已暂停\033[0m" : "\033[32m进行中\033[0m"); // 状态显示，ANSI转义序列，红色表示暂停，绿色表示进行

    printf("正确拼音/单词：%d", correct);
}
void end()
{
    system("cls");
    // 游戏结束标题艺术字
    printf("\033[36m╔════════════════════════════════════════════╗\n");
    printf("║   ██████  ██    ██ ███████ ██   ██  █████  ║\n");
    printf("║  ██    ██ ██    ██ ██      ██   ██ ██   ██ ║\n");
    printf("║  ██    ██ ██    ██ █████   ███████ ███████ ║\n");
    printf("║  ██    ██  ██  ██  ██      ██   ██ ██   ██ ║\n");
    printf("║   ██████    ████   ███████ ██   ██ ██   ██ ║\n");
    printf("╚════════════════════════════════════════════╝\033[0m\n\n");

    // ==================== 核心统计计算 ====================
    float accuracy = (hits > 0) ? ((float)correct_hits / hits * 100) : 0;

    // ==================== 详细统计面板 ====================
    printf("\033[33m├─────────────────── 数据统计 ───────────────────┤\033[0m\n");
    printf("│%-25s: \033[35m%8d\033[0m │\n", "总击键次数", hits);
    printf("│%-25s: \033[32m%8d\033[0m │\n", "正确击键次数", correct_hits);
    printf("│%-25s: \033[32m%8d\033[0m │\n", 
           currentMode == 1 ? "正确拼音数" : "正确单词数", correct);
    printf("│%-25s: \033[36m%7.1f%%\033[0m  │\n", "击键准确率", accuracy);

    // ==================== 动态评价系统 ====================
    printf("\033[33m├─────────────────── 能力评估 ───────────────────┤\033[0m\n");
    if (accuracy >= 95.0) {
        printf("│\033[5;32m ★★★★ 神速之触！精准度堪比机械键盘！\033[0m │\n");
    } else if (accuracy >= 85.0) {
        printf("│\033[32m ★★★☆ 职业水平！轻松驾驭各种输入场景！\033[0m │\n");
    } else if (accuracy >= 70.0) {
        printf("│\033[33m ★★☆☆ 熟练工！继续练习突破瓶颈！\033[0m      │\n");
    } else if (accuracy >= 50.0) {
        printf("│\033[36m ★☆☆☆ 入门阶段！每天进步一点点！\033[0m      │\n");
    } else {
        printf("│\033[31m ☆☆☆☆ 注意基础！建议进行专项训练！\033[0m    │\n");
    }

    printf("\033[33m└────────────────────────────────────────────────┘\033[0m\n");
}
// ==================== 主函数 ====================
int main()
{
    hideCursor();
    menu();
    if (!exitFlag)
    {
        // 选择模式和难度
        difficulty = selectDifficulty();
        currentMode = selectMode();
        system("cls");
    }
    else
    {

        printf("退出游戏");
        return 0;
    }

    // 速度配置（单位：毫秒）
    const int speedConfig[] = {10000, 5000, 3000};

    // 游戏主循环
    while (!exitFlag)
    {
        // 从中文/英文词库随机获取当前目标字符/拼音
        generateCharacter(); // 生成挑战字符
        displayStatus();     // 显示当前得分、正确率等状态信息

        // 使用Windows API中定义的无符号的 32 位整数处理相关时间
        DWORD start = GetTickCount();                  // 记录本轮开始时间（毫秒）
        DWORD timeLimit = speedConfig[difficulty - 1]; // 根据难度获取时间限制(毫秒）
        DWORD inputCompleted = 0;                      // 完成标志
        DWORD pausedTime = 0;                          // 累计暂停时间（毫秒）

        COORD progressPos = {0, 7}; // 控制台进度条显示坐标（列0，行7）

        // 单轮挑战循环
        while (!exitFlag)
        {
            if (paused) // 暂停状态处理
            {
                DWORD pauseStart = GetTickCount(); // 记录暂停开始时间
                displayStatus();                   // 刷新显示状态（显示暂停提示）

                // 暂停状态循环
                while (paused && !exitFlag)
                {
                    // 持续检测输入（当收到恢复指令时返回true）
                    if (handleTypingInput())
                        break;
                }

                pausedTime += GetTickCount() - pauseStart; // 累计暂停时间
                displayStatus();                           // 退出暂停时强制刷新界面
                continue;                                  // 跳过本轮剩余逻辑
            }

            // 正常游戏逻辑
            DWORD elapsed = GetTickCount() - start - pausedTime; // 计算有效经过时间（总时间-暂停时间）

            // 处理用户输入
            if (handleTypingInput())
            {
                inputCompleted = 1;
            }

            // 输入完成或超时则结束本轮
            if (inputCompleted || elapsed >= timeLimit)
                break;

            // 界面更新逻辑
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), progressPos); // 定位到进度条位置
            printf("剩余时间：[");
            float progress = (float)elapsed / timeLimit; // 计算进度比例

            // 绘制进度条（20个字符宽度）
            for (int i = 0; i < 20; i++)
                printf(i < progress * 20 ? "█" : " ");

            printf("]");

            Sleep(10);
        }
    }

    // 结束界面
    system("cls");
    end();
    return 0;
}
