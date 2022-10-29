#pragma once

#include <WinSock2.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <thread>

#define DEFAULT_PORT 777

WSAData* wsa = nullptr;

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

class Server
{
public:
	Server();
	~Server();


	int acceptClient();
	int acceptClient(u_short port);
	
private:
	u_short port;
	SOCKET serverSocket;
	struct sockaddr_in server;

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

