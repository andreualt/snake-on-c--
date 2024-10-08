#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <fstream>
#include <string>
#include <thread>
#include "server.h"

using namespace std;

class SnakeGame {
public:
    SnakeGame() {
        gameOver = false;
        paused = false;
        dir = STOP;
        score = 0;
        nTail = 0;
        LoadHighScore();
        Setup();
    }

    void Start() {
        StartMenu();
        Run();
    }

private:
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
        x = width / 2;
        y = height / 2;
        fruitX = rand() % width;
        fruitY = rand() % height;
        bonusX = -1; // No hay bonificación inicial
        bonusY = -1;
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
                    cout << "O"; // Cabeza de la serpiente
                else if (i == fruitY && j == fruitX)
                    cout << "F"; // Fruta
                else if (i == bonusY && j == bonusX)
                    cout << "B"; // Fruta bonus
                else {
                    bool print = false;
                    for (int k = 0; k < nTail; k++) {
                        if (tailX[k] == j && tailY[k] == i) {
                            cout << "o"; // Cola de la serpiente
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
            case 27: // Tecla ESC
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

            // Manejo de envoltura de pantalla
            if (x >= width) x = 0; else if (x < 0) x = width - 1;
            if (y >= height) y = 0; else if (y < 0) y = height - 1;

            // Verificar colisión con la cola
            for (int i = 0; i < nTail; i++) {
                if (tailX[i] == x && tailY[i] == y) {
                    gameOver = true;
                }
            }

            // Verificar recolección de fruta
            if (x == fruitX && y == fruitY) {
                score += 10;
                fruitX = rand() % width;
                fruitY = rand() % height;
                nTail++;
                // Generar fruta bonus ocasionalmente
                if (rand() % 5 == 0) {
                    bonusX = rand() % width;
                    bonusY = rand() % height;
                }
                else {
                    bonusX = -1; // Sin fruta bonus
                }
            }

            // Verificar recolección de fruta bonus
            if (x == bonusX && y == bonusY) {
                score += 20; // Puntos extra
                bonusX = -1; // Reiniciar fruta bonus
                bonusY = -1;
            }
        }
    }

    void StartMenu() {
        system("cls");
        cout << "========================\n";
        cout << "      SNAKE GAME\n";
        cout << "========================\n\n";
        cout << "Controles:\n";
        cout << "W - Arriba\n";
        cout << "S - Abajo\n";
        cout << "A - Izquierda\n";
        cout << "D - Derecha\n";
        cout << "P - Pausar\n";
        cout << "ESC - Salir\n\n";
        cout << "Presiona cualquier tecla para comenzar...";
        _getch();
    }

    void PauseMenu() {
        while (paused) {
            system("cls");
            cout << "\n\n*** PAUSADO ***\n";
            cout << "Presiona 'P' para continuar.\n";
            cout << "Presiona 'ESC' para salir.\n";
            Input();
            Sleep(100); // Mejora la fluidez de la pausa
        }
    }

    void Run() {
        while (!gameOver) {
            Draw();
            Input();
            Logic();
            Sleep(10); // Ajustar la velocidad del juego
            if (paused) {
                PauseMenu();
            }
        }

        SaveHighScore();
        system("cls");
        cout << "\n\n¡GAME OVER!\n";
        cout << "Puntuación final: " << score << "\n";
    }

    // Método para conexión online (por implementar)
    void OnlineMode() {
        // Aquí iría la implementación de conexión online
    }
};

int main() {
    // Inicializar Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Error al iniciar Winsock. Saliendo..." << endl;
        return 1;
    }

    SnakeGame game;
    game.Start();

    // Limpiar Winsock
    WSACleanup();
    return 0;
}
