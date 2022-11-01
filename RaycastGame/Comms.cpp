#define MAKEWORD(a,b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#include "Comms.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <errno.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

std::string format(std::string s1, std::string s2)
{
    size_t const wSize = s1.size() + 1;
    char* const w = new char[wSize];
    std::copy(s1.begin(), s1.end(), w);
    sprintf_s(w, wSize, s2.c_str());
    std::string result = w;
    delete[] w;
    return result;
}

// ------------------- WSA -------------------
WSA::WSA()
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        perror(format("Couldn't start WSA. Error Code: %d", std::to_string((long double)WSAGetLastError())).c_str());
    }
}

WSAData* WSA::GetWSAData()
{
    return &wsaData;
}

// ------------------- Server -------------------
Server::Server(u_short port)
{
    this->listening = false;
    this->port = port;
    WSA::GetInstance();

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror(format("Could not create socket: %d", std::to_string(WSAGetLastError())).c_str());
        exit(1);
    }
    
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
}

// Should be used in thread
int Server::AcceptClient()
{
    server.sin_port = htons(this->port);

    if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        perror(std::string("Couldn't bind to port ").append(std::to_string(port)).append(std::to_string((long double)WSAGetLastError())).c_str());
    }
    else 
    {
        puts("Binded to port");
    }
    this->listening = true;
    listen(serverSocket, 2); // blocking call

    SOCKADDR_IN sockInfo = { 0 };
    int addrSize = sizeof(sockInfo);
    SOCKET clientSocket(accept(serverSocket, (struct sockaddr*) & sockInfo, &addrSize));
    if (clientSocket == INVALID_SOCKET)
    {
        perror(format("Accept failed with error code: %d", std::to_string((long double)WSAGetLastError())).c_str());
    }
    else
    {
        getpeername(clientSocket, (struct sockaddr*)&sockInfo, &addrSize);
        SOCKADDR_IN* clientSockInfo = new SOCKADDR_IN(sockInfo);
//        char ipbuf[15];
 //       const char* ip = inet_ntop(AF_INET, clientSockInfo, ipbuf, 15);
        printf("Connection recieved");
        
        int readlen = 50;
        char* readbuf = (char*) calloc(readlen, sizeof(char));
        recv(clientSocket, readbuf, 50, 0);
        std::cout << "Recieved '" << readbuf << "'" << std::endl;

        closesocket(clientSocket);
        delete clientSockInfo;
//        delete ip[];
    }
    
    return 0;
}

bool Server::IsListening()
{
    return this->listening;
}

Server::~Server()
{
}

// ------------------- Client -------------------

Client::Client()
{
}

int Client::Connect(std::string ip, u_short port)
{
    SOCKET s;
    if ((s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    int ret = inet_pton(AF_INET, ip.c_str(), (void*)&server.sin_addr.s_addr);
    if (ret < 0) {
        std::cout << "Bad return code " << ret << std::endl;
        return -1;
    }

    if (connect(s, (struct sockaddr*) &server, sizeof(server)) < 0) {
        printf("Connect failed\n");
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
        return -2;
    }
    else 
    {
        std::cout << "Sent: " << message << std::endl;
    }

    return 0;
}


