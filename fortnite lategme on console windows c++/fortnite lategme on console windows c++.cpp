#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <fstream>
#include <string>
#include <thread>
#include "server.h"
using namespace std;

bool gameOver;
bool paused;
const int width = 50;
const int height = 25;
int x, y, fruitX, fruitY, bonusX, bonusY, score, highScore;
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
    bonusX = -1; // Indica que no hay bonificación inicial
    bonusY = -1;
    score = 0;
    nTail = 0;
}

void LoadHighScore() {
    ifstream infile("highscore.txt");
    if (infile.is_open()) {
        infile >> highScore;
        infile.close();
    }
    else {
        highScore = 0;
    }
}

void SaveHighScore() {
    if (score > highScore) {
        highScore = score;
        ofstream outfile("highscore.txt");
        outfile << highScore;
        outfile.close();
    }
}

void Draw() {
    system("cls");
    cout << "Score: " << score << " | High Score: " << highScore << endl;
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#";
            if (i == y && j == x)
                cout << "O"; // Snake head
            else if (i == fruitY && j == fruitX)
                cout << "F"; // Fruit
            else if (i == bonusY && j == bonusX)
                cout << "B"; // Bonus fruit
            else {
                bool print = false;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        cout << "o"; // Snake tail
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
        case 27: // ESC key
            gameOver = true;
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
        case LEFT:  x--; break;
        case RIGHT: x++; break;
        case UP:    y--; break;
        case DOWN:  y++; break;
        }

        // Handle screen wrapping
        if (x >= width) x = 0; else if (x < 0) x = width - 1;
        if (y >= height) y = 0; else if (y < 0) y = height - 1;

        // Check for collision with tail
        for (int i = 0; i < nTail; i++) {
            if (tailX[i] == x && tailY[i] == y) {
                gameOver = true;
            }
        }

        // Check for fruit collection
        if (x == fruitX && y == fruitY) {
            score += 10;
            fruitX = rand() % width;
            fruitY = rand() % height;
            nTail++;
            // Spawn bonus fruit occasionally
            if (rand() % 5 == 0) {
                bonusX = rand() % width;
                bonusY = rand() % height;
            }
            else {
                bonusX = -1; // No bonus fruit
            }
        }

        // Check for bonus fruit collection
        if (x == bonusX && y == bonusY) {
            score += 20; // Bonus points
            bonusX = -1; // Reset bonus fruit
            bonusY = -1;
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
    while (paused) {
        system("cls");
        cout << "\n\n*** PAUSED ***\n";
        cout << "Press 'P' to continue.\n";
        cout << "Press 'ESC' to exit.\n";
        Input();
        Sleep(10);
    }
}

void OnlineMode(SOCKET serverSocket) {
    // Send player position to server
    char position[50];
    while (!gameOver) {
        snprintf(position, sizeof(position), "%d,%d,%d,%d", x, y, score, nTail);
        send(serverSocket, position, sizeof(position), 0);
        Sleep(100); // Send position every 100ms
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));
    LoadHighScore();
    StartMenu();
    Setup();

    // Initialize server connection
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Cambiar a la dirección del servidor
    serverAddr.sin_port = htons(54000); // Cambiar al puerto correspondiente

    connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Start the online mode in a separate thread
    thread onlineThread(OnlineMode, serverSocket);

    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(10);
        if (paused) {
            PauseMenu();
        }
    }

    SaveHighScore();
    system("cls");
    cout << "\n\nGAME OVER!\n";
    cout << "Final Score: " << score << "\n";

    // Clean up
    closesocket(serverSocket);
    WSACleanup();
    onlineThread.join();
    return 0;
}
