#include <iostream>
#include <thread>
#include <vector>
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const int PORT = 54000;
const char* SERVER_IP = "127.0.0.1"; // hhttps://127.0.0.1/54000 THINK SO

class Server {
public:
    Server() {
        
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
        serverAddr.sin_port = htons(PORT);
        bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
        listen(serverSocket, SOMAXCONN);
        cout << "Servidor iniciado en " << SERVER_IP << ":" << PORT << endl;
    }

    void Start() {
        while (true) {
            
            SOCKET clientSocket = accept(serverSocket, NULL, NULL);
            cout << "Nuevo cliente conectado." << endl;
            clients.push_back(clientSocket);
            
            thread listener(&Server::listenForUpdates, this, clientSocket);
            listener.detach();
        }
    }

    ~Server() {
        for (SOCKET client : clients) {
            closesocket(client);
        }
        closesocket(serverSocket);
        WSACleanup();
    }

private:
    WSADATA wsaData;
    SOCKET serverSocket;
    sockaddr_in serverAddr;
    vector<SOCKET> clients;

    void listenForUpdates(SOCKET clientSocket) {
        char buffer[1024];
        while (true) {
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) {
                cout << "Cliente desconectado." << endl;
                break;
            }

            buffer[bytesReceived] = '\0'; 
            cout << "Recibido: " << buffer << endl;
            
            broadcast(buffer, bytesReceived);
        }
        closesocket(clientSocket);
    }

    void broadcast(const char* message, int length) {
        for (SOCKET client : clients) {
            send(client, message, length, 0);
        }
    }
};


int online() {
    Server server;
    server.Start();
    return 0;
}
