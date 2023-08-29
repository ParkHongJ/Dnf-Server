#pragma once
#include "Server_Defines.h"
class Session
{
public:
	Session(SOCKET clientSocket);
	void Send(BYTE* sendBuffer);

	SOCKET GetSocket() { return socket; }

	bool Recv();
private:
	SOCKET socket = INVALID_SOCKET;
};

