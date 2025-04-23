#include <iostream>
#include <winsock2.h>

using namespace std;
int initSock(){

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cout << "WSAStartup failed with error: " << iResult << endl;
        return 1;
    }
    return 0;
}