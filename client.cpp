#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>
#include <iostream>

#pragma warning(disable: 4996)

int zapros(SOCKET);

int main(int argc, char* argv[]) {
    //WSAStartup
    WSAData wsaData;
    WORD DDLVersion = MAKEWORD(2, 1);
    if(WSAStartup(DDLVersion, &wsaData) != 0) {
        std::cout << "Error #C1: winsock didn't loaded" << std::endl;
        exit(1);
    } //Подгружаем WinSock

    //Информация об адресе сервера
    SOCKADDR_IN addr;
    int sizeOfAddr = sizeof(addr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(1111);
    addr.sin_family = AF_INET;

    SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL); //Сокет для соединения с сервером
    if(connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) { //Попытка соединения. Параметры как у слушающего accept'а
        std::cout << "Error #C2: connection failed" << std::endl;
        std::cout << WSAGetLastError() << std::endl;
        exit(1);
    }
    std::cout << "Connected to server" << std::endl;

    //char msg[256];
    //recv(Connection, msg, sizeof(msg), NULL); //Принимаем сообщение
    //std::cout << msg << std::endl;
    char *msg = "give_all_ips";
    int msg_size = sizeof(msg);
    send(Connection, (char*)msg_size, sizeof(msg_size), NULL);
    send(Connection, msg, msg_size, NULL);
    delete[] msg;

    recv(Connection, (char*)&msg_size, sizeof(msg_size), NULL);
    msg = new char[msg_size + 1];
    msg[msg_size] = '\0';
    recv(Connection, msg, msg_size, NULL);

    std::cout << msg;
    delete[] msg;

    system("pause");
    return 0;
}
