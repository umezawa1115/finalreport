#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>

// 定数の定義
const int width = 40;
const int height = 20;
const char playerChar = '@';
const char enemyChar = 'E';
const char itemChar = 'T';
const char emptyChar = ' ';
const char wallChar = '#';
const int playerSpeed = 1;
const int enemyCount = 10;

class Player {
public:
    int x, y;
    Player(int startX, int startY) : x(startX), y(startY) {}

    void moveLeft(const std::vector<std::vector<char>>& maze) {
        if (x > 0 && maze[y][x - 1] != wallChar) x -= playerSpeed;
    }
    void moveRight(const std::vector<std::vector<char>>& maze) {
        if (x < width - 1 && maze[y][x + 1] != wallChar) x += playerSpeed;
    }
    void moveUp(const std::vector<std::vector<char>>& maze) {
        if (y > 0 && maze[y - 1][x] != wallChar) y -= playerSpeed;
    }
    void moveDown(const std::vector<std::vector<char>>& maze) {
        if (y < height - 1 && maze[y + 1][x] != wallChar) y += playerSpeed;
    }
};

class Enemy {
public:
    int x, y;
    Enemy(int startX, int startY) : x(startX), y(startY) {}

    void moveRandom(const std::vector<std::vector<char>>& maze) {
        int dir = rand() % 4;
        switch (dir) {
        case 0: if (x > 0 && maze[y][x - 1] != wallChar) x--; break;
        case 1: if (x < width - 1 && maze[y][x + 1] != wallChar) x++; break;
        case 2: if (y > 0 && maze[y - 1][x] != wallChar) y--; break;
        case 3: if (y < height - 1 && maze[y + 1][x] != wallChar) y++; break;
        }
    }
};

class Game {
private:
    Player player;
    std::vector<Enemy> enemies;
    int itemX, itemY;
    bool gameOver;
    HANDLE hConsole;
    std::vector<std::vector<char>> maze; // 迷路のデータ

    void setup() {
        enemies.clear();
        gameOver = false;
        srand(static_cast<unsigned>(time(0)));
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        // プレイヤーの初期位置
        player = Player(1, 1);

        // アイテムの位置
        itemX = width - 2;
        itemY = height - 2;

        // 迷路の生成
        maze = std::vector<std::vector<char>>(height, std::vector<char>(width, emptyChar));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    maze[y][x] = wallChar; // 外壁
                }
            }
        }

        // 内壁のランダム配置
        for (int i = 0; i < width * height / 8; ++i) { // 内壁の数を調整
            int wx = rand() % (width - 2) + 1;
            int wy = rand() % (height - 2) + 1;
            if (maze[wy][wx] == emptyChar && !(wx == 1 && wy == 1) && !(wx == itemX && wy == itemY)) {
                maze[wy][wx] = wallChar;
            }
        }

        // 敵の配置
        for (int i = 0; i < enemyCount; ++i) {
            int ex, ey;
            do {
                ex = rand() % (width - 2) + 1;
                ey = rand() % (height - 2) + 1;
            } while (maze[ey][ex] == wallChar || (ex == player.x && ey == player.y) || (ex == itemX && ey == itemY));
            enemies.emplace_back(ex, ey);
        }
    }

    void draw() {
        COORD coord = { 0, 0 };
        SetConsoleCursorPosition(hConsole, coord);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (x == player.x && y == player.y) {
                    std::cout << playerChar;
                }
                else if (x == itemX && y == itemY) {
                    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    std::cout << itemChar;
                    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                }
                else if (maze[y][x] == wallChar) {
                    std::cout << wallChar;
                }
                else {
                    bool printed = false;
                    for (const auto& enemy : enemies) {
                        if (x == enemy.x && y == enemy.y) {
                            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                            std::cout << enemyChar;
                            SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
                            printed = true;
                            break;
                        }
                    }
                    if (!printed) {
                        std::cout << emptyChar;
                    }
                }
            }
            std::cout << "\n";
        }
    }

    void input() {
        if (_kbhit()) {
            switch (_getch()) {
            case 'a':
                player.moveLeft(maze);
                break;
            case 'd':
                player.moveRight(maze);
                break;
            case 'w':
                player.moveUp(maze);
                break;
            case 's':
                player.moveDown(maze);
                break;
            case 'q':
                gameOver = true;
                break;
            }
        }
    }

    void update() {
        for (auto& enemy : enemies) {
            enemy.moveRandom(maze);
            if (player.x == enemy.x && player.y == enemy.y) {
                gameOver = true;
                std::cout << "Game Over! You were caught by an enemy!\n";
            }
        }
        if (player.x == itemX && player.y == itemY) {
            gameOver = true;
            std::cout << "Congratulations! You collected the item!\n";
        }
    }

    void run() {
        setup();
        while (!gameOver) {
            draw();
            input();
            update();
            Sleep(200); // 調整してゲームのスピードを制御
        }
    }

public:
    Game() : player(0, 0), gameOver(false) {}

    void start() {
        run();
    }
};

int main() {
    Game game;
    game.start();
    return 0;
}