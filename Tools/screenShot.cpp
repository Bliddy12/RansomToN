#include "screenShot.h"
#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <winsock2.h>
#include <fstream>

using namespace std;


void screenShotThreader(int clientSocket, atomic<bool>& running) {
    while (running) {
        string serverInput = "3"; // Command for screenshots
        send(clientSocket, serverInput.c_str(), serverInput.length(), 0);
        screenShot(clientSocket);
        Sleep(1000); // 1-second delay
    }
}

void screenShot(int clientSocket) {

    char sizeBuffer[sizeof(size_t)];
    recv(clientSocket, sizeBuffer, sizeof(size_t), 0);
    size_t fileSize = *reinterpret_cast<size_t*>(sizeBuffer);

    // Open output file
    ofstream outFile("../assets/received_image.bmp", ios::binary);
    if (!outFile.is_open()) {
        cerr << "Could not open output file" << endl;
    }

    // Receive file data
    char buffer[1024];
    size_t received = 0;
    while (received < fileSize) {
        size_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            outFile.write(buffer, bytesRead);
            received += bytesRead;
        } else {
            break;
        }
    }
    outFile.close();
}