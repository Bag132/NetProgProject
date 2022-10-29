#define MAKEWORD(a,b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#include "Comms.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <errno.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// ------------------- Server -------------------
Server::Server()
{
    port = DEFAULT_PORT;
    if (wsa != nullptr) {
        wsa = new WSAData();

        if (WSAStartup(MAKEWORD(2, 2), wsa) != 0)
        {
            perror(format("Couldn't start WSA. Error Code: %d", std::to_string((long double)WSAGetLastError())).c_str());
            exit(1);
        }
    }

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror(format("Could not create socket: %d", std::to_string(WSAGetLastError())).c_str());
        exit(1);
    }
    
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
}

// Should be used in thread
int Server::acceptClient(u_short port)
{
    server.sin_port = htons(port);
    this->port = port;

    if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        perror(std::string("Couldn't bind to port ").append(std::to_string(port)).append(std::to_string((long double)WSAGetLastError())).c_str());
    }
    else 
    {
        puts("Binded to port");
    }

    listen(serverSocket, 2); // blocking call

    SOCKADDR_IN sockInfo = { 0 };
    int addrSize = sizeof(sockInfo);
    SOCKET* clientSocket = new SOCKET(accept(serverSocket, (struct sockaddr*) & sockInfo, &addrSize));
    if (*clientSocket == INVALID_SOCKET)
    {
        perror(format("Accept failed with error code: %d", std::to_string((long double)WSAGetLastError())).c_str());
    }
    else
    {
        getpeername(*clientSocket, (struct sockaddr*)&sockInfo, &addrSize);
        SOCKADDR_IN* clientSockInfo = new SOCKADDR_IN(sockInfo);
        char ipbuf[15];
        const char* ip = inet_ntop(AF_INET, clientSockInfo, ipbuf, 15);
        printf("Connection recieved from %s\n", ip);
        delete clientSockInfo;
        delete ip;
    }
    delete clientSocket;
    return 0;
}

int Server::acceptClient()
{
    return acceptClient(DEFAULT_PORT);
}

Server::~Server()
{
    if (wsa != nullptr)
    {
        delete wsa;
    }
}

// ------------------- Client -------------------

Client::Client()
{
 
    if (wsa != nullptr) {
        wsa = new WSAData();

        if (WSAStartup(MAKEWORD(2, 2), wsa) != 0)
        {
            perror(format("Couldn't start WSA. Error Code: %d", std::to_string((long double)WSAGetLastError())).c_str());
            exit(1);
        }
    }
}

int Client::Connect(std::string ip, u_short port)
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_pton(ip.c_str());

    if (connect(s, (struct sockaddr*) &server, sizeof(server)) < 0) {
        return -2;
    }
    
    this->serverSocket = s;
    return 0;
}

int Client::Send(std::string message)
{
    if (serverSocket == 0)
    {
        return -1;
    }

    if (send(serverSocket, message.c_str(), message.size(), 0) == INVALID_SOCKET) {
        return 1;
    }

    return 0;
}


