#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include "fazl.h"

int main() {
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if(WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cout << "Error #C1: winsock didn't loaded" << std::endl;
        exit(1);
    }

    SOCKADDR_IN server_addr;
    int sizeOf_sAddr = sizeof(server_addr);
    InetPton(AF_INET, _T("127.0.0.1"), &server_addr.sin_addr.s_addr);
    server_addr.sin_port = htons(1111);
    server_addr.sin_family = AF_INET;

    SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
    if(connect(Connection, (SOCKADDR*)&server_addr, sizeOf_sAddr) != 0) {
        std::cout << "Error #C2: connection failed" << std::endl;
        std::cout << WSAGetLastError() << std::endl;
        exit(2);
    }
    std::cout << "Connected to server" << std::endl;

    char cmd[CMD_LEN];
    long lmsg;
    char msg[256];
    strcpy(cmd, "allip");
    send(Connection, cmd, sizeof(cmd), NULL);
    recv(Connection, (char*)&lmsg, sizeof(lmsg), NULL);

    for(long i = lmsg; i > 0; i--) {
        recv(Connection, msg, sizeof(msg), NULL);
        std::cout << msg << std::endl;
    }

    return 0;
}
