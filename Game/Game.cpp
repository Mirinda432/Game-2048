#include <iostream> //Ввод-вывод
#include <vector> // Работа с дин.массивами
#include <cstdlib> // Функции rand() и srand()
#include <ctime> // Функция time()
#include <algorithm> //Алгоритмы
#include <limits> // Пределы типов
#include <string> // Работа со строками
#include <map> // Ассоциативные массивы
// ANSI коды для управления курсором и цветами
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
#define CLEAR_SCREEN "\033[2J"
#define MOVE_CURSOR_HOME "\033[H"
// ANSI цветовые коды
#define COLOR_RESET   "\033[0m"
#define COLOR_BLACK   "\033[30m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BRIGHT_BLACK   "\033[90m"
#define COLOR_BRIGHT_RED     "\033[91m"
#define COLOR_BRIGHT_GREEN   "\033[92m"
#define COLOR_BRIGHT_YELLOW  "\033[93m"
#define COLOR_BRIGHT_BLUE    "\033[94m"
#define COLOR_BRIGHT_MAGENTA "\033[95m"
#define COLOR_BRIGHT_CYAN    "\033[96m"
#define COLOR_BRIGHT_WHITE   "\033[97m"
#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"
#define BG_BRIGHT_BLACK   "\033[100m"
#define BG_BRIGHT_RED     "\033[101m"
#define BG_BRIGHT_GREEN   "\033[102m"
#define BG_BRIGHT_YELLOW  "\033[103m"
#define BG_BRIGHT_BLUE    "\033[104m"
#define BG_BRIGHT_MAGENTA "\033[105m"
#define BG_BRIGHT_CYAN    "\033[106m"
#define BG_BRIGHT_WHITE   "\033[107m"
//Платформозависимые заголовки
#ifdef _WIN32
#include <conio.h> // Для _kbnit() и _getch() в Windows
#include <windows.h>
#else
#include <termios.h> // Алтернативы для Линукс и МакОС
#include <unistd.h>
#include <fcntl.h>
#endif

using namespace std;
// Локализация(ДВуязычная поддержка)
map<string, map<string, string>> translations = {
    {"en", {
        {"choose_size", COLOR_CYAN "Choose board size (3-9): " COLOR_RESET},
        {"invalid_input", COLOR_RED "Invalid input. Please enter a number." COLOR_RESET},
        {"invalid_size", COLOR_RED "Invalid size. Please choose between 3 and 9." COLOR_RESET},
        {"title", COLOR_YELLOW "2048 Game - Board " COLOR_RESET},
        {"controls", COLOR_GREEN "Use " COLOR_BRIGHT_WHITE "WASD" COLOR_GREEN " to move\n" COLOR_RESET},
        {"start", COLOR_MAGENTA "Press any key to start..." COLOR_RESET},
        {"game_over", COLOR_RED "Game over! Your score: " COLOR_RESET},
        {"max_tile", COLOR_YELLOW "Maximum tile: " COLOR_RESET},
        {"exit", COLOR_MAGENTA "Press any key to exit..." COLOR_RESET},
        {"language", COLOR_CYAN "Choose language (1 - English, 2 - Русский): " COLOR_RESET},
        {"invalid_lang", COLOR_RED "Invalid choice, try again." COLOR_RESET}
    }},
    {"ru", {
        {"choose_size", COLOR_CYAN "Выберите размер доски (3-9): " COLOR_RESET},
        {"invalid_input", COLOR_RED "Ошибка ввода. Пожалуйста, введите число." COLOR_RESET},
        {"invalid_size", COLOR_RED "Некорректный размер. Пожалуйста, выберите от 3 до 9." COLOR_RESET},
        {"title", COLOR_YELLOW "Игра 2048 - Доска " COLOR_RESET},
        {"controls", COLOR_GREEN "Используйте " COLOR_BRIGHT_WHITE "WASD" COLOR_GREEN " для движения\n" COLOR_RESET},
        {"start", COLOR_MAGENTA "Нажмите любую клавишу для начала..." COLOR_RESET},
        {"game_over", COLOR_RED "Игра окончена! Ваш счет: " COLOR_RESET},
        {"max_tile", COLOR_YELLOW "Максимальная плитка: " COLOR_RESET},
        {"exit", COLOR_MAGENTA "Нажмите любую клавишу для выхода..." COLOR_RESET},
        {"language", COLOR_CYAN "Выберите язык (1 - English, 2 - Русский): " COLOR_RESET},
        {"invalid_lang", COLOR_RED "Некорректный выбор, попробуйте еще раз." COLOR_RESET}
    }}
};

string current_lang = "en"; //Текущий язык
//Функция получения перевода по коду
string t(string key) {
    return translations[current_lang][key];
}
//Класс игры 2048
class Game2048 {
private:
    vector<vector<int>> board; //Игровое поле (Двумерный вектор)
    int size; // Размер поля (N x N)
    int score; // Текущий счёт
    bool moved; // Флаг движения плиток
    //Очистка экрана (платформозависимая)
    void clearScreen() {
    #ifdef _WIN32
        // Для Windows используем системные вызовы для скрытия курсора
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hOut, &cursorInfo);
        cursorInfo.bVisible = false; // Скрываем курсор
        SetConsoleCursorInfo(hOut, &cursorInfo);
        system("cls");
    #else
        // Для Linux/macOS используем ANSI коды
        cout << HIDE_CURSOR << CLEAR_SCREEN << MOVE_CURSOR_HOME;
    #endif
    }
    //Добавление случайной плитки (2 или 4)
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
    // Движение плиток в строке/столбце
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
    //Поворот игрового поля
    void rotateBoard() {
        vector<vector<int>> temp(size, vector<int>(size));
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                temp[j][size - 1 - i] = board[i][j];
            }
        }
        board = temp;
    }
    //Проверка возможности хода
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

    void showCursor() {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hOut, &cursorInfo);
        cursorInfo.bVisible = true; // Показываем курсор
        SetConsoleCursorInfo(hOut, &cursorInfo);
#else
        cout << SHOW_CURSOR;
#endif
    }

public:
    
    //Конструктор (инициализация поля)
    Game2048(int boardSize) : size(boardSize), score(0), moved(false) {
        board.resize(size, vector<int>(size, 0));
        srand(static_cast<unsigned int>(time(nullptr)));
        addRandomTile();
        addRandomTile();
    }
    ~Game2048() {
        showCursor();
    }
    //Вывод игрового поля
    void printBoard() {
        clearScreen();
        #ifndef _WIN32
        cout << "\033[s"; // Сохраняем позицию курсора
        #endif
        // Цветной заголовок счета
        cout << COLOR_BRIGHT_BLUE "Score: " COLOR_YELLOW << score << COLOR_RESET "\n\n";

        int maxTile = getMaxTile();
        int cellWidth = to_string(maxTile).length() + 2;

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (board[i][j] == 0) {
                    cout << COLOR_BRIGHT_BLACK << string(cellWidth, '.') << COLOR_RESET "\t";
                }
                else {
                    // Разные цвета для разных значений плиток
                    string color;
                    switch (board[i][j]) {
                    case 2:    color = COLOR_BRIGHT_WHITE; break;
                    case 4:    color = COLOR_BRIGHT_CYAN; break;
                    case 8:    color = COLOR_BRIGHT_GREEN; break;
                    case 16:   color = COLOR_BRIGHT_YELLOW; break;
                    case 32:   color = COLOR_BRIGHT_MAGENTA; break;
                    case 64:   color = COLOR_BRIGHT_RED; break;
                    case 128:  color = COLOR_YELLOW BG_BLUE; break;
                    case 256:  color = COLOR_WHITE BG_MAGENTA; break;
                    case 512:  color = COLOR_WHITE BG_RED; break;
                    case 1024: color = COLOR_WHITE BG_GREEN; break;
                    case 2048: color = COLOR_BRIGHT_YELLOW BG_RED; break;
                    default:   color = COLOR_BRIGHT_WHITE BG_BLACK; break;
                    }
                    cout << color << board[i][j];
                    int spaces = cellWidth - to_string(board[i][j]).length();
                    cout << string(spaces, ' ') << COLOR_RESET "\t";
                }
            }
            cout << "\n\n";
        #ifndef _WIN32
            cout << "\033[u"; // Восстанавливаем позицию курсора
        #endif
        }
    }
    // Обработка движения (0-3 - направления)
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
    // Проверка окончания игры
    bool isGameOver() {
        return !canMove();
    }
    //Получение текущего счета
    int getScore() const {
        return score;
    }
    //Получение максимальной плитки
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
//Реализация _getch() для Linux/macOS
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
//Функция выбора размера игрового поля
int getBoardSize() {
    int size;
    //Цикл до получения корректного размера (3-9)
    while (true) {
        cout << t("choose_size");
        cin >> size;
        // Обработка ввода с проверкой ошибок
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000);
            cout << t("invalid_input") << endl;
            continue;
        }
        //Проверка допустимого диапазона
        if (size >= 3 && size <= 9) {
            return size;
        }
        //Сообщение об ошибке
        else {
            cout << t("invalid_size") << endl;
        }
    }
}
//Функция выбора языка интерфейса
bool selectLanguage() {
    bool language_selected = false;
    // Цветное меню выбора языка
    string prompt = string(COLOR_CYAN) +
        (current_lang == "ru" ?
            "Выберите язык:\n" :
            "Choose language:\n") +
        COLOR_GREEN "1" COLOR_RESET " - English\n" +
        COLOR_GREEN "2" COLOR_RESET " - Русский\n" +
        COLOR_CYAN "> " COLOR_RESET;

    while (!language_selected) {
        cout << prompt;
        //Чтение ввода целиком
        string input;
        getline(cin, input);
        //Проверка корректности ввода
        if (input.empty() || input.size() != 1 || !isdigit(input[0])) {
            cout << t("invalid_lang") << endl;
            continue;
        }

        int choice = input[0] - '0';
        //Обработка выбора
        switch (choice) {
        case 1: //Английский
            current_lang = "en";
            language_selected = true;
            break;
        case 2://Русский
            current_lang = "ru";
            language_selected = true;
            break;
        default:// Некорректный выбор
            cout << t("invalid_lang") << endl;
        }
    }
    return true;
}
//Главная функция
int main() {
    // Устанавливаем обработчик для корректного восстановления курсора
    #ifdef _WIN32
    SetConsoleCtrlHandler([](DWORD event) -> BOOL {
        if (event == CTRL_C_EVENT) {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_CURSOR_INFO cursorInfo;
            GetConsoleCursorInfo(hOut, &cursorInfo);
            cursorInfo.bVisible = true;
            SetConsoleCursorInfo(hOut, &cursorInfo);
        }
        return TRUE;
        }, TRUE);
    #endif

    try {
            // Для Windows нужно активировать поддержку ANSI кодов
            #ifdef _WIN32
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;
            GetConsoleMode(hOut, &dwMode);
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
            #endif

            setlocale(LC_ALL, "ru");

            // Цветной заголовок
            cout << COLOR_BRIGHT_MAGENTA
            << "==========================\n"
            << "||" COLOR_BRIGHT_YELLOW "       2048 GAME       " COLOR_BRIGHT_MAGENTA"||\n"
            << "==========================\n"
            << COLOR_RESET;

            if (!selectLanguage()) {
                return EXIT_FAILURE;
            }

            int boardSize = getBoardSize();
            Game2048 game(boardSize);
            char input;
            //Стартовые сообщения
            cout << t("title") << boardSize << "x" << boardSize << endl;
            cout << t("controls");
            cout << t("start");
            _getch();
            //Главный игровой цикл
            while (true) {
            game.printBoard();
            //Проверка окончания игры
            if (game.isGameOver()) {
                cout << t("game_over") << game.getScore() << endl;
                cout << t("max_tile") << game.getMaxTile() << endl;
                break;
            }
            //Обработка ввода
            if (_kbhit()) {
                input = _getch();
                //Обработка WASD
                switch (tolower(input)) {
                case 'w': game.move(3); break; //Вверх
                case 'd': game.move(2); break; //Вправо
                case 's': game.move(1); break; //Вниз
                case 'a': game.move(0); break; //Влево
                case 'q': return 0;   
                    }
                }
            }


            //Завершение игры
            cout << t("exit");
            _getch();
    }
    catch (...) {
        // В случае ошибки восстанавливаем курсор
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hOut, &cursorInfo);
        cursorInfo.bVisible = true;
        SetConsoleCursorInfo(hOut, &cursorInfo);
#else
        cout << SHOW_CURSOR;
#endif
        throw;
    }

    // Перед выходом восстанавливаем курсор
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = true;
    SetConsoleCursorInfo(hOut, &cursorInfo);
#else
    cout << SHOW_CURSOR;
#endif

    return 0;
}
