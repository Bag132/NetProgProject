#pragma once

#include <WinSock2.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>

#define DEFAULT_PORT 777
#define DEFAULT_PORT_UDP 778
#define DEFAULT_RECV_LEN 512
#define LEAVE_COMMAND "LEAVE"
#define LEAVE_COMMAND_SIZE 5
#define START_GAME_COMMAND "STARTGAME"
#define START_GAME_COMMAND_SIZE 10

#define JOIN_COMMAND "JOIN"
#define JOIN_COMMAND_SIZE 4
#define START_RECIEVED "STARTRECVD"
#define START_RECIEVED_SIZE 11

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
	Server();
	~Server();

	int Serve();
	int startGame(bool alone);
	bool IsListening();
	bool IsServing();
	bool serving = false;
	void Stop();
	
private:
	void recieveWorker(SOCKET*, SOCKET*);

	std::mutex waitForStart;
	u_short port;
	SOCKET serverSocket;
	struct sockaddr_in server;
	std::thread recieveThread, sendThread;
	bool listening = false,
		opponentJoined = false,
		inLobby = false,
		stopped = true,
		start = false,
		gameStarted = false;

};


class Client
{
public:
	Client();
	int Connect(std::string ip);
	int Send(std::string message);
	int Join();

private:
	void recieveWorker(SOCKET*);
	SOCKET serverSocket = 0;
	std::string ip;
};

