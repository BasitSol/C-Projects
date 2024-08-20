#include<iostream>
#include<cstdlib>  // For rand() and srand()
#include<ctime>    // For time()
#include<windows.h> // For Windows-specific functions
#include<conio.h>  // For _getch(), _kbhit()

using namespace std;

#define MAX_LENGTH 1000

// Directions
const char DIR_UP = 'U';
const char DIR_DOWN = 'D';
const char DIR_LEFT = 'L';
const char DIR_RIGHT = 'R';

const char WALL = '#';  // Character representing the wall

int consoleWidth, consoleHeight;

void initScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

struct Point {
    int xCoord;
    int yCoord;
    Point() {}
    Point(int x, int y) : xCoord(x), yCoord(y) {}
};

class Snake {
    int length;
    char direction;

public:
    Point Body[MAX_LENGTH];
    Snake(int x, int y) {
        length = 1;
        Body[0] = Point(x, y);
        direction = DIR_RIGHT;
    }

    int getLength() {
        return length;
    }

    void changeDirection(char newDirection) {
        if (newDirection == DIR_UP && direction != DIR_DOWN)
            direction = newDirection;
        else if (newDirection == DIR_DOWN && direction != DIR_UP)
            direction = newDirection;
        else if (newDirection == DIR_LEFT && direction != DIR_RIGHT)
            direction = newDirection;
        else if (newDirection == DIR_RIGHT && direction != DIR_LEFT)
            direction = newDirection;
    }

    bool move(Point food) {
        for (int i = length - 1; i > 0; i--)
            Body[i] = Body[i - 1];

        switch (direction) {
        case DIR_UP:
            Body[0].yCoord--;
            break;
        case DIR_DOWN:
            Body[0].yCoord++;
            break;
        case DIR_RIGHT:
            Body[0].xCoord++;
            break;
        case DIR_LEFT:
            Body[0].xCoord--;
            break;
        }

        // Snake bites itself
        for (int i = 1; i < length; i++) {
            if (Body[0].xCoord == Body[i].xCoord && Body[0].yCoord == Body[i].yCoord) {
                return false;
            }
        }

        // Snake eats food
        if (food.xCoord == Body[0].xCoord && food.yCoord == Body[0].yCoord) {
            Body[length] = Point(Body[length - 1].xCoord, Body[length - 1].yCoord);
            length++;
        }

        // Check if snake hits the wall
        if (Body[0].xCoord <= 0 || Body[0].xCoord >= consoleWidth - 1 ||
            Body[0].yCoord <= 0 || Body[0].yCoord >= consoleHeight - 1) {
            return false;
        }

        return true;
    }
};

class Board {
    Snake* snake;
    const char SNAKE_BODY = 'O';
    Point food;
    const char FOOD = 'o';
    int score;

public:
    Board() {
        spawnFood();
        snake = new Snake(consoleWidth / 2, consoleHeight / 2);
        score = 0;
    }

    ~Board() {
        delete snake;
    }

    int getScore() {
        return score;
    }

    void spawnFood() {
        int x, y;
        do {
            x = rand() % (consoleWidth - 2) + 1;
            y = rand() % (consoleHeight - 2) + 1;
        } while (x == 0 || y == 0 || x == consoleWidth - 1 || y == consoleHeight - 1);
        food = Point(x, y);
    }

    void displayCurrentScore() {
        gotoxy(consoleWidth / 2, 0);
        cout << "CURRENT SCORE IS :  " << score << endl;
    }

    void gotoxy(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    void drawWalls() {
        for (int i = 0; i < consoleWidth; i++) {
            gotoxy(i, 0);
            cout << WALL;
            gotoxy(i, consoleHeight - 1);
            cout << WALL;
        }
        for (int i = 0; i < consoleHeight; i++) {
            gotoxy(0, i);
            cout << WALL;
            gotoxy(consoleWidth - 1, i);
            cout << WALL;
        }
    }

    void draw() {
        system("cls");
        drawWalls();

        for (int i = 0; i < snake->getLength(); i++) {
            gotoxy(snake->Body[i].xCoord, snake->Body[i].yCoord);
            cout << SNAKE_BODY;
        }

        gotoxy(food.xCoord, food.yCoord);
        cout << FOOD;

        displayCurrentScore();
    }

    bool update() {
        bool isAlive = snake->move(food);
        if (!isAlive)
            return false;

        if (food.xCoord == snake->Body[0].xCoord && food.yCoord == snake->Body[0].yCoord) {
            score++;
            spawnFood();
        }

        return true;
    }

    void getInput() {
        if (_kbhit()) {
            int key = _getch();
            if (key == 'w' || key == 'W')
                snake->changeDirection(DIR_UP);
            else if (key == 'a' || key == 'A')
                snake->changeDirection(DIR_LEFT);
            else if (key == 's' || key == 'S')
                snake->changeDirection(DIR_DOWN);
            else if (key == 'd' || key == 'D')
                snake->changeDirection(DIR_RIGHT);
        }
    }
};

int main() {
    srand(time(0));
    initScreen();
    Board* board = new Board();
    while (board->update()) {
        board->getInput();
        board->draw();
        Sleep(20);
    }
    cout << "GAME OVER!!!" << endl;
    cout << "FINAL SCORE IS: " << board->getScore();
    delete board; 
    return 0;
}

