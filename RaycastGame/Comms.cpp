#define MAKEWORD(a,b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#include "Comms.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <errno.h>
#include <mutex>
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
Server::Server()
{
	waitForStart.lock();
	this->port = (u_short)777;
	WSA::GetInstance();

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror(format("Server: Could not create socket: %d", std::to_string(WSAGetLastError())).c_str());
		exit(1);
	}

	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(this->port);
}

/// <summary>
/// Starts and runs a game server
/// </summary>
/// <returns>Status code</returns>
int Server::Serve()
{
	stopped = false;
	if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		perror(std::string("Server: Couldn't bind to port ").append(std::to_string(port)).append(std::to_string((long double)WSAGetLastError())).c_str());
		return -1;
	}
	else
	{
		puts("Server: Binded to port\n");
	}
	this->listening = true;
	listen(serverSocket, 2); // blocking call
	this->serving = true;

	SOCKADDR_IN sockInfo = { 0 };
	int addrSize = sizeof(sockInfo);
	SOCKET* clientSocket = new SOCKET(accept(serverSocket, (struct sockaddr*)&sockInfo, &addrSize));
	if (*clientSocket == INVALID_SOCKET)
	{
		perror(format("Server: Accept failed with error code: %d\n", std::to_string((long double)WSAGetLastError())).c_str());
		return -2;
	}
	else
	{
		getpeername(*clientSocket, (struct sockaddr*)&sockInfo, &addrSize);
		SOCKADDR_IN* clientSockInfo = new SOCKADDR_IN(sockInfo);
		char ipbuf[15];
		const char* ip = inet_ntop(AF_INET, &clientSockInfo->sin_addr, ipbuf, 15);
		printf("Server: Connection recieved from '%s'\n", ipbuf);
		opponentJoined = true;

		// Send functionality here
		// Start recieve thread

		SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Setup UDP
		struct sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(DEFAULT_PORT_UDP);

		struct sockaddr_in opponent;
		int opponentLen = sizeof(opponent);
		opponent.sin_family = AF_INET;
		opponent.sin_port = htons(DEFAULT_PORT_UDP);
		inet_pton(AF_INET, ipbuf, &opponent.sin_addr.s_addr);

		bind(udpSocket, (struct sockaddr*)&server, sizeof(server));

		this->recieveThread = std::thread(&Server::recieveWorker, this, clientSocket, &udpSocket);

		printf("Server: Waiting for start...\n");

		//        while (!start); // Wait for user to start game
		waitForStart.lock();

		printf("Server: Game starting\n");

		if (send(*clientSocket, START_GAME_COMMAND, START_GAME_COMMAND_SIZE, 0) == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(*clientSocket);
			WSACleanup();
			return 1;
		}
		else
		{
			printf("Server: Sent start game command\n");

			

//			while (!gameStarted);
//			gameStarted = true;
			// Start UDP sends
			const char* testMessage = "From Server";
			printf("Server: Send loop started\n");
			while (true) // Check something idk 
			{
				// Send this player data
				if (sendto(udpSocket, testMessage, 12, 0, (struct sockaddr*)&opponent, sizeof(opponent)) == SOCKET_ERROR)
				{
					printf("Server: sendto() failed with error code: %d\n", WSAGetLastError());
				}
				else
				{
					printf("Server: Sent UDP message '%s'\n", testMessage);
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(30));
			}
		}

		if (shutdown(*clientSocket, SD_SEND) == SOCKET_ERROR)
		{
			printf("Server: recv failed with error: %d\n", WSAGetLastError());
			closesocket(*clientSocket);
			WSACleanup();
			return -4;
		}

		closesocket(*clientSocket);
		delete clientSocket;
		delete clientSockInfo;
	}

	return 0;
}

void Server::recieveWorker(SOCKET* clientSocket, SOCKET* udpSocket)
{
	char recvbuf[DEFAULT_RECV_LEN];
	char udpRecvBuf[50];
	struct sockaddr_in otherHost;
	int otherHostLen = sizeof(otherHost);
	int result = 1;
	printf("Server: Recieving messages\n");
	bool startRecieved = false;
	do {
		if (!startRecieved) { // TCP Game state stuff
			result = recv(*clientSocket, recvbuf, 50, 0); // Recieve commands from
			if (result > 0)
			{
				printf("Server: Recieved '%s'\n", recvbuf);
				if (strcmp(recvbuf, START_RECIEVED) == 0)
				{
					startRecieved = true;
					this->gameStarted = true;
					waitForStart.unlock();
					printf("Server: Client is ready to play\n");
					continue;
				}
			}
			else if (result == 0)
			{
				printf("Server: Connection closing\n");
			}
			else
			{
				printf("Server: recv failed with error: %d\n", WSAGetLastError());
				closesocket(*clientSocket);
				WSACleanup();
			}
		}
		else // UDP in-game stuff
		{
			// Recieve and update opponent data
			memset(udpRecvBuf, '\0', 50);
			if (recvfrom(*udpSocket, udpRecvBuf, 50, 0, (struct sockaddr*)&otherHost, &otherHostLen) == SOCKET_ERROR)
			{
				printf("Server: recvfrom() failed with error code : %d\n", WSAGetLastError());
			}
			else
			{
				printf("Server: UDP recieved: '%s'\n", udpRecvBuf);
			}
		}
	} while (result > 0);
}

int Server::startGame(bool alone)
{
	if (!this->opponentJoined && !alone)
	{
		return -1;
	}
	// Start game
	waitForStart.unlock();
	this->start = true;
	return 0;
}

bool Server::IsListening()
{
	return this->listening;
}

bool Server::IsServing()
{
	return this->serving;
}

void Server::Stop()
{
	this->stopped = true;
}

Server::~Server()
{
}

// ------------------- Client -------------------

Client::Client()
{
	WSA::GetInstance();
}

/// <summary>
/// Joins a host and runs client code
/// </summary>
/// <param name="ip">String representation of IPv4 address</param>
/// <param name="port">Port number</param>
/// <returns>Status code</returns>
int Client::Connect(std::string ip)
{
	SOCKET serverSock;
	if ((serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
		printf("Client: Could not create socket : %d", WSAGetLastError());
		return -1;
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(DEFAULT_PORT);
	int ret = inet_pton(AF_INET, ip.c_str(), (void*)&server.sin_addr.s_addr);
	if (ret < 0) {
		printf("Client: Bad return code %d", ret);
		return -1;
	}

	if (connect(serverSock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("Client: Connect failed\n");
		return -2;
	}

	// Stuff to correspond with server
	SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in udpServerInfo;
	udpServerInfo.sin_family = AF_INET;
	udpServerInfo.sin_addr.s_addr = INADDR_ANY;
	udpServerInfo.sin_port = htons(DEFAULT_PORT_UDP);
	bind(udpSocket, (struct sockaddr*) &udpServerInfo, sizeof(udpServerInfo));

	struct sockaddr_in opponentInfo;
	int opponentInfoLen = sizeof(opponentInfo);

	char recvbuf[50];
	int recvResult;
	bool gameStarted = false;
	do
	{
		printf("Client: Waiting for TCP message\n");
		recvResult = recv(serverSock, recvbuf, 50, 0);
		if (recvResult > 0)
		{
			printf("Client: Recieved '%s'\n", recvbuf);
			if (strcmp(recvbuf, START_GAME_COMMAND) == 0) // If start game is recieved then send start game recieved (not sure why I'm doing this)
			{
				gameStarted = true;

				opponentInfo.sin_family = AF_INET;
				opponentInfo.sin_port = htons(DEFAULT_PORT_UDP);
				inet_pton(AF_INET, ip.c_str(), &opponentInfo.sin_addr.s_addr);

				printf("Client: Start command recieved\n");
				if (send(serverSock, START_RECIEVED, START_RECIEVED_SIZE, 0) == SOCKET_ERROR)
				{
					printf("Client: Send failed with error: %d\n", WSAGetLastError());
					closesocket(serverSocket);
					WSACleanup();
					return 1;
				}
				else
				{
					printf("Client: Sent START_RECIEVED\n");
				}
				break; // Break loop to begin UDP sending
			}
		}
		else if (recvResult == 0)
		{
			printf("Client: Connection closing\n");
		}
		else
		{
			printf("Client: recv failed with error: %d\n", WSAGetLastError());
			closesocket(serverSocket);
			WSACleanup();
		}
	} while (recvResult > 0);

	const char* testMessage = "From client";
	printf("Client: UDP send loop started\n");
	std::thread recieveThread(&Client::recieveWorker, this, &udpSocket);
	while (gameStarted)
	{
		if (sendto(udpSocket, testMessage, 12, 0, (struct sockaddr*) &opponentInfo, opponentInfoLen) == SOCKET_ERROR)
		{
			printf("Client: sendto() failed with error %d\n", WSAGetLastError());
		}
		else
		{
			printf("Client: Sent '%s'\n", testMessage);
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}
	}
	printf("Client UDP loop ended\n");

	this->serverSocket = serverSock;
	return 0;
}

void Client::recieveWorker(SOCKET* udpSocket)
{
	const u_int recvBufSize = 50;
	char recvBuf[recvBufSize];
	sockaddr_in opponent;
	int opponentSize = sizeof(opponent);
	printf("Client: Recieve loop started\n");
	while (true)
	{
		memset(recvBuf, '\0', recvBufSize);
		if (recvfrom(*udpSocket, recvBuf, recvBufSize, 0, (sockaddr*)&opponent, &opponentSize) == SOCKET_ERROR)
		{
			printf("Client: recvfrom() failed with error code : %d\n", WSAGetLastError());
		}
		else
		{
			printf("Client: UDP recieved: '%s'\n", recvBuf);
		}
	}
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

int Client::Join()
{
	if (serverSocket == 0)
	{
		return -1;
	}

	if (send(serverSocket, JOIN_COMMAND, JOIN_COMMAND_SIZE, 0) == INVALID_SOCKET) {
		return -2;
	}
	else
	{
		std::cout << "Client: Sent join command\n";
	}

	return 0;
}
