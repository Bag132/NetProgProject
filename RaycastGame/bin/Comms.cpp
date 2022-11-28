#define MAKEWORD(a,b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define UDP_BUFFER_SIZE 100

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
		

		// LOCAL_DEBUG UDP: Client sends to 780, Server Recieves at 780
		//					Server sends to 770, Client recieves at 770

		struct sockaddr_in udpServerInfo;
		udpServerInfo.sin_family = AF_INET;
		udpServerInfo.sin_addr.s_addr = INADDR_ANY;
		if (LOCAL_DEBUG)
		{
			udpServerInfo.sin_port = htons(780);
		}
		else
		{
			udpServerInfo.sin_port = htons(DEFAULT_PORT_UDP);
		}

		struct sockaddr_in opponentUdpInfo;
		int opponentLen = sizeof(opponentUdpInfo);
		opponentUdpInfo.sin_family = AF_INET;
		if (LOCAL_DEBUG)
		{
			opponentUdpInfo.sin_port = htons(770);
		}
		else 
		{
			opponentUdpInfo.sin_port = htons(DEFAULT_PORT_UDP);
		}
		inet_pton(AF_INET, ipbuf, &opponentUdpInfo.sin_addr.s_addr);

		SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // Setup UDP
		bind(udpSocket, (struct sockaddr*)&udpServerInfo, sizeof(udpServerInfo));

		this->recieveThread = std::thread(&Server::recieveWorker, this, clientSocket, &udpSocket);

		printf("Server: Waiting for start...\n");
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

			

			// Start UDP sends
			const char* testMessage = "From Server";

			printf("Server: Send loop started\n");
			while (true) // Check something idk 
			{
				std::string stateStr =
					std::to_string(this->currentState.xPos) + " " 
					+ std::to_string(this->currentState.yPos) + " " 
					+ std::to_string(this->currentState.orientation) + " "
					+ (this->currentState.acting ? "t" : "f");
				// Send this player data
				if (sendto(udpSocket, stateStr.c_str(), stateStr.size(), 0, (struct sockaddr*)&opponentUdpInfo, sizeof(opponentUdpInfo)) == SOCKET_ERROR)
				{
					printf("Server: sendto() failed with error code: %d\n", WSAGetLastError());
				}
				else
				{
					printf("Server: Sent UDP message '%s'\n", stateStr.c_str());
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
	char udpRecvBuf[UDP_BUFFER_SIZE];
	struct sockaddr_in otherHost;
	int otherHostLen = sizeof(otherHost);
	int result = 1;
	printf("Server: Recieving messages\n");
	bool startRecieved = false;

	do {
		if (!startRecieved) { // TCP Game state stuff
			result = recv(*clientSocket, recvbuf, DEFAULT_RECV_LEN, 0); // Recieve commands from
			if (result > 0)
			{
				printf("Server: Recieved '%s'\n", recvbuf);
				if (strcmp(recvbuf, START_RECIEVED) == 0)
				{
					startRecieved = true;
					this->gameStarted = true;
//					waitForStart.unlock();
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
			memset(udpRecvBuf, '\0', UDP_BUFFER_SIZE);
			if (recvfrom(*udpSocket, udpRecvBuf, UDP_BUFFER_SIZE, 0, (struct sockaddr*)&otherHost, &otherHostLen) == SOCKET_ERROR)
			{
				printf("Server: recvfrom() failed with error code : %d\n", WSAGetLastError());
			}
			else
			{
//				printf("Server: UDP recieved: '%s'\n", udpRecvBuf);
				std::string recvStr(udpRecvBuf);

				int xPosEnd = recvStr.find(" ", 0);
				std::string xStr = recvStr.substr(0, xPosEnd);

				int yPosEnd = recvStr.find(" ", xPosEnd);
				std::string yStr = recvStr.substr(xPosEnd, yPosEnd);

				int orEnd = recvStr.find(" ", yPosEnd);
				std::string orStr = recvStr.substr(yPosEnd, orEnd);

				std::string actStr = recvStr.substr(orEnd + orStr.size() + 1, recvStr.size());

				float x = std::stof(xStr), y = std::stof(yStr), orientation = std::stof(orStr);
				bool acting = (actStr == "t" ? true : false);

				printf("Server: Opp X: %f, Opp Y: %f, Opp ori: %f\n", x, y, orientation);
				
				this->opponentState.xPos = x;
				this->opponentState.yPos = y;
				this->opponentState.orientation = orientation;
				this->opponentState.acting = acting;
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

bool Server::ClientJoined()
{
	return this->opponentJoined;
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

void Server::SetPlayerState(PlayerState ps)
{
	this->currentState = ps;
}

void Server::SetPlayerState(float x, float y, float orientation)
{
	this->currentState.xPos = x;
	this->currentState.yPos = y;
	this->currentState.orientation = orientation;
}

PlayerState Server::GetOpponentState()
{
	return this->opponentState;
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
	inet_pton(AF_INET, ip.c_str(), (void*)&server.sin_addr.s_addr);

	if (connect(serverSock, (struct sockaddr*)&server, sizeof(server)) < 0) {
		printf("Client: Connect failed %d\n", WSAGetLastError());
		return -2;
	}
	else
	{
		this->joinedServer = true;
		printf("Client: Connected to server\n");
	}

	// LOCAL_DEBUG UDP: Client sends to 780, Server Recieves at 780
	// Server sends to 770, Client recieves at 770

	SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in udpServerInfo;
	udpServerInfo.sin_family = AF_INET;
	udpServerInfo.sin_addr.s_addr = INADDR_ANY;
	if (LOCAL_DEBUG)
	{
		udpServerInfo.sin_port = htons(770);
	}
	else
	{
		udpServerInfo.sin_port = htons(DEFAULT_PORT_UDP);
	}

	struct sockaddr_in opponentInfo;
	int opponentInfoLen = sizeof(opponentInfo);
	opponentInfo.sin_family = AF_INET;
	if (LOCAL_DEBUG)
	{
		opponentInfo.sin_port = htons(780);
	} 
	else {
		opponentInfo.sin_port = htons(DEFAULT_PORT_UDP);
	}
	inet_pton(AF_INET, ip.c_str(), &opponentInfo.sin_addr.s_addr);

	bind(udpSocket, (struct sockaddr*) &udpServerInfo, sizeof(udpServerInfo));

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

	std::string stateStr =
		std::to_string(this->currentState.xPos) + " " 
		+ std::to_string(this->currentState.yPos) + " " 
		+ std::to_string(this->currentState.orientation) + " "
		+ (this->currentState.acting ? "t" : "f");

	while (gameStarted)
	{
		if (sendto(udpSocket, stateStr.c_str(), stateStr.size(), 0, (struct sockaddr*)&opponentInfo, opponentInfoLen) == SOCKET_ERROR)
		{
			printf("Client: sendto() failed with error %d\n", WSAGetLastError());
		}
		else
		{
			printf("Client: Sent '%s'\n", stateStr.c_str());
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
//			printf("Client: UDP recieved: '%s'\n", recvBuf);
			std::string recvStr(recvBuf);

			int xPosEnd = recvStr.find(" ", 0);
			std::string xStr = recvStr.substr(0, xPosEnd);

			int yPosEnd = recvStr.find(" ", xPosEnd);
			std::string yStr = recvStr.substr(xPosEnd, yPosEnd);

			std::string orStr = recvStr.substr(yPosEnd + yStr.size() + 1, recvStr.size());

			float x = std::stof(xStr), y = std::stof(yStr), orientation = std::stof(orStr);

			printf("Client: Opp X: %f, Opp Y: %f, Opp ori: %f\n", x, y, orientation);

			this->opponentState.xPos = x;
			this->opponentState.yPos = y;
			this->opponentState.orientation = orientation;
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

bool Client::JoinedServer()
{
	return this->joinedServer;
}

void Client::SetPlayerState(PlayerState ps)
{
	this->currentState = ps;
}

void Client::SetPlayerState(float x, float y, float orientation)
{
	this->currentState.xPos = x;
	this->currentState.yPos = y;
	this->currentState.orientation = orientation;
}

PlayerState Client::GetOpponentState()
{
	return this->opponentState;
}
