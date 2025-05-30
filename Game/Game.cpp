#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <limits>
#include <string> // Добавлено для to_string

// Для платформозависимых функций
#ifdef _WIN32
#include <conio.h> // Для _kbhit() и _getch()
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace std;

class Game2048 {
private:
    vector<vector<int>> board;
    int size;
    int score;
    bool moved;

    void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void addRandomTile() {
        vector<pair<int, int>> emptyCells;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (board[i][j] == 0) {
                    emptyCells.emplace_back(i, j);
                }
            }
        }

        if (!emptyCells.empty()) {
            int index = rand() % emptyCells.size();
            int value = (rand() % 10 == 0) ? 4 : 2;
            board[emptyCells[index].first][emptyCells[index].second] = value;
        }
    }

    void moveTiles(vector<int>& line) {
        vector<int> nonZero;
        for (int num : line) {
            if (num != 0) {
                nonZero.push_back(num);
            }
        }

        for (int i = 0; i < (int)nonZero.size() - 1; ++i) {
            if (nonZero[i] == nonZero[i + 1]) {
                nonZero[i] *= 2;
                score += nonZero[i];
                nonZero.erase(nonZero.begin() + i + 1);
                moved = true;
            }
        }

        while (nonZero.size() < size) {
            nonZero.push_back(0);
        }

        if (line != nonZero) {
            moved = true;
            line = nonZero;
        }
    }

    void rotateBoard() {
        vector<vector<int>> temp(size, vector<int>(size));
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                temp[j][size - 1 - i] = board[i][j];
            }
        }
        board = temp;
    }

    bool canMove() {
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (board[i][j] == 0) {
                    return true;
                }
            }
        }

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size - 1; ++j) {
                if (board[i][j] == board[i][j + 1]) {
                    return true;
                }
            }
        }

        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < size - 1; ++i) {
                if (board[i][j] == board[i + 1][j]) {
                    return true;
                }
            }
        }

        return false;
    }

public:
    Game2048(int boardSize) : size(boardSize), score(0), moved(false) {
        board.resize(size, vector<int>(size, 0));
        srand(static_cast<unsigned int>(time(nullptr)));
        addRandomTile();
        addRandomTile();
    }

    void printBoard() {
        clearScreen();
        cout << "Score: " << score << "\n\n";

        int maxTile = getMaxTile();
        int cellWidth = to_string(maxTile).length() + 2;

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (board[i][j] == 0) {
                    cout << string(cellWidth, '.') << "\t";
                }
                else {
                    cout << board[i][j];
                    int spaces = cellWidth - to_string(board[i][j]).length();
                    cout << string(spaces, ' ') << "\t";
                }
            }
            cout << "\n\n";
        }
    }

    void move(int direction) {
        moved = false;

        for (int i = 0; i < direction; ++i) {
            rotateBoard();
        }

        for (int i = 0; i < size; ++i) {
            moveTiles(board[i]);
        }

        for (int i = 0; i < (4 - direction) % 4; ++i) {
            rotateBoard();
        }

        if (moved) {
            addRandomTile();
        }
    }

    bool isGameOver() {
        return !canMove();
    }

    int getScore() const {
        return score;
    }

    int getMaxTile() {
        int maxTile = 0;
        for (const auto& row : board) {
            for (int num : row) {
                maxTile = max(maxTile, num);
            }
        }
        return maxTile;
    }
};

// Альтернативная реализация _getch() для Linux
#ifndef _WIN32
int _getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int _kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
#endif

int getBoardSize() {
    int size;
    while (true) {
        cout << "Выберите размер доски (3-9): ";
        cin >> size;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Ошибка ввода. Пожалуйста, введите число.\n";
            continue;
        }

        if (size >= 3 && size <= 9) {
            return size;
        }
        else {
            cout << "Некорректный размер. Пожалуйста, выберите от 3 до 9.\n";
        }
    }
}

int main() {
    setlocale(LC_ALL, "ru");
    int boardSize = getBoardSize();
    Game2048 game(boardSize);
    char input;

    cout << "Игра 2048 - Доска " << boardSize << "x" << boardSize << "\n";
    cout << "Используйте WASD или стрелки для движения\n";
    cout << "Нажмите любую клавишу для начала...";
    _getch();

    while (true) {
        game.printBoard();

        if (game.isGameOver()) {
            cout << "Игра окончена! Ваш счет: " << game.getScore() << "\n";
            cout << "Максимальная плитка: " << game.getMaxTile() << "\n";
            break;
        }

        if (_kbhit()) {
            input = _getch();

            switch (tolower(input)) {
            case 'w': game.move(3); break;
            case 'd': game.move(2); break;
            case 's': game.move(1); break;
            case 'a': game.move(0); break;
            case 'q': return 0;
                
            }
        }
    }

    cout << "Нажмите любую клавишу для выхода...";
    _getch();
    return 0;
}