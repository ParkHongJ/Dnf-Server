#pragma once
#include <winsock2.h>
#include <queue>
#define PORT 7777
#define PACKET_SIZE 512

class ServerManager
{
public:
	ServerManager();
	~ServerManager();

	void Connect();
	void RecvThread(SOCKET Socket);

private:
	SOCKET clientSocket;
	std::queue<BYTE*> Packets;
};

