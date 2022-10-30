#pragma once

#include <WinSock2.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>

#define DEFAULT_PORT 777

class WSA
{
public:
	WSA(const WSA&) = delete;
	void operator=(const WSA&) = delete;

	static WSA& GetInstance()
	{
		static WSA instance;
		return instance;
	}
	WSAData* GetWSAData();
	
private:
	WSA();
	WSAData wsaData;
	
};

class Server
{
public:
	Server(u_short port);
	~Server();

	int AcceptClient();
	bool IsListening();
	
private:
	u_short port;
	SOCKET serverSocket;
	struct sockaddr_in server;
	bool listening;

};


class Client
{
public:
	static Client& getInstance()
	{
		static Client instance;
		return instance;
	}

	int Connect(std::string ipv4, u_short port);
	int Send(std::string message);

	void testComms()
	{
	}

private:
	Client();
	SOCKET serverSocket = 0;
};

