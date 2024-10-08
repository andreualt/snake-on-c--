#include <iostream>
#include <thread>
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int PORT = 54000;
const char* SERVER_IP = "127.0.0.1"; // Localhost

void listenForUpdates(SOCKET serverSocket) {
    char buffer[1024];
    while (true) {
        int bytesReceived = recv(serverSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;

        // Process incoming data (e.g., print player positions)
        cout << buffer;
    }
}

int online() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    thread listener(listenForUpdates, serverSocket);

    while (true) {
        char input;
        if (_kbhit()) {
            input = _getch();
            send(serverSocket, &input, sizeof(input), 0);
        }
    }

    listener.join();
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
