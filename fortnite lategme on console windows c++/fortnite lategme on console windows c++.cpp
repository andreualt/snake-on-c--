#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>

using namespace std;

bool gameOver;
bool paused;
const int width = 50;
const int height = 25;
int x, y, fruitX, fruitY, score;
int tailX[200], tailY[200];
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

void Setup() {
    gameOver = false;
    paused = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
}

void Draw() {
    system("cls");
    cout << "Score: " << score << endl;
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#";
            if (i == y && j == x)
                cout << "O";
            else if (i == fruitY && j == fruitX)
                cout << "F";
            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o";
                        print = true;
                    }
                }
                if (!print) cout << " ";
            }
            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'a':
                if (dir != RIGHT) dir = LEFT;
                break;
            case 'd':
                if (dir != LEFT) dir = RIGHT;
                break;
            case 'w':
                if (dir != DOWN) dir = UP;
                break;
            case 's':
                if (dir != UP) dir = DOWN;
                break;
            case 'p':
                paused = !paused;
                break;
            case 27:
                if (!paused) {
                    gameOver = true;
                }
                break;
        }
    }
}

void Logic() {
    if (!paused) {
        int prevX = tailX[0];
        int prevY = tailY[0];
        int prev2X, prev2Y;
        tailX[0] = x;
        tailY[0] = y;

        for (int i = 1; i < nTail; i++) {
            prev2X = tailX[i];
            prev2Y = tailY[i];
            tailX[i] = prevX;
            tailY[i] = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }

        switch (dir) {
            case LEFT:
                x--;
                break;
            case RIGHT:
                x++;
                break;
            case UP:
                y--;
                break;
            case DOWN:
                y++;
                break;
            default:
                break;
        }

        if (x >= width) x = 0; else if (x < 0) x = width - 1;
        if (y >= height) y = 0; else if (y < 0) y = height - 1;

        for (int i = 0; i < nTail; i++) {
            if (tailX[i] == x && tailY[i] == y) {
                gameOver = true;
            }
        }

        if (x == fruitX && y == fruitY) {
            score += 10;
            fruitX = rand() % width;
            fruitY = rand() % height;
            nTail++;
        }
    }
}

void StartMenu() {
    system("cls");
    cout << "========================\n";
    cout << "      SNAKE GAME\n";
    cout << "========================\n\n";
    cout << "Controls:\n";
    cout << "W - Up\n";
    cout << "S - Down\n";
    cout << "A - Left\n";
    cout << "D - Right\n";
    cout << "P - Pause\n";
    cout << "ESC - Exit\n\n";
    cout << "Press any key to start...";
    _getch();
}

void PauseMenu() {
    system("cls");
    cout << "\n\n*** PAUSED ***\n";
    cout << "Press 'P' to continue.\n";
    cout << "Press 'ESC' to exit.\n";
    while (paused) {
        Input();
        Sleep(10);
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    StartMenu();
    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(10);
        if (paused) {
            PauseMenu();
        }
    }

    system("cls");
    cout << "\n\nGAME OVER!\n";
    cout << "Final Score: " << score << "\n";
    return 0;
}
