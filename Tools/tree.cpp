#include "tree.h"
#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <winsock2.h>
#include <fstream>

using namespace std;

void treeGrower(int clientSocket) {
    string serverInput = "5";
    FILE* treeFile = fopen("../assets/tree.txt", "a+");
    send(clientSocket, serverInput.c_str(), serverInput.length(), 0);
    char buffer[1024] = {0};
    while (true) {
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
            if (bytesReceived <= 0) {
                break; 
            }
        buffer[bytesReceived] = '\0';
        if (strcmp(buffer, "EOF") == 0) {
            break;
            }
        std::cout << buffer;
        fprintf(treeFile, "%s", buffer);
                        
    }
    fclose(treeFile);
}

