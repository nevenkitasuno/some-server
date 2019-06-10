#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <list>
#include <string>
#include <thread>

#pragma warning(disable: 4996)

struct client{
    ULONG ip_address;
    std::string ip_address_str;
    USHORT port;
};

std::list<client> clients;

void clientHandler(const SOCKET& clientConnection, const SOCKADDR_IN& clientAddr) {
    int msg_size;
    int resp_size;
    std::string response;
    while(true) {
        recv(clientConnection, (char*)&msg_size, sizeof(msg_size), NULL);
        char *msg = new char[msg_size + 1];
        msg[msg_size] = '\0';
        recv(clientConnection, msg, msg_size, NULL);
        response = "";

        if (msg == "give_all_ips") {
            for(client c : clients) {
                if(c.ip_address == clientAddr.sin_addr.s_addr && c.port == clientAddr.sin_port) {
                    continue;
                }
                response += c.ip_address_str + "\n";
            }
            resp_size = response.size();
            send(clientConnection, (char*)&resp_size, sizeof(resp_size), NULL);
            send(clientConnection, response.c_str(), resp_size, NULL);
        }
        delete[] msg;
    }
}

int main() {
    WSAData wsaData;
    WORD DDLVersion = MAKEWORD(2, 1);
    if(WSAStartup(DDLVersion, &wsaData) != 0) {
        std::cout << "Error #S1: winsock didn't loaded" << std::endl;
        exit(1);
    }

    SOCKADDR_IN listened_addr;
    int sizeOf_lAddr = sizeof(listened_addr);
    listened_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    listened_addr.sin_port = htons(1111);
    listened_addr.sin_family = AF_INET;

    SOCKADDR_IN client_addr;
    int sizeOf_cAddr = sizeof(client_addr);

    SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);

    bind(sListen, (SOCKADDR*)&listened_addr, sizeof(listened_addr));
    listen(sListen, SOMAXCONN);

    //toDel
    for(int i = 0; i < 9; i++) {
        clients.push_back({NULL, "127.0.0." + std::to_string(i), htons(1111)});
    }

    SOCKET newConnection;
    char client_ip_cstr[INET_ADDRSTRLEN];
    while(true) {
        newConnection = accept(sListen, (SOCKADDR*)&listened_addr, &sizeOf_lAddr);

        if (newConnection == 0) {
            std::cout << "Error #S2: connection failed" << std::endl;
        } else {
            //clients.push_back(newConnection);
            getpeername(newConnection, (SOCKADDR*)&client_addr, &sizeOf_cAddr);
            inet_ntop(AF_INET, &(client_addr), client_ip_cstr, INET_ADDRSTRLEN);
            client connected_client = {client_addr.sin_addr.s_addr, client_ip_cstr, client_addr.sin_port};
            clients.push_back(connected_client);
            std::cout << "Client connected" << std::endl;

            std::thread thr(clientHandler, newConnection, client_addr);
        }
    }

    return 0;
}
