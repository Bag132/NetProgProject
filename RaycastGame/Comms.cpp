#define MAKEWORD(a,b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#include "Comms.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <errno.h>

Server::Server()
{
	wsa = WSAData();

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        perror(Server::format("Couldn't start WSA. Error Code: %d", std::to_string((long double)WSAGetLastError())).c_str());
        exit(1);
    }

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror(format("Could not create socket: %d", std::to_string(WSAGetLastError())).c_str());
        exit(1);
    }
    
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_family = AF_INET;
}


int Server::acceptClient(u_short port)
{
    server.sin_port = htons(port);
    this->port = port;

    SOCKADDR_IN clientSockInfo = { 0 };
    int addrSize = sizeof(clientSockInfo);
    SOCKET* clientSocket = new SOCKET(accept(serverSocket, (struct sockaddr*) & clientSockInfo, &addrSize));
    if (*clientSocket == INVALID_SOCKET)
    {
        perror(format("Accept failed with error code: %d", std::to_string((long double)WSAGetLastError())).c_str());
    }
    else
    {
        
    }
}

int Server::acceptClient()
{
    acceptClient(DEFAULT_PORT);
}
