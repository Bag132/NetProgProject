#pragma once

#include <WinSock2.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#define DEFAULT_PORT 777

class Server
{
public:
	static Server& getInstance()
	{
		static Server instance;
		return instance;
	}

	int acceptClient();
	int acceptClient(u_short port);
	
private:
	Server();
	u_short port;
	WSAData wsa;
	SOCKET serverSocket;
	struct sockaddr_in server;

	static std::string format(std::string s1, std::string s2)
	{
		char* w = new char[s1.size() + 1];
		std::copy(s1.begin(), s1.end(), w);
		sprintf(w, s2.c_str());
		std::string result = w;
		delete[] w;
		return result;
	}
};


class Client
{

};