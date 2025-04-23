# What Is RansomToN 

RansomToN is a userland-style rootkit simulator, built in C++ with a command-line interface. It mimics various malware functions for red teaming, cybersecurity research, and malware analysis training.

# Features

- Message Sender – Simulate ransom notes or popups

- Keylogger – Logs keystrokes (demo-only)

- Screen Recorder – Capture user activity (Windows API)

- Remote Shutdown – Trigger remote shutdown on target (local network)

- Tree Command Utility – Custom implementation of the tree command

- Userland Design – Operates at the user level without kernel-mode drivers

- Windows Defender and most AVs currently do not flag this as malicious (subject to change).
  
- This project does not contain real ransomware encryption, destructive payloads, or persistence mechanisms. It is intended for learning and educational labs only.

# Installation

For server:
```
git clone https://github.com/Bliddy12/RansomToN.git
cd RansomToN
cd server
g++ server.cpp sendM.cpp ../Tools/keylogger.cpp ../Tools/tree.cpp initSock.cpp -o server -lgdi32 -lws2_32 ; ./server
```
For client:
```
git clone https://github.com/Bliddy12/RansomToN.git
cd RansomToN
```
Make sure to change the IP on client.cpp line 14 to the server IP
```
const char *SERVER_IP = "<IP>";
```
Then
```
g++ client.cpp -o client -lws2_32 -lgdi32 -mwindows -static ; ./client
```

# Looks

![image](https://github.com/user-attachments/assets/52132f8f-a6eb-41d6-9a91-5b56f4941108)

After connection:
![image](https://github.com/user-attachments/assets/f61a396e-f7ea-478c-903a-acd9235d1f19)

