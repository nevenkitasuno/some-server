#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <list>
#include <string>
#include <cstring>
#include <tchar.h>
#include <thread>
#include "fazl.h"

struct client{
    char client_ip_cstr[INET_ADDRSTRLEN];
};

std::list<client> clients;

void clientHandler(const SOCKET& clientConnection, const SOCKADDR_IN& clientAddr) {
    char cmd[CMD_LEN];
    long lmsg;
    char msg[256];
    while (true) {
        recv(clientConnection, cmd, sizeof(cmd), NULL);
        if(strcmp (cmd, "allip") == 0) {
            lmsg = clients.size();
            send(clientConnection, (char*)&lmsg, sizeof(lmsg), NULL);

            for(std::list<client>::iterator iter = clients.begin(); iter != clients.end(); ++iter) {
                strcpy(msg, iter->client_ip_cstr);
                send(clientConnection, msg, sizeof(msg), NULL);
            }
        }
    }
}

int main() {
    WSAData wsaData;
    WORD DLLVersion = MAKEWORD(2, 1);
    if(WSAStartup(DLLVersion, &wsaData) != 0) {
        std::cout << "Error #S1: winsock didn't loaded" << std::endl;
        exit(1);
    }

    SOCKADDR_IN listened_addr;
    int sizeOf_lAddr = sizeof(listened_addr);
    InetPton(AF_INET, _T("127.0.0.1"), &listened_addr.sin_addr.s_addr);
    listened_addr.sin_port = htons(1111);
    listened_addr.sin_family = AF_INET;

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);

    bind(sListen, (SOCKADDR*)&listened_addr, sizeOf_lAddr);
    listen(sListen, SOMAXCONN);

    SOCKET newConnection;

    SOCKADDR_IN client_addr;
    int sizeOf_cAddr = sizeof(client_addr);

    char client_ip_cstr[INET_ADDRSTRLEN];

    while(true) {
        newConnection = accept(sListen, (SOCKADDR*)&client_addr, &sizeOf_cAddr);

        if (newConnection == 0) {
            std::cout << "Error #S2: connection failed" << std::endl;
        } else {
            InetNtop(AF_INET, &client_addr.sin_addr.s_addr, client_ip_cstr, INET_ADDRSTRLEN);
            client connected_client;
            strcpy(connected_client.client_ip_cstr, client_ip_cstr);
            clients.push_back(connected_client);
            std::cout << "Client connected" << std::endl;
            std::thread thr(clientHandler, newConnection, client_addr);
            thr.detach();
        }
    }

    return 0;
}
